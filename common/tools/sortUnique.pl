#!perl -w
use strict;
# Sorts the input, removes duplicates, and outputs it

# Read it
my @content = <>;

# Sort it, and grep to remove duplicates
my $previous = "\n\n";
@content = grep {$_ ne $previous && ($previous = $_, 1) } sort @content;

# Write it
print @content;
