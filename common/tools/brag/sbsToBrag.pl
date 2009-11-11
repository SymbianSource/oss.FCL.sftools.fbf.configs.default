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
use lib "$FindBin::Bin";

use XML::Parser;
#use Getopt::Long;

use ToBrag;

my $raptorLogGlob = shift or die "First argument must be raptor XML log to read\n";
shift and die "Only one argument please\n";

@ARGV = glob $raptorLogGlob;

# Start to build structure to be output as XML (same format as XML::Parser would create for us)
my ($doc, $buildStatus) = ToBrag::createDocumentAndRoot("buildStatus");
# Obtain a phase object
my $buildPhase = ToBrag::ensureChild($buildStatus, "phase", "name", "Build");

# Parse the Raptor logs
# (Use XML::Parser in "Stream" mode so we don't have to hold all the data in memory at the same time)
my $xml = XML::Parser->new(Style => "Stream", Pkg => "main");

foreach my $log (@ARGV)
{
	$xml->parsefile($log);
}

print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
print "<?xml-stylesheet type='text/xsl' href='brag.xsl'?>\n";
ToBrag::printTree($doc->[0]);
print "\n";

exit (0);

my $context;
sub StartTag
{
	my $expat = shift;
	my $tagName = shift;
	my %attrib = %_;

	if ($tagName eq "recipe")
	{
		$context = {%attrib};
	}
	elsif ($tagName eq "status")
	{
		$context->{"exit"} = $attrib{"exit"};
	}
}
sub EndTag
{
	my $expat = shift;
	my $tagName = shift;

	if ($tagName eq "recipe")
	{
		die unless $context;

		if ($context->{"exit"} ne "ok")
		{
			# Create a more readable error message
			my %errorIdToDetail = (
				tem => {message => "Failed to execute '$context->{source}' invoked via $context->{bldinf}", severity => "major"},
				msvctoolscompile => {message => "Failed to compile $context->{source}", severity => "minor"},
				compile => {message => "Failed to compile $context->{source}", severity => "minor"},
				compile2object => {message => "Failed to compile $context->{source}", severity => "minor"},
				win32compile2object => {message => "Failed to compile $context->{source}", severity => "minor"},
				tools2lib => {message => "Failed to build library $context->{target}", severity => "minor"},
				ar => {message => "Failed to build library $context->{target}", severity => "minor"},
				win32archive => {message => "Failed to build library $context->{target}", severity => "minor"},
				"link" => {message => "Failed to create symbols for $context->{target}", severity => "minor"},
				postlink => {message => "Failed to link $context->{target}", severity => "minor"},
				win32stageonelink => {message => "Failed to link $context->{target} (stage 1)", severity => "minor"},
				win32stagetwolink => {message => "Failed to link $context->{target}", severity => "minor"},
				win32simplelink => {message => "Failed to link $context->{target}", severity => "minor"},
				win32processexports => {message => "Failed to export $context->{source} to $context->{target}", severity => "minor"},
				tracecompile => {message => "Trace compile failure for $context->{target}", severity => "unknown"},
				extension_makefile => {message => "Failed within an extension makefile connected to $context->{bldinf}", severity => "major"},
			);
#			die $context->{name} unless exists $errorIdToDetail{$context->{name}};

			my $message = $errorIdToDetail{$context->{name}}->{message} || "Unknown failure tag '$context->{name}' ($context->{source} -> $context->{target})";
			$context->{severity} = $errorIdToDetail{$context->{name}}->{severity} || "unknown";

			# Obtain a step object
			my $step = ToBrag::ensureStep($buildPhase, $context->{config});
			# Also create empty <failures> tags with severities in a sensible order
			ToBrag::ensureFailureSet($step, "critical");
			ToBrag::ensureFailureSet($step, "major");
			ToBrag::ensureFailureSet($step, "minor");
			# Obtain a failures object
			my $failureSet = ToBrag::ensureFailureSet($step, $context->{severity});

			# Now create the failure itself, and add it to this failure set
			my $failureItem = bless {
				Kids => [ bless { Kids => [ bless { Text => $message }, "Characters" ]}, "effect" ],
			}, "failure";
			if ($context->{component})
			{
				$context->{bldinf} =~ s{^\w:(/sf/.*?/.*?)/.*$}{$1};
				$failureItem->{package} = $context->{bldinf};
			}
			my @causes = grep { $_ && ! m/^\+ / } split("\n", $context->{Chars});
			@causes = map { "  $_" } @causes;
			if (@causes)
			{
				my @reportedCauses = @causes[0 .. min($#causes, 49)];
				my $causesItem = bless {
					Kids => [ bless { Text => join "\n", @reportedCauses }, "Characters" ]
				}, "causes";
				push @{$failureItem->{Kids}}, $causesItem;
				my $unreportedCauses = scalar @causes - scalar @reportedCauses;
				$failureItem->{unreported_causes} = $unreportedCauses;
			}
			push @{$failureSet->{Kids}}, $failureItem, $ToBrag::xmlNewline;
		}
		
		$context = undef;
	}
}
sub Text
{
	s/^\n*//;
	if ($context)
	{
		$context->{Chars} .= $_;
	}
}

sub min
{
	return ($_[0] < $_[1]) ? $_[0] : $_[1] ;
}

