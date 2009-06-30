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
# Extracts output text in <buildlog> context which doesn't belong to <recipe>'s

use strict;

use XML::SAX;
use RaptorSAXHandler;
use Getopt::Long;

my @logfiles;
my $basedir = '';
my $help = 0;
GetOptions((
	'log:s' => \@logfiles,
	'basedir:s' => \$basedir,
	'help!' => \$help
));

$help = 1 if (!@logfiles);

if ($help)
{
	print "Extracts text which doesn't belong to recipes from a raptor log file\n";
	print "Usage: perl unreciped_text.pl --log=FILE1 --log=FILE2 [OPTIONS]\n";
	print "where OPTIONS are:\n";
	print "\t--basedir=DIR Generate output file under DIR\n";
	exit(0);
}

my $reset_status = {};
my $buildlog_status = {};
my $buildlog_subtag_status = {};

$reset_status->{name} = 'reset_status';
$reset_status->{next_status} = {buildlog=>$buildlog_status};

$buildlog_status->{name} = 'buildlog_status';
$buildlog_status->{next_status} = {'?default?'=>$buildlog_subtag_status};
$buildlog_status->{on_start} = 'main::on_start_buildlog';
$buildlog_status->{on_end} = 'main::on_end_buildlog';
$buildlog_status->{on_chars} = 'main::on_chars_buildlog';

$buildlog_subtag_status->{name} = 'buildlog_subtag_status';
$buildlog_subtag_status->{next_status} = {};

my $characters = '';

if (!$basedir)
{
	$basedir = time;
	
	print "Using $basedir as basedir.\n";
}
if (-d $basedir)
{
	print "Directory $basedir exists. Quitting.\n";
	exit(1);
}
mkdir($basedir);
#print "Created dir $basedir.\n";

my $saxhandler = RaptorSAXHandler->new();
$saxhandler->set_init_status($reset_status);
my $parser = XML::SAX::ParserFactory->parser(Handler=>$saxhandler);
for (@logfiles)
{
	$parser->parse_uri($_);
}


sub on_start_buildlog
{
	my $filename = "$basedir/unreciped.txt";
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