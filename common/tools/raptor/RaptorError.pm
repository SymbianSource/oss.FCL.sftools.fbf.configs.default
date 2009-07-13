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

$buildlog_error_status->{name} = 'buildlog_error_status';
$buildlog_error_status->{next_status} = {};
$buildlog_error_status->{on_start} = 'RaptorError::on_start_buildlog_error';
$buildlog_error_status->{on_end} = 'RaptorError::on_end_buildlog_error';
$buildlog_error_status->{on_chars} = 'RaptorError::on_chars_buildlog_error';

my $characters = '';

my $category = $RaptorCommon::CATEGORY_RAPTORERROR;

sub process
{
	my ($text) = @_;
	
	my $severity = $RaptorCommon::SEVERITY_UNKNOWN;
	
	if ($text =~ m,Cannot process schema version .* of file,)
	{
		$severity = $RaptorCommon::SEVERITY_CRITICAL;
		
		#dump_error($category, $severity, $text);
		print "$category, $severity, $text\n";
	}
}

sub on_start_buildlog_error
{
	my $filename = "$::basedir/errors.txt";
	print "Writing error file $filename\n" if (!-f$filename);
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
	
	process($characters);
	
	print FILE $characters if ($characters =~ m,[^\s^\r^\n],);
	print FILE "\n" if ($characters !~ m,[\r\n]$, );
	
	$characters = '';
	
	close(FILE);
}


1;