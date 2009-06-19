#!/usr/bin/perl


use strict;

#my $cache = "d:/HG_cache/";
#my $master = "//v800008/Builds01/";
my $cache = shift @ARGV;
my $csv = shift @ARGV;
my $generated = shift @ARGV;
my @recover;
my @nospace;
my $exitcode = 0;

if(defined $cache && defined $generated && defined $csv)
{ 
  print "Cache:$cache\nIn:$csv\nOut:$generated\n";
  
  # Format the cache directory path
  if ( $cache !~ /(.*)[\\\/]$/ )
  {
	$cache .= "/";
  }
  
  open(IN, "<$csv") or die "Couldn't open $csv for reading";
  open(OUT,">$generated") or die "Couldn't open $generated for writing";
  my $header = <IN>;
  print OUT $header;
  while( my $line = <IN>)
  {
    my @args = split(',',$line);
    my $repo = shift @args;
	my $master = "";
	if ( $repo =~ m/^(.*\/)((oss|rnd|sfl)\/.*\/)$/i )
	{
		$master = $1;
		$repo = $2;
	}
    if(-d $master.$repo.".hg")
    {
  #    print "Found:\t".$master.$repo.".hg\n";
      my $cmd;
      if(-d $cache.$repo.".hg") # update
      {
        $cmd = "hg pull -R $cache$repo $master$repo";
      }
      else #clone
      {
        #taken from the normal clone script...
        my @dirs = split ('\/', $cache.$repo);
        my $destdir = pop @dirs;
        my $path = "";    
        foreach my $dir (@dirs)
          {
          $path = ($path eq "") ? $dir : "$path/$dir";
          if (!-d $path)
            {
            mkdir $path;
            }
          }
            
        $cmd = "hg clone -U $master$repo $cache$repo";
      }  
      if(cache($cmd))
        {
          print OUT $cache.$repo.",".join(',', @args);
        }
      else
        {
           print OUT $master.$repo.",".join(',', @args);
		   $exitcode = 1;
        }
    }
    else
    {
      print "Error: cannot find ".$master.$repo.".hg\n";
	  $exitcode = 1;
    }
  }
  
  close OUT;
  close IN;
}
else
{
  print "Usage: <cache_path> <source_csv> <generated_csv>";
  $exitcode = 1;
}

foreach my $line (@recover)
{
  print "WARNING: HG Recover: $line\n";
}
foreach my $line (@nospace)
{
  print "WARNING: No Space: $line\n";
}

exit $exitcode;

sub cache($cmd)
{
  my $cmd = shift;
  print "$cmd\n";
  
  open(CMD, "$cmd 2>&1 |") or die "Couldn't execute $cmd";
  while(my $line = <CMD>)
  {
#    print $line;
    # parse the output for failures. On fail return 0;
    if($line =~ m/abort/i)
    {
      print $line;
      if($line =~ m/hg\s+recover/i)
      {
        push(@recover, $cmd);
      }
      elsif($line =~ m/No\s+space/i)
      {
        push(@nospace, $cmd);
      }
      close CMD;
      return 0;
    }    
  }
  close CMD;
  return 1;
}