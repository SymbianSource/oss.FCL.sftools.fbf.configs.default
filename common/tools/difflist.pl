#! perl -w
use strict;

my $element;
my @union = ();
my @intersection = ();
my @difference = ();
my %count = ();

my $file1 = shift or die "Usage: $0 file1 file2\n";
my $file2 = shift or die "Usage: $0 file1 file2\n";

open FILE1, "<$file1" or die "ERROR: Can't read $file1";
open FILE2, "<$file2" or die "ERROR: Can't read $file2";

my @file1_content = <FILE1>;
my @file2_content = <FILE2>;

close FILE1;
close FILE2;

print "* Comparing $file1 and $file2\n";
foreach $element (@file1_content, @file2_content) { $count{$element}++ }
foreach $element (keys %count) {
    push @union, $element;
    push @{ $count{$element} > 1 ? \@intersection : \@difference }, $element;
}

if (@difference > 0) {
    foreach (@difference){
        print $_;
    }
} else {
    print "* Files are identical\n";
}
