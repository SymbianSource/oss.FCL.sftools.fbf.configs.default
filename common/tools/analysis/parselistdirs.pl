#!/usr/bin/perl
use strict;

#listdir_platform_MCL.PDK-3.5_baseline.log
#listdir_platform_MCL.PDK-3.5_post-clean.log
#listdir_platform_MCL.PDK-3.5_post-clean_delta.log
#listdir_platform_MCL.PDK-3.5_post-build-tools2.log
#listdir_platform_MCL.PDK-3.5_post-build-tools.log
#listdir_platform_MCL.PDK-3.5_post-build-main.log
#listdir_platform_MCL.PDK-3.5_s60-baseline.log
#listdir_platform_MCL.PDK-3.5_post-s60-clean.log
#listdir_platform_MCL.PDK-3.5_post-s60-clean_delta.log
#listdir_platform_MCL.PDK-3.5_post-s60-build-tools.log
#listdir_platform_MCL.PDK-3.5_post-s60-build-main.log



my $lowercase = 1;
my $useoutputfiles = 1;
main();



sub main()
{
  if($lowercase)
  {
    print "Running in lower case mode!\n";
  }
  my $path = shift @ARGV;
  my @listfiles = glob($path."listdir*");
  
  my $gt_base;
  my $gt_clean;
  my $gt_main;
  my $s60_base;
  my $s60_clean;
  my $s60_main;
  foreach my $file (@listfiles)
  {
    if($file =~ m/s60/)
    {
      if($file =~ m/baseline/)
      {
        $s60_base = parsefile($file);  
      }
      elsif($file =~ m/clean.log/)
      {
        $s60_clean = parsefile($file);
      }
      elsif($file =~ m/main.log/)
      {
        $s60_main = parsefile($file);
      }
    }
    else
    {
      if($file =~ m/baseline/)
      {
        $gt_base = parsefile($file);  
      }
      elsif($file =~ m/clean.log/)
      {
        $gt_clean = parsefile($file);
      }
      elsif($file =~ m/main.log/)
      {
        $gt_main = parsefile($file);
      } 
    }
  }

 
  
  my $gt_try = diff($gt_base, $gt_clean);
  my $gt_fail = diff($gt_base, $gt_main);
  my $gt_built = diff($gt_try, $gt_fail);

#  printgroup($gt_fail,'fail');
#  printgroup($gt_built,'built');


  my $s60_try = diff($s60_base, $s60_clean);
  my $s60_fail = diff($s60_base, $s60_main);
  my $s60_built = diff($s60_try, $s60_fail);

  my $s60_add = diff($s60_main,$s60_base); 
  my $gt_add = diff($gt_main,$gt_base); 
 

  my $try = union($gt_try,$s60_try); # All the stuff we try to build
  my $untouched = diff($gt_base,$try); # all the stuff we didn't try.

 #printgroup($try,"try");

  my $uptodate = finduptodate($path); # this is a bit dicey, 'cos it might get deleted/rebuilt by another part...
  
  
  printgroup(diff($untouched,$uptodate),"untouched"); # 'clean' doesn't remove headers if they are 'uptodate'

  my $rebuildfail = intersect(union($gt_built,$gt_add), $s60_fail); #everything built in GT, minus stuff that failed in S60 
  my $rebuilt = intersect($gt_built, $s60_built); # everything built in both
  my $built = diff(diff(union($gt_built, $s60_built),$rebuilt),$rebuildfail); # everything built, minus rebuilt, minus rebuildfail  
  my $fail = diff(union($gt_fail,$s60_fail),$rebuildfail); #everyhting that failed, minus the rebuild failures

  my $added = diff(union($gt_add,$s60_add),$rebuildfail); #all the stuff that got added, minus the stuff that filaed to rebuild

  printgroup($built,"built"); 
  printgroup($rebuilt,'rebuilt');
  printgroup($rebuildfail,'rebuildfail');
  printgroup($added,"added");
  

  printgroup($fail,"failed");
  
  $uptodate = diff($uptodate,union($added,$built)); #remove all stuff in other categories...'uptodate' was added late in the program
  printgroup($uptodate,"uptodate"); # uptodate list isn't that good at the moment...put it last.
}

sub printgroup($$)
{
  my $group = shift;
  my $label = shift;
  foreach my $key (sort keys %$group)
  {
    print $label." : ".$key."\t".$group->{$key}."\n";
  }
  if($useoutputfiles)
  {
    my $filename = "results_$label.log"; 
    open(FILE,">$filename") or die "Couldn't open $filename\n";
    foreach my $key (sort keys %$group)
    {
      print FILE $key."\n";
    }    
    close FILE;
  }
}

sub diff($s1,$s2)
{
  my $s1 = shift;
  my $s2 = shift;
  my %r;
  foreach my $key (keys %$s1)
  {
    if(!defined $s2->{$key})
    {
      $r{$key} = $s1->{$key};
    }
  }
  return \%r;
}



sub intersect($s1,$s2)
{
  my $s1 = shift;
  my $s2 = shift;
  my %r;
  foreach my $key (keys %$s1)
  {
    if(defined $s2->{$key})
    {
      $r{$key} = $s2->{$key};
    }
  }
  return \%r;
}

sub union($s1,$s2)
{
  my $s1 = shift;
  my $s2 = shift;
  my %r;
  foreach my $key (keys %$s1)
  {
    $r{$key} = $s1->{$key};
  }
  foreach my $key (keys %$s2) #lazy
  {
    $r{$key} = $s2->{$key};
  }
  return \%r;
}


sub finduptodate($path)
{
  my $path = shift;
  my @files = glob($path."*CLEAN*compile.log");
  my %results;
  foreach my $file (@files)
  {
    print "Reading $file\n";
    open(FILE,"<$file") or die "Cannot open $file\n";
    while( my $line = <FILE>)
    {
      if($line =~ m/<info>Up-to-date:\s+(.+)<\/info>/)
      {
        my $str = $1;
        $str =~ s/^\S:\///;
        if($lowercase)
        {
          $str = lc($str);
        }      
 
        $results{$str} = "";
#        print $str;
      }
    }     
    close FILE;
  }

  return \%results;
}


sub parsefile($file)
{
  my $file = shift;
#  my @results;
  my %results;
  print "Reading $file\n";
  open(FILE,"<$file") or die "Couldn't open $file\n";
  while(my $line = <FILE>)
  {
    $line =~ s/\n//;
    if($line =~ m/\S+/)
    {
      if( $line !~/^epoc32/ ) #latest lists sometimes don't have this...
      {
        $line = "epoc32/".$line;
      }
      if( $line !~/epoc32\/build\// ) #ignore epoc32/build
      {
        if($lowercase)
        {
          $line = lc($line);
        }      
        $results{$line} = "";
#       $results{$line} = $file; #debugging
      }  

#     push(@results,$line)
    }
  }
  close FILE;
#  return \@results;
  return \%results;
}