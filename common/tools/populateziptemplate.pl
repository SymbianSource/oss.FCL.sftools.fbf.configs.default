#!perl -w
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
# Populates the template for packaging src and binaries in the build

use strict;

use FindBin;
use lib "$FindBin::Bin/lib";
use Text::CSV;
require XML::Simple;

# Raw inputs come in as parameters to the script
# TODO: Use a proper option parsing module
my $sourcesCSV = shift or die "First arg must be source csv file";
my $template = shift or die "Second arg must be template file";
my $ftl = shift or die "Third arg must be output file";
my $rndExcludes = shift or die "Fourth arg must be rnd-excludes file";

# Load CSV
open my $csvText, "<", $sourcesCSV or die "Unable to open sources.csv from $sourcesCSV";
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
my $zipConfig = $xml->XMLin($template, keyattr => $keyAttr);
my @allRndFiles;
my $miscCount = 0;

# For each package in CSV...
foreach my $package (@packages)
{
	warn "Warning: Package $package->{dst} does not appear on the local system\n" unless -d $package->{dst};
	$package->{dst} =~ s{^/}{}g;
	if ($package->{source} =~ m{/(sfl|oss)/(MCL|FCL)/(sf|utilities|interim)/(([^/]+)/)?([^/]+)?})
	{
		my ($license, $codeline, $thingy, $layer, $packageName) = ($1, $2, $3, $5, $6);
		# $thingy is the part of the path after the codeline. For
		# platform packages, it's "sf". For the utilities package, it's
		# "utilities" (the name of the package) and there's no more
		# path.
		#
		# I can't think of anything to describe this item, hence $thingy
		if ($thingy eq "utilities")
		{
			$layer = "tools";
			$packageName = "utilities";
		}
		elsif ($thingy eq "interim")
		{
			if ($layer eq "QEMU")
			{
				$packageName = $layer;
				$layer = "adaptation";
			}
			else
			{
				warn "ERROR: Don't know how to deal with interim package $layer\n";
				goto MISC_PACKAGE;
			}
		}
		elsif (!defined $packageName)
		{
			goto MISC_PACKAGE;
		}
		
  		push @{$zipConfig->{config}->{config}->{src}->{config}->{$license}->{config}},
  		{
  			set =>
  			[
  				{
  					name => "name",
  					value=> join "_", "src", $license, $layer, $packageName,
  				},
  				{
  					name => "include",
  					value => "$package->{dst}/**",
  				},
  			]
  		};
		$zipConfig->{config}->{config}->{"src-by-layer"}->{config}->{$license."-$layer"} =
		{
			set => 
			[
				{
					name => "name",
					value => join "_", "src", $license, $layer,
				},
				{
					name => "include",
					value => join "_", "src", $license, $layer, "*.zip",
				},
			]
		};
	}
	elsif ($package->{source} =~ m{/rnd/([^/]+)/([^/]+)})
	{
		# RnD repository
		my $licenseType = $1;
		my $binName="bin_$1_$2";
		my $srcName="src_$1_$2";
		my $postbuildName = "binaries_$2";
		my $config = "rnd";
		if ($licenseType eq "internal")
		{
			$binName = "binaries_$2_prebuild";
			$config = "rnd-internal";
		}
		
		# Create a zip object
		push @{$zipConfig->{config}->{config}->{$config}->{config}},
		{
			set =>
			[
				{
					name => "root.dir",
					value=> "\${build.drive}/$package->{dst}",
				},
				{
					name => "name",
					value=> "$binName",
				},
				{
					name => "include",
					value=> "epoc32/**",
				},
				{
					name => "include",
					value=> "*.txt",
				},
			]
		};
		# Enumerate all the files on the local disk that are in this repository
		(my $dosCompatibleDst = $package->{dst}) =~ s{/}{\\}g;
		my @files = `dir /b/s/a-d $dosCompatibleDst 2> nul:`;
		next unless @files;
		# Add the files to the global list of items to be excluded in the binary zips
		foreach (@files)
		{
			chomp;
			s{\\}{/}g;
			s!^[A-Z]:/$package->{dst}/!!i;
		}
		my @srcFiles = grep { !m{^epoc32/} and !m{^[^/]+\.txt$}i and !m{^\.hg} } @files;
		if (@srcFiles)
		{
			# Create a zip object for the source
			push @{$zipConfig->{config}->{config}->{$config}->{config}},
			{
				set =>
				[
					{
						name => "root.dir",
						value=> "\${build.drive}/$package->{dst}",
					},
					{
						name => "name",
						value=> "$srcName",
					},
					{
						name => "include",
						value=> "**",
					},
					{
						name => "exclude",
						value=> "epoc32/**",
					},
					{
						name => "exclude",
						value=> ".hg/**",
					},
					{
						name => "exclude",
						value=> ".hg_archival.txt",
					},
				]
			};
		}
		@files = grep {
			m{^epoc32/}i;
		} @files;
		push @allRndFiles, @files;

		if ($licenseType eq "internal")
		{
			# Add a zip object to zip this package from the epoc tree in the postbuild phase
			push @{$zipConfig->{config}->{config}->{"rnd-postbuild"}->{config}},
			{
				name => $postbuildName,
				set =>
				[
					{
						name => "name",
						value=> $postbuildName,
					},
					# Turn the array of files into an array of inclusion hashes
					(map { {name => "include", value => $_ } } @files),
				]
			};
		}
	}
	else
	{
MISC_PACKAGE:
		(my $dest2 = $package->{dst}) =~ s{[\\/]}{_slash_}g;
		push @{$zipConfig->{config}->{config}->{src}->{config}->{misc}->{config}},
		{
			set =>
			[
				{
					name => "name",
					value=> "src_misc_$dest2"."_$miscCount",
				},
				{
					name => "include",
					value => "$package->{dst}/**",
				},
			]
		};
		$miscCount++;
		warn "Warning: Cannot determine license for '$package->{source}' - it will be packaged as 'src_misc_$dest2"."_$miscCount'\n";
	}
}

# Turn the RnD source inclusion lists into a binary exclusion list
my @excludes = map { {name => "exclude", value => "$_"} } @allRndFiles;
push @{$zipConfig->{config}->{config}->{bin}->{config}->{set}}, @excludes;

$xml->XMLout($zipConfig, OutputFile => $ftl, XMLDecl => 1, RootName => 'build', keyattr => $keyAttr);

# Output all rnd files into exclude list for later
open my $fh, ">", $rndExcludes or die "Cannot write exlude file!";
foreach (@allRndFiles)
{
	print $fh "$_\n";
}
close $fh;

