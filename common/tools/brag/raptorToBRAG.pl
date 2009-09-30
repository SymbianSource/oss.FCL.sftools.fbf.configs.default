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
# Generate the BRAG-compatible XML summary of the Raptor log from the CSV output of the raptor parser

use strict;

use Getopt::Long;
use Text::CSV;


my $raptorSummary;
my $help = 0;
GetOptions((
	'raptorsummary=s' => \$raptorSummary,
	'help!' => \$help
));

$help = 1 if (!$raptorSummary);
if ($help)
{
	print "Generate an XML summary of the Raptor build from a summary.csv file\n";
	print "Usage: perl summarize.pl --raptorsummary=CSV\n";
	exit(0);
}

# Start to build structure to be output as XML (same format as XML::Parser would create for us)
my $xmlNewline = bless { Text => "\n" }, "Characters";
my $data = [bless {name => "build", Kids => [ $xmlNewline ] }, "stage"];
# Get a shortcut reference to the bit we will use a lot
my $buildStage = $data->[0];

# READ SUMMARY.CSV FILE
open(CSV, $raptorSummary);
my $csv = Text::CSV->new();
while (my $line = <CSV>)
{
	chomp $line;
	
	unless ($csv->parse($line))
	{
		my $err = $csv->error_input();
		warn "Failed to parse $raptorSummary line line $. as CSV '$line': $err  Skipping\n";
		next;
	}
	
	my @keys = qw{category subcategory severity config component phase recipe file linenum};
	my @values = $csv->fields();
	unless (scalar @keys == scalar @keys)
	{
		warn "WARNING: line does not match expected format at $raptorSummary line $.. Skipping\n";
		next;
	}
	
	my $failure = {};
	@{$failure}{@keys} = @values;
	
	if (!$failure->{category})
	{
		warn "WARNING: summary line without a category at $raptorSummary line $.. Skipping\n";
		next;
	}
	
	if ($failure->{category} =~ m{^recipe_failure$}i and !$failure->{component})
	{
		warn "WARNING: recipe_failure with component field empty at $raptorSummary line $.. Skipping\n";
		next;
	}
	$failure->{subcategory} ||= 'uncategorized';
	$failure->{severity} ||= 'unknown';
	
	# Look through the steps to see if we already have one to match this config
	my $step;
	foreach (@{$buildStage->{Kids}})
	{
		next unless ref $_ eq "step";
		if ($_->{name} eq $failure->{config})
		{
			$step = $_;
			last;
		}
	}
	unless ($step)
	{
		# First item found in this step - create step entry
		$step = bless { name => $failure->{config}, Kids => [ $xmlNewline ] }, "step";
		push @{$buildStage->{Kids}}, $step, $xmlNewline;
	}
	
	# Look through the sets of failures in this step to see if we hve one which matches this severity
	my $failureSet;
	foreach (@{$step->{Kids}})
	{
		next unless ref $_ eq "failures";
		if ($_->{level} eq $failure->{severity})
		{
			$failureSet = $_;
			last;
		}
	}
	unless ($failureSet)
	{
		# First item found at this severity - create failures entry
		$failureSet = bless { level => $failure->{severity}, Kids => [ $xmlNewline ] }, "failures";
		push @{$step->{Kids}}, $failureSet, $xmlNewline;
	}

	# Now create the failure itself, and add it to this failure set
	my $failureItem = bless { href => "", Kids => [ bless { Text => $failure->{subcategory} }, "Characters" ] }, "failure";
	push @{$failureSet->{Kids}}, $failureItem, $xmlNewline;
}
close(CSV);

# Print XML
print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
print "<?xml-stylesheet type='text/xsl' href='brag.xsl'?>\n";
printTree($data->[0]);
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

