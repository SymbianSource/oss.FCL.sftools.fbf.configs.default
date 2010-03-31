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
use Win32::OLE;

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

# Connect to WMI services on this machine (".")
my $wmiServices = Win32::OLE->GetObject( "winmgmts:{impersonationLevel=impersonate,(security)}//." ) or die;
# Get list of all volumes (drive letters)
my @volumes = Win32::OLE::in($wmiServices->InstancesOf( "Win32_LogicalDisk" ));
# Get list of substed drives
my %subst = map { (substr $_, 0, 2) => 1 } `subst`;
# Filter volumes to remove non-Partitions, and substed drives
@volumes = grep { $_->{DriveType} == 3 && !exists $subst{$_->{DeviceID}} } @volumes;
# Also remove the system drive (usually C:) unless it's the only drive in the box!
@volumes = grep { $_->{DeviceID} ne $ENV{SystemDrive} } @volumes if scalar(@volumes) > 1;

die "Unable to find any suitable drives at all\n" unless @volumes;

if ($option->{all})
{
	print join ",", map { $_->{DeviceID} } @volumes;
	print "\n";
}
elsif ($option->{capacity})
{
	# Sort by capacity to find the largest volume and print out the corresponding letter
	@volumes = reverse sort { $a->{Size} <=> $b->{Size} } @volumes;
	print "$volumes[0]->{DeviceID}\n";
}
elsif ($option->{space})
{
	# Sort by space to find the volume with the largest amount of space and print out the corresponding letter
	@volumes = reverse sort { $a->{FreeSpace} <=> $b->{FreeSpace} } @volumes;
	print "$volumes[0]->{DeviceID}\n";
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

