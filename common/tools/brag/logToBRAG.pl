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
# Identifies failures in a log file (as determined by a set of rules) and produces a BRAG XML summary

use strict;

use Getopt::Long;

my $phaseName;
my $stepName;
my $rulesFile;
my $help = 0;
GetOptions((
	'phase=s' => \$phaseName,
	'step=s' => \$stepName,
	'rules=s' => \$rulesFile,
	'help!' => \$help,
));

my $wrongArgs = 0;
unless ($help)
{
	$wrongArgs += warn "No phase specified to indicate the phase that the failures fall under\n" unless defined $phaseName;
	$wrongArgs += warn "No step specified to indicate the step that the failures fall under\n" unless defined $stepName;
	$wrongArgs += warn "No file of rules specified to interpret the log file\n" unless defined $rulesFile;
	$wrongArgs += warn "No log files to process\n" unless @ARGV;
}
if ($help || $wrongArgs)
{
	print <<"EOT";

logToBRAG.pl --phase=prebuild --step=syncsource --rules=syncsourceRules.tsv presync.log [doSync.log ...] > output.xml
EOT
	exit(0 + !$help);
}

# Start to build structure to be output as XML (same format as XML::Parser would create for us)
my $xmlNewline = bless { Text => "\n" }, "Characters";
my $step = bless {
	name => $stepName,
	Kids => [ $xmlNewline ]
}, "step";
my $phase = bless {
	name => $phaseName,
	Kids => [ $xmlNewline, $step, $xmlNewline ]
}, "phase";
my $buildStatus = [ bless {
	Kids =>
	[
		$xmlNewline,
		$phase,
		$xmlNewline,
	]
}, "buildStatus"];

# Also create empty <failures> tags with severities in a sensible order
# And shortcuts to those items
my $severityShortcut = {};
foreach my $severity (qw{critical major minor})
{
	my $failureSet = bless { level => $severity, Kids => [ $xmlNewline ] }, "failures";
	push @{$step->{Kids}}, $failureSet, $xmlNewline;
	$severityShortcut->{$severity} = $failureSet;
}

# Read rules file
my @rules;
open(TSV, $rulesFile) or die "Unable to open rules file '$rulesFile'\n";
while (my $line = <TSV>)
{
	chomp $line;
	next unless $line;
	my @terms = split m{\t+}, $line;
	die "Rules file not formatted as expected at line $.\n" unless scalar @terms == 2;
	push @rules, { regexp => qr{$terms[0]}, severity => $terms[1] };
}	

# Iterate through all the lines of all the files in @ARGV
while (my $line = <>)
{
	chomp $line;
	foreach my $rule (@rules)
	{
		if ($line =~ m[$rule->{regexp}])
		{
			last if $rule->{severity} eq "ignore";
			# We found a match
			my $failure = bless{ Kids => [ bless { Text => $line }, "Characters" ] }, "failure";
			# Ensure we have a <failures> tag for this severity
			if (!exists $severityShortcut->{$rule->{severity}})
			{
				# Create the failure set and add it to the shortcut list too
				my $failureSet = bless { level => $rule->{severity}, Kids => [ $xmlNewline ] }, "failures";
				push @{$step->{Kids}}, $failureSet, $xmlNewline;
				$severityShortcut->{$rule->{severity}} = $failureSet;
			}
			push @{$severityShortcut->{$rule->{severity}}->{Kids}}, $failure, $xmlNewline;
			# Do not consider any more rules
			last;
		}
	}
}

# Print XML
print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
print "<?xml-stylesheet type='text/xsl' href='brag.xsl'?>\n";
printTree($buildStatus->[0]);
print "\n";

exit(0);

sub printTree
{
	my $tree = shift or die;
	die unless ref $tree;

	my $tagName = ref $tree;
	$tagName =~ s{^main::}{};
	if ($tagName eq "Characters")
	{
		print $tree->{Text};
		return;
	}
	
	print "<$tagName";

	foreach my $attr (
		sort {
			my $order = "name level start stop href";
			my $ixA = index $order, $a;
			my $ixB = index $order, $b;
			die "$a $b" if $ixA + $ixB == -2;
			$ixA - $ixB;
		}
		grep {
			! ref $tree->{$_}
		}
		keys %$tree)
	{
		print " $attr=\"$tree->{$attr}\"";
	}

	my $children = $tree->{Kids} || [];
	if (scalar @$children)
	{
		print ">";
		foreach my $child (@$children)
		{
			printTree($child);
		}
		print "</$tagName";
	}
	else
	{
		print "/"
	}

	print ">";
}

