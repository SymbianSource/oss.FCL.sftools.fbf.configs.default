use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/lib";
use Text::CSV;
require XML::Simple;

# Raw inputs come in as parameters to the script
# TODO: Use a proper option parsing module
my $sourcesCSV = shift or die "First arg must be source csv file";
my $template = shift or die "Second arg must be template file";
my $ftl = shift or die "Third arg must be output file";
shift and die "No more than three arguments please";

# Load CSV
open my $csvText, "<", $sourcesCSV or die;
my $csv = Text::CSV->new();
my @keys;
my @packages;
while (my $line = <$csvText>)
{
	chomp $line;
	next unless $line;
	unless ($csv->parse($line))
	{
		my $err = $csv->error_input();
		die "Failed to parse line '$line': $err";
	}

	if (! @keys)
	{
		# First line - note the column names
		@keys =  $csv->fields();
	}
	else
	{
		# Already got the keys, so get the data
		my %package;
		# Read into a hash slice
		@package{@keys} = $csv->fields();
		push @packages, \%package;
	}
}
close $csvText;

# This controls how the XML parsing decides what should be tags and what should be attributes
# It's been worked out mostly by trial and error :-(
my $keyAttr = { config => "name", name => "set"};
# Load template
my $xml = XML::Simple->new();
my $zipConfig = $xml->XMLin($template, KeyAttr => $keyAttr);
my @allRndFiles;

# For each package in CSV...
foreach my $package (@packages)
{
	warn "Warning: Package $package->{dst} does not appear on the local system\n" unless -d $package->{dst};
	$package->{dst} =~ s{^/}{}g;
	if ($package->{source} =~ m{/(sfl|epl)/sf/([^/]+)/([^/]+)})
	{	    
		push @{$zipConfig->{config}->{config}->{src}->{config}->{$1}->{config}},
		{
			set =>
			[
				{
					name => "name",
					value=> "src_$1_$2_$3",
				},
				{
					name => "include",
					value => "$package->{dst}/**",
				},
			]
		};
	}
	elsif ($package->{source} =~ m{/rnd/([^/]+)/([^/]+)})
	{
		# RnD repository
		my $name = "bin_rnd_$1_$2";
		# Create a zip object
		push @{$zipConfig->{config}->{config}->{src}->{config}->{rnd}->{config}},
		{
			set =>
			[
				{
					name => "root.dir",
					value=> "\${build.drive}/$package->{dst}",
				},
				{
					name => "name",
					value=> "$name",
				},
				{
					name => "include",
					value=> "/**",
				},
			]
		};
		# Enumerate all the files on the local disk that are in this repository
		(my $dosCompatibleDst = $package->{dst}) =~ s{/}{\\}g;
		my @files = `dir /b/s/a-d $dosCompatibleDst 2> nul:`;
		#print "@files\n";
		next unless @files;
		# Add the files to this zip object
		@files = grep {
			chomp;
			s{\\}{/}g;
			s!^[A-Z]:/$package->{dst}/!!i;
			m{^epoc32/}i;
		} @files;
		push @allRndFiles, @files;
	}
	else
	{
		die "Cannot determine license for '$package->{source}'";
	}
}

# Turn the RnD source inclusion lists into a binary exclusion list
my @excludes = map { {name => "exclude", value => "$_"} } @allRndFiles;
push @{$zipConfig->{config}->{config}->{bin}->{config}->{set}}, @excludes;

$xml->XMLout($zipConfig, OutputFile => $ftl, XMLDecl => 1, RootName => 'build', KeyAttr => $keyAttr);

