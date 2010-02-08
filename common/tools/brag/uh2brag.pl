#!perl -w
#
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
# Generate a BRAG-compatible XML summary from an index.html file coming out of the uh parser

use strict;

use Getopt::Long;

my $raptorSummary;
my $help = 0;
GetOptions((
	'index=s' => \$raptorSummary,
	'help!' => \$help
));

$help = 1 if (!$raptorSummary);
if ($help)
{
	print "Generate an XML summary of the Raptor build from a summary.csv file\n";
	print "Usage: perl summarize.pl --index=INDEXFILE\n";
	exit(0);
}

my $criticals = 0;
my $majors = 0;
my $minors = 0;
my $unknowns = 0;

# READ INDEX.HTML FILE
if (open(INDEX, $raptorSummary))
{
	while (my $line = <INDEX>)
	{
		if ($line =~ m{<tr><td><a href='.*'>.*</a></td><td>(\d+)</td><td>(\d+)</td><td>(\d+)</td><td>(\d+)</td></tr>})
		{
			$criticals += $1 if ($1);
			$majors += $2 if ($2);
			$minors += $3 if ($3);
			$unknowns += $4 if ($4);
		}
	}
	close(INDEX);
}

# Print XML
print <<_END;
<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<?xml-stylesheet type='text/xsl' href='brag.xsl'?>
<buildStatus>
<phase name="Build">
<step name=\"Raptor Build\" detailshref=\"..\\html\\index.html\">
<failures level=\"critical\" count=\"$criticals\"/>
<failures level=\"major\" count=\"$majors\"/>
<failures level=\"minor\" count=\"$minors\"/>
<failures level=\"unknown\" count=\"$unknowns\"/>
</step>
</phase>
</buildStatus>
_END

exit(0);

