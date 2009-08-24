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
# Extract, analyzes and dumps raptor errors i.e. content of <error> tags from a raptor log file

package RaptorError;

use strict;
use RaptorCommon;

our $reset_status = {};
my $buildlog_status = {};
my $buildlog_error_status = {};

$reset_status->{name} = 'reset_status';
$reset_status->{next_status} = {buildlog=>$buildlog_status};

$buildlog_status->{name} = 'buildlog_status';
$buildlog_status->{next_status} = {error=>$buildlog_error_status};
$buildlog_status->{on_start} = 'RaptorError::on_start_buildlog';

$buildlog_error_status->{name} = 'buildlog_error_status';
$buildlog_error_status->{next_status} = {};
$buildlog_error_status->{on_start} = 'RaptorError::on_start_buildlog_error';
$buildlog_error_status->{on_end} = 'RaptorError::on_end_buildlog_error';
$buildlog_error_status->{on_chars} = 'RaptorError::on_chars_buildlog_error';

my $filename = '';

my $characters = '';

my $category = $RaptorCommon::CATEGORY_RAPTORERROR;

sub process
{
	my ($text, $component, $phase, $recipe, $file, $line) = @_;
	
	my $severity = $RaptorCommon::SEVERITY_UNKNOWN;
	
	if ($text =~ m,Cannot process schema version .* of file,)
	{
		$severity = $RaptorCommon::SEVERITY_CRITICAL;
		my $subcategory = $RaptorCommon::CATEGORY_RAPTORERROR_CANNOTPROCESSSCHEMAVERSION;
		RaptorCommon::dump_fault($category, $subcategory, $severity, $component, $phase, $recipe, $file, $line);
	}
	elsif ($text =~ m,No bld\.inf found at,)
	{
		$severity = $RaptorCommon::SEVERITY_MAJOR;
		my $subcategory = $RaptorCommon::CATEGORY_RAPTORERROR_NOBLDINFFOUND;
		RaptorCommon::dump_fault($category, $subcategory, $severity, $component, $phase, $recipe, $file, $line);
	}
	elsif ($text =~ m,Can't find mmp file,)
	{
		$severity = $RaptorCommon::SEVERITY_NORMAL;
		my $subcategory = $RaptorCommon::CATEGORY_RAPTORERROR_CANTFINDMMPFILE;
		RaptorCommon::dump_fault($category, $subcategory, $severity, $component, $phase, $recipe, $file, $line);
	}
	elsif ($text =~ m,The make-engine exited with errors,)
	{
		$severity = $RaptorCommon::SEVERITY_CRITICAL;
		my $subcategory = $RaptorCommon::CATEGORY_RAPTORERROR_MAKEEXITEDWITHERRORS;
		RaptorCommon::dump_fault($category, $subcategory, $severity, $component, $phase, $recipe, $file, $line);
	}
	elsif ($text =~ m,tool .* from config .* did not return version .* as required,)
	{
		$severity = $RaptorCommon::SEVERITY_CRITICAL;
		my $subcategory = $RaptorCommon::CATEGORY_RAPTORERROR_TOOLDIDNOTRETURNVERSION;
		RaptorCommon::dump_fault($category, $subcategory, $severity, $component, $phase, $recipe, $file, $line);
	}
	else # log everything by default
	{
		$severity = $RaptorCommon::SEVERITY_NORMAL;
		my $subcategory = '';
		RaptorCommon::dump_fault($category, $subcategory, $severity, $component, $phase, $recipe, $file, $line);
	}
}

sub on_start_buildlog
{
	RaptorCommon::init();
	
	$filename = "$::basedir/errors.txt";
	if (!-f$filename)
	{
		print "Writing errors file $filename\n";
		open(FILE, ">$filename");
		close(FILE);
	}
}

sub on_start_buildlog_error
{
	open(FILE, ">>$filename");
}

sub on_chars_buildlog_error
{
	my ($ch) = @_;
	
	#print "on_chars_buildlog_error\n";
	
	$characters .= $ch->{Data};
	
	#print "characters is now -->$characters<--\n";
}

sub on_end_buildlog_error
{
	#print "on_end_buildlog_error\n";
	
	print FILE $characters if ($characters =~ m,[^\s^\r^\n],);
	print FILE "\n" if ($characters !~ m,[\r\n]$, );
	close(FILE);
	
	# get the line number - not really optimized
	my $linecount = 0;
	open(FILE, "$filename");
	for ($linecount = 0; <FILE>; $linecount++) { }
	close(FILE);
	
	process($characters, '', '', '', "errors.txt", $linecount);
	
	$characters = '';
}


1;