#!/usr/bin/perl
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
# Matt Davies <mattd@symbian.org>
#
# Description:
# dumprecipe.pl : Gives you the recipe starting with the given line number
# perl dumprecipe.pl <log> <line>
#
# Tool is generally for use with yarp, to look at individual errors, and was written as the logs are too verbose for many editors to handle.
# As usual, this tool is rough and ready.
 

use strict;

my $file = shift @ARGV;
my $number = shift @ARGV;
my %numbers;
if(!-e $number && $number =~ m/^\d+$/)
{
  $numbers{$number} = $number;
}
else
{
  open(NUMBERS, "<$number") or die "Couldn't open $number";
  while(my $line = <NUMBERS>)
  {
    if($line =~ m/(\d+)\s+(\S+)/)
    {
      $numbers{$1} = "$2($1)";
    }
    elsif($line =~ m/(\d+)/)
    {
      $numbers{$1} = $1;
    }
  }
  close NUMBERS;
}  


open(FILE,"<$file") or die "Coudln't open file $file\n";
my $linecount = 0;
my $recipe = 0;
while(my $line = <FILE>)
{
  ++$linecount;

  if(!$recipe && defined $numbers{$linecount})
  {
    $recipe = $numbers{$linecount};
  }
  if($recipe)
  {
    print $recipe."\t".$line;
  }
  if($line =~ m/<\/recipe>/)
  {
    $recipe = 0;
  }
}
#print $linecount;