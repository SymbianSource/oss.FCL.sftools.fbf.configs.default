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
# Preprocess a raptor log, trying to countermeasure a list of known anomalies

use strict;

use Getopt::Long;

my $infile = '';
my $outfile = '';
my $basedir = '';
my $help = 0;
GetOptions((
	'in:s' => \$infile,
	'out:s' => \$outfile,
	'help!' => \$help
));

$help = 1 if (!$infile);

if ($help)
{
	print "Preprocess a raptor log, trying to countermeasure a list of known anomalies\n";
	print "Usage: perl preprocess_log.pl --in=INFILE --out=OUTFILE\n";
	exit(0);
}

open(INFILE, $infile);
open(OUTFILE, ">$outfile");

for my $line (<INFILE>)
{
	if ($line =~ m,<[^<^>]+>.*&.*</[^<^>]+>,)
	{
		$line = escape_ampersand($line);
	}
	elsif ($line =~ m,<\?xml\s.*encoding=.*\".*\?>,)
	{
		$line = set_encoding_utf8($line);
	}
	
	print OUTFILE $line;
}

close(OUTFILE);
close(INFILE);


sub escape_ampersand
{
	my ($line) = @_;
	
	print "escape_ampersand\n";
	print "in: $line";
	
	$line =~ s,&,&amp;,g;
	
	print "out: $line";
	return $line;
}

sub set_encoding_utf8
{
	my ($line) = @_;
	
	print "set_encoding_utf8\n";
	print "in: $line";
	
	$line =~ s,encoding=".*",encoding="utf-8",;
	
	print "out: $line";
	return $line;
}