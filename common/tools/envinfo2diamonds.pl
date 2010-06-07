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
# Translates envinfo report to a Diamonds file

use strict;

use Getopt::Long;

my $input = "\\output\\logs\\envinfo.txt";
my $output = "\\output\\logs\\diamonds_envinfo.xml";
my $help = 0;
GetOptions((
	'in=s' => \$input,
	'out=s' => \$output,
	'help!' => \$help
));

if ($help)
{
	print "Translates envinfo report to a Diamonds file\n";
	print "Usage: perl envinfo2diamonds.pl [-i INFILE] [-o OUTFILE]\n";
	print "\n";
	print "INFILE is optional, its default is \\output\\logs\\envinfo.txt\n";
	print "OUTFILE is optional, its default is \\output\\logs\\diamonds_envinfo.xml\n";
	exit(0);
}

my @environment_info = ();

open(INFILE, $input) or die "Can't open $input for reading";
while (<INFILE>)
{
	my $line = $_;
	if ($line =~ /([^\t]*)\t([^\t]*)/)
	{
		my $name = $1;
		my $version = $2;
		chomp $name;
		chomp $version;
		push @environment_info, {name=>$name, version=>$version};
	}
}
close(INFILE);

# write diamonds file
@environment_info = reverse(@environment_info);

my $xml_content = <<_EOX;
<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<diamonds-build>
<schema>10</schema>
<tools>        
_HERE_TOOLS_LINES_
</tools>
</diamonds-build>
_EOX

my $tools_lines = '';
for my $tool_info (@environment_info)
{
	$tools_lines .= "   <tool><name>$tool_info->{name}</name><version>$tool_info->{version}</version></tool>\n";
}

$xml_content =~ s/_HERE_TOOLS_LINES_/$tools_lines/;

if (open(ENVINFO, ">$output"))
{
	print ENVINFO $xml_content;
	close(ENVINFO);
	print "Wrote Diamonds file: $output\n";
}
else
{
	warn "Could not write to file: $output\n";
}
