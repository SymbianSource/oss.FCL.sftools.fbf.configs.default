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


if(! -e $csvfile)
{
  die "cannot find $csvfile\n";
}

  
if(!-e $filelist)
{
  die "Cannot find $filelist\n";
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
           
                    $files{$file} = $files{$file}.",".$group;                                        
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
        print RESULTS $header.",status\n";
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
 
            if(defined $files{$target})
                {                    
                    $line = $line.$files{$target};
                    if($files{$target} =~ m/fail/i)
                    {
                      if(!defined $failed{$bldinf})
                      {
                        $failed{$bldinf} = 1;
                      }
                    }
                }
            print RESULTS $line."\n";
            
            }            
        close RESULTS;
        close CSV;
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
        
