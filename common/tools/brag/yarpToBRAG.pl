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

use FindBin;
use lib "$FindBin::Bin/../lib";
use lib "$FindBin::Bin";

use Getopt::Long;
use Text::CSV;

use ToBrag;

if (!@ARGV)
{
	warn "Generate an XML summary of the Raptor build from a Yarp CSV file\n";
	warn "Eg: yarpToBRAG.pl aYarpFile.csv [*_yarp.csv ...]\n";
	exit(1);
}

# Start to build structure to be output as XML (same format as XML::Parser would create for us)
my $buildStatus = ToBrag::createBuildStatus();
# Obtain a phase object
my $buildPhase = ToBrag::ensurePhase($buildStatus, "Build");

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
		
		# Obtain a matching step
		my $step = ToBrag::ensureStep($buildPhase, $failure->{platform});
		# Also create empty <failures> tags with severities in a sensible order
		ToBrag::ensureFailureSet($step, "critical");
		ToBrag::ensureFailureSet($step, "major");
		ToBrag::ensureFailureSet($step, "minor");
		
		# Obtain a set of failures which matches this severity
		my $failureSet = ToBrag::ensureFailureSet($step, $failure->{severity});
		
		# Now create the failure itself, and add it to this failure set
		my $failureItem = bless {
#			href => "",
			"package" => $failure->{package},
			Kids => [ bless { Text => $message }, "Characters" ],
		}, "failure";
		push @{$failureSet->{Kids}}, $failureItem, $ToBrag::xmlNewline;
	}
	close(CSV);
}
# Print XML
print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
print "<?xml-stylesheet type='text/xsl' href='brag.xsl'?>\n";
ToBrag::printTree($buildStatus->[0]);
print "\n";

exit(0);

