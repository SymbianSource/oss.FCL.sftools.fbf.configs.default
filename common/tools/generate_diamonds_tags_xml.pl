#! perl -w
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
# Generate an XML file in the Diamonds format to upload build tags to Diamonds

use strict;

use Getopt::Long;

my $taglist = 0;
my $outputfile = 0;
my $help = 0;
GetOptions((
	'tags:s' => \$taglist,
	'output:s' => \$outputfile,
	'help!' => \$help,
));

if ($help or !$outputfile)
{
	print "Usage: perl generate_diamonds_tags_xml.pl --tags COMMA_SEPARATED_TAG_LIST --output OUTPUT_FILE\n";
}

open(FILE, ">$outputfile");

print FILE <<_END_HEADER;
<?xml version=\"1.0\" encoding=\"utf-8\"?>
<diamonds-build>
  <schema>13</schema>
  <tags>
_END_HEADER

my @tags = split(/,/, $taglist);
for (@tags)
{
	print FILE "    <tag>$_</tag>\n";
}

print FILE <<_END_TRAILER;  
  </tags>
</diamonds-build>
_END_TRAILER

close(FILE);
