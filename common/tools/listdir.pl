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
# Recursive listing of a directory, outputting lower-cased relative paths with unix dir separators

use strict;

my $dir      = shift or die "Usage: $0 <dir> \n";   #  provided dir to traverse
my $filelist = [];
my $init = $dir = lc($dir);
$init =~ s{\\}{\\\\};

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
      if(-d "$dir/$file") {
        #  traverse subdirs
        recursedir("$dir/$file", $list);
      }
      elsif(-f "$dir/$file") {
        my $formatted = lc($dir)."/".lc($file);
        $formatted =~ s!$init/!!;
        push @$list, $formatted;
      }
    }
    closedir DIR;
  }
  else {
    warn "Cannot open the directory '$dir' $!\n";
  }
}
