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
# XML::Simple leaks memory, so there is now a manual parsing mode which is enabled by the '$manualparsing' global variable.
# Writing output to a file is hacked in, so it's not too pretty.

use strict;
use XML::Simple;
use Data::Dumper;

my @header = qw(line layer component name armlicence platform phase code bldinf mmp target source);
my $manualparsing = 1; #XML::Simple leaks memory. Manual parsing doesn't, but may not be as robust if Raptor changes it's output...

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
    my $recipe = undef;
    my %attempts;
    my %licenceattempts;
    my $counter = 0;
    my $licence = 0;
    my $failures = 0;

    while( my $line = <FILE>)
    {
      if(defined $recipe)
      {
        if(defined $recipe->{'content'})
        {
          my $ref = $recipe->{'content'}; 
          push(@$ref, $line);
        }
      }
      ++$counter;
      if($line =~ m/^<recipe\s+(\S.+)>/)
      {
        $recipe = parseline($line."</recipe>");
        $recipe->{'line'} = $counter;
        my @content;
        push(@content, $line);
        $recipe->{'content'} = \@content;
        
#        print Dumper($recipe);
      }
      elsif($line =~ m/<\/recipe>/)
      {
        if(defined $recipe)
        {         
#          if($recipe->{'exit'} !~ m/ok/)
          if($recipe->{'exit'} =~ m/failed/)
          {
	          ++$failures;
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
        my $status = parseline($1);
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
  
  print OUT "Raptor recipe failures: $failures\n";
  
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
  my $content = $recipe->{'content'};
  for my $line (@$content)
  {
    print $line;
  }              
  #print Dumper($recipe);

}

sub parseline($line)
{
  my $line = shift;
#  print "\t$line\n";
  my $val;
  if($manualparsing)
  {
    my $strippedline;    
    if($line =~ m/<\S+(.+)\/>/)
    {
      $strippedline = $1;
    }
    elsif($line =~ m/<\S+(.+)>\s*<\/\S+>/)
    {
      $strippedline = $1;
    }
#    print $strippedline."\n";
    my @stuff = split('\s+', $strippedline);
    my %results;
    foreach my $pair (@stuff)
    {
#      print $pair."\n";
      if($pair =~ m/^(\S+)=\'(\S+)\'$/)
      {
        $results{$1}=$2;
      }
    }
    $val = \%results;
  }  
  else
  {
    $val = XMLin($line);
  }   
  return $val; 
}