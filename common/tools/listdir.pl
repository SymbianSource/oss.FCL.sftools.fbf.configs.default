#!perl -w
use strict;

my $dir      = shift or die "Usage: $0 <dir> \n";   #  provided dir to traverse
my $filelist = [];

# fwd declaration to prevent warning
sub recursedir($$);

# run recurse and print
recursedir ($dir, $filelist);
print $_, "\n" for(@$filelist);

sub recursedir($$) {

  my $dir  = shift @_;
  my $list = shift @_;

  if(opendir(DIR, "$dir")) {
    #  list dir
    for my $file(grep { !/^\./ } readdir DIR) {
      if(-d "$dir\\$file") {
        #  traverse subdirs
        recursedir("$dir\\$file", $list);
      }
      elsif(-f "$dir\\$file") {
        #  if file then swap (any present) fwd to bkslash and add to list        
        $dir   =~s/\//\\/;
        $file  =~s/\//\\/;
        push @$list, "$dir\\$file";
      }
    }
    closedir DIR;
  }
  else {
    warn "Cannot open the directory '$dir' $!\n";
  }
}
