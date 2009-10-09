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
# Generate the BRAG-compatible XML summary of the Raptor log from Yarp analysis

use strict;

use Getopt::Long;
use Text::CSV;


if (!@ARGV)
{
	warn "Generate an XML summary of the Raptor build from a Yarp CSV file\n";
	warn "Eg: yarpToBRAG.pl aYarpFile.csv [*_yarp.csv ...]\n";
	exit(1);
}

# Start to build structure to be output as XML (same format as XML::Parser would create for us)
my $xmlNewline = bless { Text => "\n" }, "Characters";
my $buildPhase = bless { name => "Build", Kids => [ $xmlNewline ] }, "phase";
my $buildStatus =
[
	bless
	{
		Kids =>
		[
			$xmlNewline,
			$buildPhase,
			$xmlNewline,
		]
	}, "buildStatus"
];

@ARGV = map { glob $_ } @ARGV;

foreach my $yarpCSV (@ARGV)
{
	# Read Yarp CSV File
	unless (open(CSV, $yarpCSV))
	{
		warn "ERROR: Unable to open $yarpCSV to process. Skipping.\n";
		next;
	}

	my $csv = Text::CSV->new();
	my @keys = ();
	while (my $line = <CSV>)
	{
		chomp $line;
		
		unless ($csv->parse($line))
		{
			my $err = $csv->error_input();
			warn "WARNING: Failed to parse $yarpCSV line $. as CSV '$line': $err  Skipping\n";
			next;
		}

		if (!@keys)
		{
			@keys = $csv->fields();
			next;
		}

		my @values = $csv->fields();
		last unless $values[0];
		unless (scalar @values == scalar @keys)
		{
			warn "WARNING: line does not match expected format at $yarpCSV line $.. Skipping\n";
			next;
		}
		# Populate the hash using a hash slice
		my $failure = {};
		@{$failure}{@keys} = @values;
		$failure->{platform} = lc $failure->{platform};
		
		# Work out the package
		$failure->{package} = $failure->{bldinf};
		$failure->{package} =~ s{[A-Z]:}{}i;
		$failure->{package} =~ s{^(/sf/.*?/.*?)/.*}{$1}i;
		
		# Create a more readable error message
		my %errorIdToDetail = (
			tem => {message => "Failed to process $failure->{source}", severity => "major"},
			msvctoolscompile => {message => "Failed to compile $failure->{source}", severity => "minor"},
			compile => {message => "Failed to compile $failure->{source}", severity => "minor"},
			compile2object => {message => "Failed to compile $failure->{source}", severity => "minor"},
			win32compile2object => {message => "Failed to compile $failure->{source}", severity => "minor"},
			tools2lib => {message => "Failed to build library $failure->{target}", severity => "minor"},
			ar => {message => "Failed to build library $failure->{target}", severity => "minor"},
			win32archive => {message => "Failed to build library $failure->{target}", severity => "minor"},
			"link" => {message => "Failed to create symbols for $failure->{target}", severity => "minor"},
			postlink => {message => "Failed to link $failure->{target}", severity => "minor"},
			win32stageonelink => {message => "Failed to link $failure->{target} (stage 1)", severity => "minor"},
			win32stagetwolink => {message => "Failed to link $failure->{target}", severity => "minor"},
			win32simplelink => {message => "Failed to link $failure->{target}", severity => "minor"},
			win32processexports => {message => "Failed to export $failure->{source} to $failure->{target}", severity => "minor"},
			tracecompile => {message => "Trace compile failure for $failure->{target}", severity => "unknown"},
			extension_makefile => {message => "Failed to process an extension makefile connected to $failure->{bldinf}", severity => "major"},
		);
#		die $failure->{name} unless exists $errorIdToDetail{$failure->{name}};
		my $message = $errorIdToDetail{$failure->{name}}->{message} || "Unknown failure tag '$failure->{name}' ($failure->{source} -> $failure->{target})";
		$failure->{severity} = $errorIdToDetail{$failure->{name}}->{severity} || "unknown";

		# Look through the steps to see if we already have one to match this platform
		my $step;
		foreach (@{$buildPhase->{Kids}})
		{
			next unless ref $_ eq "step";
			if ($_->{name} eq $failure->{platform})
			{
				$step = $_;
				last;
			}
		}
		unless ($step)
		{
			# First item found for this platform - create step entry
			$step = bless { name => $failure->{platform}, Kids => [ $xmlNewline ] }, "step";
			push @{$buildPhase->{Kids}}, $step, $xmlNewline;
			# Also create empty <failures> tags with severities in a sensible order
			foreach my $severity (qw{critical major minor})
			{
				my $failureSet = bless { level => $severity, Kids => [ $xmlNewline ] }, "failures";
				push @{$step->{Kids}}, $failureSet, $xmlNewline;
			}
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
		my $failureItem = bless {
#			href => "",
			"package" => $failure->{package},
			Kids => [ bless { Text => $message }, "Characters" ],
		}, "failure";
		push @{$failureSet->{Kids}}, $failureItem, $xmlNewline;
	}
	close(CSV);
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

