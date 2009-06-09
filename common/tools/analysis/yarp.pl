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
# YARP - Yet Another Recipe Parser
# This tool parses Raptor logs looking for failures, and writes a CSV file of the results
#
# Usage:
# perl yarp.pl <logfile> <csvfile>
#
# Notes:
# Currently it won't tell you any info about why it fails, with the exception of arm licence issues.
# It also uses a lot of memory, so while there is a subroutine for doing multiple files, it's not used, and is out of date.
# Writing output to a file is hacked in, so it's not too pretty.

use strict;
use XML::Simple;
use Data::Dumper;

my @header = qw(line layer component name armlicence platform phase code bldinf mmp target source);

main();


sub main()
{
  my $filename = shift @ARGV;
  my $output = shift @ARGV;
  open(OUT,">$output") or die "Coudn't open $output\n";
  foreach my $key (@header)
  {
    print OUT $key.",";
  } 
  print OUT "\n";

  parsefile($filename);
  close OUT;
}
sub scandir()
{
  my $path = shift @ARGV;
  my @files = glob($path."/*compile.log");
   
  foreach my $filename (@files)
  {
#    print $filename."\n";
    parsefile($filename);
  }  
}
  
sub  parsefile($filename)
{
    my $filename = shift;
#    print "Scanning $filename\n";
    open(FILE,"<$filename") or die "Couldn't open filename\n";
    my $recipe;
    my %attempts;
    my %licenceattempts;
    my $counter = 0;
    my $licence = 0;
    while( my $line = <FILE>)
    {
      ++$counter;
      if($line =~ m/^<recipe\s+(\S.+)>/)
      {
        $recipe = XMLin($line."</recipe>");
        $recipe->{'line'} = $counter;
#        print Dumper($recipe);
      }
      elsif($line =~ m/<\/recipe>/)
      {
        if(defined $recipe)
        {         
#          if($recipe->{'exit'} !~ m/ok/)
          if($recipe->{'exit'} =~ m/failed/)
          {
#            if($recipe->{'target'} =~ m/\S:epoc32\//i) 
#               && $recipe->{'target'} !~ m/\S:epoc32\/build/i)
            {
              DumpRecipe($recipe);

            }
          }         
        $recipe = undef;
        }
      }
      elsif($line =~ m/Error:\sC3397E:\s/) #ARM Licence error code...
      {
        ++$licence;
        if(defined $recipe)
        {
          $recipe->{'armlicence'} = 1;
        }  
      }
      elsif($line =~ m/(<status\s.+\/>)/)
      {
        my $status = XMLin($1);
        if(defined $recipe)
        {
          $recipe->{'exit'} = $status->{'exit'};
          $recipe->{'attempt'} = $status->{'attempt'};
          if(defined $status->{'code'})
          {
            $recipe->{'code'} = $status->{'code'}; 
          }
          if(!defined $attempts{$status->{'attempt'}})
            {
              $attempts{$status->{'attempt'}} = 0;
            }
          $attempts{$status->{'attempt'}} = $attempts{$status->{'attempt'}} + 1;
          if(defined $recipe->{'armlicence'})
          {
            if(!defined $licenceattempts{$status->{'attempt'}})
              {
                $licenceattempts{$status->{'attempt'}} = 0;
              }
            $licenceattempts{$status->{'attempt'}} = $licenceattempts{$status->{'attempt'}} + 1;
          }               
        }      
      }
    }
  close FILE;
  print OUT "\n\nSummaries\n\n";
  foreach my $attempt (sort keys %attempts)
  {
    print OUT "Overall attempts: $attempt,".$attempts{$attempt}.",\n";
  }
  foreach my $attempt (sort keys %licenceattempts)
  {
    print OUT "ARM Licence Fail attempts: $attempt,".$licenceattempts{$attempt}.",\n";
  }
  print OUT "Total ARM Licence failures,$licence\n";
  
   
}

sub DumpRecipe($)
{
  my $recipe = shift;
  foreach my $key (@header)
  {
    if(defined $recipe->{$key})
    {
      print OUT $recipe->{$key};
    }
    print OUT ",";
  }
  print OUT "\n";            
  #print Dumper($recipe);

}
