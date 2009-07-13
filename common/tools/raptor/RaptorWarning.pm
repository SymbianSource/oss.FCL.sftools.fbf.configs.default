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
# Extract, analyzes and dumps raptor warnings i.e. content of <warning> tags from a raptor log file

package RaptorWarning;

use strict;
use RaptorCommon;

our $reset_status = {};
my $buildlog_status = {};
my $buildlog_warning_status = {};

$reset_status->{name} = 'reset_status';
$reset_status->{next_status} = {buildlog=>$buildlog_status};

$buildlog_status->{name} = 'buildlog_status';
$buildlog_status->{next_status} = {warning=>$buildlog_warning_status};

$buildlog_warning_status->{name} = 'buildlog_warning_status';
$buildlog_warning_status->{next_status} = {};
$buildlog_warning_status->{on_start} = 'RaptorWarning::on_start_buildlog_warning';
$buildlog_warning_status->{on_end} = 'RaptorWarning::on_end_buildlog_warning';
$buildlog_warning_status->{on_chars} = 'RaptorWarning::on_chars_buildlog_warning';

my $characters = '';

my $category = $RaptorCommon::CATEGORY_RAPTORWARNING;

sub process
{
	my ($text) = @_;
	
	my $severity = $RaptorCommon::SEVERITY_UNKNOWN;
	
	if ($text =~ m,unmatchable,)
	{
		$severity = $RaptorCommon::SEVERITY_CRITICAL;
		
		#dump_error($category, $severity, $text);
		print "$category, $severity, $text\n";
	}
}

sub on_start_buildlog_warning
{
	my $filename = "$::basedir/warnings.txt";
	print "Writing warning file $filename\n" if (!-f$filename);
	open(FILE, ">>$filename");
}

sub on_chars_buildlog_warning
{
	my ($ch) = @_;
	
	#print "on_chars_buildlog_warning\n";
	
	$characters .= $ch->{Data};
	
	#print "characters is now -->$characters<--\n";
}

sub on_end_buildlog_warning
{
	#print "on_end_buildlog_warning\n";
	
	process($characters);
	
	print FILE $characters if ($characters =~ m,[^\s^\r^\n],);
	print FILE "\n" if ($characters !~ m,[\r\n]$, );
	
	$characters = '';
	
	close(FILE);
}


1;