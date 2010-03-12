#!perl -w
#
# Copyright (c) 2010 Symbian Foundation Ltd
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
# Search physical drives to find either:
#  * The one with the largest amount of free space
#  * The one with the greatest capacity
#  * The list of all such drives

use strict;

use Getopt::Long;

# Read option arguments
my $option;
my $ok = GetOptions(
	'capacity' => \$option->{capacity},
	'space' => \$option->{space},
	'all' => \$option->{all},
	'help|?' => \$option->{help},
);

if (defined $option->{help})
{
	usage();
	exit;
}

if (!$ok || @ARGV || 1 != scalar grep { defined $option->{$_} } keys %$option)
{
	warn "Exactly one option must be supplied to indicate the required output\n$ok\n@ARGV\n";
	usage();
	exit(1);
}

# Use Windows command to list physical volumes on the machine
# (No substed drives, or mapped network drives)
my @details = map {chomp;$_} `echo list volume | diskpart`;

my @drives;
my %space;
my %capacity;
for my $driveLine (@details)
{
	# If this line of output is actually about a healthy HD volume...
	if ($driveLine =~ m{^\s+Volume \d+\s+([A-Z]).*?(Partition|RAID-5)\s+(\d+) ([A-Z]+)\s+Healthy} )
	{
		my ($letter, $capacityValue, $capacityUnit) = ($1, $3, $4);
		
		my %multiplier = (
			MB => 1000000,
			GB => 1000000000,
			TB => 1000000000000,
		);

		if (not exists $multiplier{$capacityUnit})
		{
			warn "Don't know how to interpret $capacityValue $capacityUnit\n";
			next;
		}
		$capacityValue *= $multiplier{$capacityUnit};

		# Ignore the system drive
		next if ($driveLine =~ m{System\s*$});

		# Use dir to get the freespace (bytes)
		my @bytesFree = grep { s{^.*?(\d+) bytes free\s*$}{$1} } map {chomp;$_} `cmd /c dir /-C /A $letter:\\`;
		# Take the value from the bottom of the report
		my $bytesFree = $bytesFree[-1];

		# Record info for this volume
		push @drives, $letter;
		$space{$bytesFree} = $letter;
		$capacity{$capacityValue} = $letter;
	}
}

die "Unable to find any suitable drives at all\n" unless %space;

if ($option->{all})
{
	print join ",", map { "$_:" } @drives;
	print "\n";
	exit;
}
elsif ($option->{capacity})
{
	# Sort by capacity to find the largest volume and print out the corresponding letter
	print "$capacity{(reverse sort keys %capacity)[0]}:\n";
}
elsif ($option->{space})
{
	# Sort by space to find the volume with the largest amount of space and print out the corresponding letter
	print "$space{(reverse sort keys %space)[0]}:\n";
}

exit;

sub usage
{
	$0 =~ m{[\\/]([^\\/]*)$};
	print <<EOT;

Usage: $1 -all | -capacity | -space | -help

  -all          Outputs all physical drives in the system (separated by ',').
  -capacity     Outputs physical drive of greatest capacity in the system.
  -space        Outputs physical drive with greatest free space in the system.
  -help         Outputs this help message.

EOT
}

