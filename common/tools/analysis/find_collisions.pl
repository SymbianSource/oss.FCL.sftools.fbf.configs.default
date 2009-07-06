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
#
# Description:
# Adds info form a file to a CSV

use strict;

main();

sub main()
{
  my $csvfile = shift @ARGV;

  if(! -e $csvfile)
  {
    die "cannot find $csvfile\n";
  }

  open(CSV,"<$csvfile") or die "Couldn't open $csvfile\n";
  my $header = <CSV>;
  $header =~ s/\n//;
  print RESULTS $header.",status\n";
  my @fields = split(',',$header);
  my $targetindex = 0;
  my $counter = 0;
  my $bldinfindex = 0;
  my $makefileindex = 0;
  my $typeindex = 0;
  my $extindex = 0;
  my %failed;
  my %bldinffiles;
  
  my %targets;
  
  foreach my $column (@fields)
  {
      if($column =~ m/target/)
      {
          $targetindex = $counter;
      }
      elsif($column =~ m/bldinf/)
      {
        $bldinfindex = $counter;
      }
      elsif($column =~ m/makefile/)
      {
        $makefileindex = $counter;
      }
      elsif($column =~ m/type/)
      {
        $typeindex = $counter;
      }
      elsif($column =~ m/extension/)
      {
        $extindex = $counter;
      }
      ++$counter;
  }
#        print "\ntarget:$targetindex\tbuildinf:$bldinfindex\n";
    #header
  my $resultsfile = $csvfile."_collisions.csv"; 
  open(RESULTS, ">$resultsfile") or die "Coudn't open $resultsfile";
  
  print RESULTS "Collision,target,extension,type,source1,source2\n";
  while(my $line = <CSV>)
      {
      $line =~ s/\n//;
      @fields = split(',',$line);
      my $target = $fields[$targetindex];
      $target = lc($target);
      my $makefile = $fields[$makefileindex];
      my $bldinf =   $fields[$bldinfindex];
                
      if(defined $targets{$target})
        {
          my $currentmakefile = $targets{$target};
          if($makefile eq "")
          {
            $makefile = $bldinf;
          }
          if (!($currentmakefile eq $makefile))
            {
            my $type = $fields[$typeindex]; #DODGY - smoe custom makefiles also clash with export headers...
            my $ext = $fields[$extindex];
            my $collision = "-";
            if($type eq "export")
            {
              $collision = diffcollision($target,$currentmakefile,$makefile);
            }   
            print RESULTS "$collision,$target,$ext,$type,$currentmakefile,$makefile\n";
            }
        }
      else
        {
          if($makefile eq "")
          {
             $targets{$target} = $bldinf;
          }
          else
          {
            $targets{$target} = $makefile;
          }  
        }      
      }
  close RESULTS;    
  close CSV;
}

sub diffcollision($$$)
{
  my $target = shift;
  my $left = shift;
  my $right = shift;
  
  $target =~ s/\//\\/g;
  $left  =~ s/\//\\/g;
  $right =~ s/\//\\/g;  
  my $ret = "unknown";
  if(!-e $target)
  {
    $ret = "missing";
  }
  else
  {
    if(-e $left && -e $right)
    {
      my $leftdiff = 1;
      my $rightdiff = 1;
      open(DIFF,"fc $left $target|") or die "couldn't execute fc";
      print "fc $left $target\n";
      
      while(my $line = <DIFF>)
      {
        
        if($line =~ m/FC:\sno\sdifferences\sencountered/i)
        {
          $leftdiff = 0;
        }
        print "\t$line";
      }
      close DIFF;

      open(DIFF,"fc $right $target|") or die "couldn't execute fc";
      print "fc $right $target\n";
      while( my $line = <DIFF>)
      {
        if($line =~ m/FC:\sno\sdifferences\sencountered/i)
        {
          $rightdiff = 0;
        }
        print "\t$line";
      }
      close DIFF;

     if($leftdiff && !$rightdiff )
        {$ret = "match right";}
     elsif($rightdiff && !$leftdiff)
        {$ret = "match left";}
     elsif($rightdiff && $leftdiff)
        {$ret = "match neither";}
     elsif(!$rightdiff && !$leftdiff)
        {$ret = "match both";}                  
    }
  
    
  }
  return $ret; 
}