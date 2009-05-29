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
my $csvfile = shift @ARGV;
my $filelist = shift @ARGV;
my $zipfile = shift @ARGV;

if(! -e $csvfile)
{
  die "cannot find $csvfile\n";
}

  
if(!-e $filelist)
{
  die "Cannot find $filelist\n";
}
        my %ziptimes;
        my %zipused;
        if(defined $zipfile)
          {
          open(ZIP,"7z l $zipfile 2>&1|")  or die "Error: Couldn't look in $zipfile\n";
#          print "time,file\n";
          while( my $line = <ZIP>)
            {
            if($line =~ m/^(\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2})\s\S{5}\s+(\d+)\s+\d+\s+(.+)$/) #ignoring packed size...
              {
              my $name = $3;
              my $size = $2;
              my $time = $1;
              $name =~ s/\\/\//g;
              $name = lc($name);
              $ziptimes{$name} = $time;
#              print "$time,$name\n";
              }
            }
          close ZIP;
          }
        my %files;

        open(FILES,"<$filelist") or die "Couldn't open $filelist\n";
        while(my $line = <FILES>)
        {
            $line =~ s/\\/\//g;

            if($line =~ m/^(.+)\s*:\s(.+\S)\s*$/)
            {
                my $group = $1;
                my $file = $2;

                $file = lc($file);
                if($files{$file})
                {
           
                    $files{$file} = $files{$file}." ".$group; #Stop polluting next column                                        
                       print "Multi:$file".$files{$file}."\n";

                }
                else
                {
                     $files{$file} = ",".$group;
                }
            }                
        }
        close FILE;

        open(CSV,"<$csvfile") or die "Couldn't open $csvfile\n";
        my $resultsfile = $csvfile."_results.csv"; 
        open(RESULTS,">$resultsfile") or die "Couldn't open write to $resultsfile\n";
        my $header = <CSV>;
        $header =~ s/\n//;
        print RESULTS $header.",status,time\n";
        my @fields = split(',',$header);
        my $targetindex = 0;
        my $counter = 0;
        my $bldinfindex = 0;
        my %failed;
        my %bldinffiles;
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
            ++$counter;
        }
        ++$counter;
        my $timeindex = $counter;
#        print "\ntarget:$targetindex\tbuildinf:$bldinfindex\n";
        while(my $line = <CSV>)
            {
            $line =~ s/\n//;
            @fields = split(',',$line);
            my $target = $fields[$targetindex];
            $target = lc($target);
            my $bldinf = $fields[$bldinfindex];
            if(!defined $bldinffiles{$bldinf})
            {
              $bldinffiles{$bldinf} = 1;
            }
            my $found = 0; 
            if(defined $files{$target})
                {
                    $found = 1;                    
                    $line = $line.$files{$target};
                    if($files{$target} =~ m/fail/i)
                    {
                      if(!defined $failed{$bldinf})
                      {
                        $failed{$bldinf} = 1;
                      }
                    }
                }
            if(defined $ziptimes{$target})
              {
                $zipused{$target} = 1;
                if($found)
                {
                  $line = $line.",".$ziptimes{$target};
                }
                else
                {
                  $line = $line.",,".$ziptimes{$target};
                }
              }     
            print RESULTS $line."\n";
            
            }            
        close CSV;
        
        foreach my $target (sort(keys %ziptimes))
        {

          if(!defined $zipused{$target})
          {
            my $time = $ziptimes{$target};
            my $columnCounter=0;
            my @row;
            while($columnCounter <= $counter)
            {
              if($columnCounter == $bldinfindex)
              {
                push(@row,"Orphaned,");
              }
              elsif($columnCounter == $targetindex)
              {
                push(@row,"$target,");
              }
              elsif($columnCounter == $timeindex-1)
              {
                push(@row,"untouched,");
              }
              elsif($columnCounter == $timeindex)
              {
                push(@row,"$time,");
              }              
              else
              {
                push(@row,",");
              }
              ++$columnCounter;
            }
            print RESULTS @row;
            print RESULTS "\n";
          }
        }  
        close RESULTS;

        foreach my $bldinf (sort(keys %bldinffiles))
        {
          if(!defined $failed{$bldinf})
          {
            print "OK:\t$bldinf\n";
          }
        }
        foreach my $bldinf (sort(keys %failed))
        {
            print "Failed:\t$bldinf\n";
        }
        
