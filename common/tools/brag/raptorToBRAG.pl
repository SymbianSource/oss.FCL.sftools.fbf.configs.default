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

use FindBin;
use lib "$FindBin::Bin/../lib";
use lib "$FindBin::Bin";

use Getopt::Long;
use Text::CSV;

use ToBrag;

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
my $buildStatus = ToBrag::createBuildStatus();
# Obtain a phase object
my $buildPhase = ToBrag::ensurePhase($buildStatus, "Build");

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
	unless (scalar @values == scalar @keys)
	{
		warn "WARNING: line does not match expected format at $raptorSummary line $.. Skipping\n";
		next;
	}
	
	# Populate the hash using a hash slice
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
	
	# Obtain a matching step
	my $step = ToBrag::ensureStep($buildPhase, $failure->{config});
	# Also create empty <failures> tags with severities in a sensible order
	ToBrag::ensureFailureSet($step, "critical");
	ToBrag::ensureFailureSet($step, "major");
	ToBrag::ensureFailureSet($step, "minor");
	
	# Obtain a set of failures which matches this severity
	my $failureSet = ToBrag::ensureFailureSet($step, $failure->{severity});

	# Now create the failure itself, and add it to this failure set
	my $failureItem = bless {
		Kids => [ bless { Kids => [ bless { Text => $failure->{subcategory} }, "Characters" ]}, "effect" ],
	}, "failure";
	if ($failure->{component})
	{
		$failure->{component} =~ s{^(/sf/.*?/.*?)/.*$}{$1};
		$failureItem->{package} = $failure->{component};
	}
	push @{$failureSet->{Kids}}, $failureItem, $ToBrag::xmlNewline;
}
close(CSV);

# Print XML
print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
print "<?xml-stylesheet type='text/xsl' href='brag.xsl'?>\n";
ToBrag::printTree($buildStatus->[0]);
print "\n";

exit(0);

