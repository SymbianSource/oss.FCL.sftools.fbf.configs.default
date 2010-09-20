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
# Wrapper for dependency analysis tool (http://www.dependencywalker.com), which identifies missing dependencies. This script will
# process a set of given binaries and produce a tab separated output in the following format
#
#   Missing DLL/EXE		(binary with broken dependancy)
#
# Note: 
# 1) Requires minimum of v2.0 of depends.exe to be available in the path.
# 2) Binaries are dealt with sequentially. Any parallelisation should be dealt with by whoever calls this tool.
#
# Usage:
# perl depends.pl <file pattern>
#
# Example:
# perl depends.pl M:\epoc32\release\winscw\udeb\avkon.dll
# perl depends.pl M:\epoc32\release\winscw\udeb\libOpenV* 
# perl depends.pl M:\epoc32\release\winscw\udeb\*
#
use Text::CSV;
my %missing_binaries;
my $debug = 0;

sub walk_binary($)
{
  my $filename = shift;
  my $walk_cmd = "depends.exe /oc:$filename.csv /c $filename";
  my $ret = system("$walk_cmd");
  print "$walk_cmd --DONE\n" if $debug==1;

  # Load CSV
  
  my $csv = Text::CSV->new();
  my @rows;
  
  open my $csvText, "<", $filename.".csv" or die;
  while ( my $row = $csv->getline( $csvText ) ) 
  {
     # column 0 is "?" if unknown dependency
     if ($row->[0] =~ m/^\?$/)
	 {
         # column 1 contains missing dependency	
	 print "MISSING: $filename is missing $row->[1]\n" if $debug==1;
	 $missing_binaries{"$row->[1] \t\t($filename)"} = 1;
	 } 
  }
  $csv->eof or $csv->error_diag();
  # cleanup
  close $csvText;
  system("del $filename.csv");
  
}


my @files = map(glob,@ARGV);
foreach my $file (@files)
{
  print "Walking $file...\n" if $debug==1;
  walk_binary($file);
}
print join("\n", sort keys %missing_binaries, "");
