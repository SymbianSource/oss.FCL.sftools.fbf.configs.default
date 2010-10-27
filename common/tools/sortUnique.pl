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
# Sorts the input, removes duplicates, and outputs it

use strict;

# Read it
my @content = <>;

# Sort it, and grep to remove duplicates
my $previous = "\n\n";
@content = grep {$_ ne $previous && ($previous = $_, 1) } sort @content;

# Write it
print @content;
