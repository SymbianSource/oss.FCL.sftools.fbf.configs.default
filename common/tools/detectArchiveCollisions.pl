#!perl -w
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
# Identify collisions between file archives
#
# Usage:
# detectArchiveCollisions.pl this.zip that.zip several_*.zip and\also_*.7z

use strict;

# Expand shell wildcards
@ARGV = map { glob $_ } @ARGV;

my $data;

foreach my $archive (@ARGV)
{
	print "Reading $archive...\n";
	my $file = undef;
	foreach my $line (`7z l -slt $archive`)
	{
		if ($line =~ m{^Path = (.*)})
		{
			$file = $1;
			next;
		}

		if ($line =~ m{^Folder = -})
		{
			# Record this (non-directory) item
			push @{$data->{$file}}, $archive;
			next;
		}
	}
}

foreach my $file (sort keys %$data)
{
	next unless scalar @{$data->{$file}} > 1;
	print "ERROR: $file\n";
	foreach (@{$data->{$file}})
	{
		print "\t$_\n";
	}
}

