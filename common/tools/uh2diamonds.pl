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
# Translates uh report (index.html) to a Diamonds file

use strict;

use Getopt::Long;

my $input = "\\output\\logs\\html\\index.html";
my $output = "\\output\\logs\\diamonds_uh.xml";
my $bit_output = "\\output\\logs\\bitinfo.txt";
my $help = 0;
GetOptions((
	'in=s' => \$input,
	'out=s' => \$output,
	'help!' => \$help
));

if ($help)
{
	print "Translates uh report (index.html) to a Diamonds file\n";
	print "Usage: perl uh2diamonds.pl [--in=UHINDEX] [--out=XMLFILE]\n";
	print "\n";
	print "UHINDEX is optional, its default is \\output\\logs\\html\\index.html\n";
	print "XMLFILE is optional, its default is \\output\\logs\\diamonds_uh.xml\n";
	exit(0);
}

my $critical = 0;
my $major = 0;
my $minor = 0;
my $unknown = 0;
my $missing = 0;

open(INDEX, $input) or die "Can't open $input for reading";
while (<INDEX>)
{
	my $line = $_;
	if ($line =~ m,<tr><td><a href='.*'>\w+</a></td><td>(\d+)</td><td>(\d+)</td><td>(\d+)</td><td>(\d+)</td></tr>,)
	{
		$critical += $1;
		$major += $2;
		$minor += $3;
		$unknown += $4;
	}
	elsif($line =~ m,<tr><td><a href='.+'>.+</a></td><td>(\d+)</td><td>(\d+)</td><td>(\d+)</td><td>(\d+)</td><td>(\d+)</td></tr>,)
	{
		$critical += $1;
		$major += $2;
		$minor += $3;
		$unknown += $4;
		$missing += $5;
	}
}
close(INDEX);

my $xml_content = <<_EOX;

<?xml version=\"1.0\" encoding=\"utf-8\"?>
<diamonds-build>
  <schema>14</schema>
  <faults>
    <total severity=\"Missing Artifacts\">$missing</total>
    <total severity=\"Raptor Build Unknown\">$unknown</total>
    <total severity=\"Raptor Build Minor\">$minor</total>
    <total severity=\"Raptor Build Major\">$major</total>
    <total severity=\"Raptor Build Critical\">$critical</total>
  </faults>
</diamonds-build>
_EOX

if (open(XML, ">$output"))
{
	print XML $xml_content;
	close(XML);
	print "Wrote Diamonds file: $output\n";
}
else
{
	warn "Could not write to file: $output\n";
}

if (open(BITINFO, ">>$bit_output"))
{
	print BITINFO "failure\tmissing,$missing\n";
	print BITINFO "failure\tminor,$minor\n";
	print BITINFO "failure\tmajor,$major\n";
	print BITINFO "failure\tcritical,$critical\n";
	close(BITINFO);
}