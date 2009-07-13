# Copyright (c) 2009 Symbian Foundation Ltd
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Symbian Foundation Ltd - initial contribution.
#
# Contributors:
#
# Description:
# Raptor parser module.
# Extract, analyzes and dumps raptor recipes i.e. content of <recipe> tags from a raptor log file

package RaptorRecipe;

use strict;
use RaptorCommon;

our $reset_status = {};
my $buildlog_status = {};
my $buildlog_recipe_status = {};

$reset_status->{name} = 'reset_status';
$reset_status->{next_status} = {buildlog=>$buildlog_status};

$buildlog_status->{name} = 'buildlog_status';
$buildlog_status->{next_status} = {recipe=>$buildlog_recipe_status};

$buildlog_recipe_status->{name} = 'buildlog_recipe_status';
$buildlog_recipe_status->{next_status} = {};
$buildlog_recipe_status->{on_start} = 'RaptorRecipe::on_start_buildlog_recipe';
$buildlog_recipe_status->{on_end} = 'RaptorRecipe::on_end_buildlog_recipe';
$buildlog_recipe_status->{on_chars} = 'RaptorRecipe::on_chars_buildlog_recipe';

my $characters = '';
my $recipe_info = {};

my $category = $RaptorCommon::CATEGORY_RAPTORRECIPE;

sub process
{
	my ($text) = @_;
	
	my $severity = $RaptorCommon::SEVERITY_UNKNOWN;
	
	if ($text =~ m,Cannot process schema version .* of file,)
	{
		$severity = $RaptorCommon::SEVERITY_CRITICAL;
		
		#dump_recipe($category, $severity, $text);
		print "$category, $severity, $text\n";
	}
}

sub on_start_buildlog_recipe
{
	my ($el) = @_;
	
	$recipe_info = {};
	$characters = '';
	
	my $attrstring = '';
	my $bldinf = '';
	
	my $attributes = $el->{Attributes};
	for (keys %{$attributes})
	{
		if ($attributes->{$_}->{'LocalName'} eq 'bldinf')
		{
			$bldinf = $attributes->{$_}->{'Value'};
		}
		
		$attrstring .= "$attributes->{$_}->{'LocalName'}}='$attributes->{$_}->{'Value'}' ";
		
	}
	
	if ($bldinf eq '')
	{
		print "WARNING: recipe tag with no bldinf attribute. Associating to package unknown/unknown\n";
		$bldinf = "/sf/unknown/unknown/group/bld.inf";
	}
	
	$recipe_info->{bldinf} = $bldinf;
	
	$characters = "<recipe $attrstring>\n";
}

sub on_chars_buildlog_recipe
{
	my ($ch) = @_;
	
	#print "on_chars_buildlog_recipe\n";
	
	$characters .= $ch->{Data};
	
	#print "characters is now -->$characters<--\n";
}

sub on_end_buildlog_recipe
{
	#print "on_end_buildlog_recipe\n";
	
	$characters .= "\n</recipe>\n";
	
	my $normalized = lc($recipe_info->{bldinf});
	$normalized =~ s,^[A-Za-z]:,,;
	$normalized =~ s,[\\],/,g;
	
	$normalized =~ m,^/sf/([^/]+)/([^/]+)/,;
	my $layer = $1;
	my $package = $2;
	
	mkdir("$::basedir/recipes");
	mkdir("$::basedir/recipes/$layer");
	mkdir("$::basedir/recipes/$layer/$package");
	
	my $filename = "$::basedir/recipes/$layer/$package/recipes.txt";
	
	print "Writing recipes file $filename\n" if (!-f$filename);
	open(FILE, ">>$filename");
	print FILE $characters;
	close(FILE);
}


1;