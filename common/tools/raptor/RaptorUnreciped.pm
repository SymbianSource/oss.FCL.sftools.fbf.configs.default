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
# Extract, analyzes and dumps text in <buildlog> context which doesn't belong to any <recipe> tags

package RaptorUnreciped;

use strict;
use RaptorCommon;

our $reset_status = {};
my $buildlog_status = {};
my $buildlog_subtag_status = {};

$reset_status->{name} = 'reset_status';
$reset_status->{next_status} = {buildlog=>$buildlog_status};

$buildlog_status->{name} = 'buildlog_status';
$buildlog_status->{next_status} = {'?default?'=>$buildlog_subtag_status};
$buildlog_status->{on_start} = 'RaptorUnreciped::on_start_buildlog';
$buildlog_status->{on_end} = 'RaptorUnreciped::on_end_buildlog';
$buildlog_status->{on_chars} = 'RaptorUnreciped::on_chars_buildlog';

$buildlog_subtag_status->{name} = 'buildlog_subtag_status';
$buildlog_subtag_status->{next_status} = {};

my $characters = '';

sub on_start_buildlog
{
	my $filename = "$::basedir/unreciped.txt";
	print "Writing unreciped file $filename\n" if (!-f$filename);
	open(FILE, ">>$filename");
}

sub on_chars_buildlog
{
	my ($ch) = @_;
	
	my $characters = $ch->{Data};
	
	print FILE $characters if ($characters =~ m,[^\s^\r^\n],);
	
	#print "characters is now -->$characters<--\n";
}

sub on_end_buildlog
{
	close(FILE);
}


1;