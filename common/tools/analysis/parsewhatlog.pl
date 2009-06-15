#!/usr/bin/perl
#parsewhatlog - parses a whatlog, gives csv output.
use strict;

#<whatlog bldinf='t:/sf/mw/ipappprotocols/sipconnproviderplugins/sipstatemachine/group/bld.inf' mmp='' config='winscw_udeb.whatlog'>
#<export destination='t:/epoc32/rom/include/sipstatemachine.iby' source='t:/sf/mw/ipappprotocols/sipconnproviderplugins/sipstatemachine/group/SipStateMachine.iby'/>
#</whatlog>
#<whatlog bldinf='t:/sf/os/mm/mmplugins/3gplib/group/bld.inf' mmp='t:/sf/os/mm/mmplugins/3gplib/impl/group/3gpmp4lib.mmp' config='winscw_udeb.whatlog'>
#<build>t:/epoc32/release/winscw/udeb/3gpmp4lib.lib</build>
#<build>t:/epoc32/release/winscw/udeb/3gpmp4lib.lib</build>
#<build>t:/epoc32/release/winscw/udeb/3gpmp4lib.dll</build>
#</whatlog>
#<whatlog bldinf='t:/sf/mw/messagingmw/messagingfw/msgconf/group/bld.inf' mmp='t:/sf/mw/messagingmw/messagingfw/msgconf/group/messaging_config.mmp' config='winscw_udeb.whatlog'>
#<bitmap>t:/epoc32/data/z/resource/messaging/bif/vclp.mbm</bitmap>
#<bitmap>t:/epoc32/release/winscw/udeb/z/resource/messaging/bif/vclp.mbm</bitmap>
#<bitmap>t:/epoc32/release/winscw/urel/z/resource/messaging/bif/vclp.mbm</bitmap>
#<bitmap>t:/epoc32/localisation/vclp/mbm/vclp0.bmp</bitmap>
#<bitmap>t:/epoc32/localisation/group/vclp.info</bitmap>
#</whatlog>
#<whatlog bldinf='t:/sf/mw/mmmw/mmmiddlewarefws/mmfw/sounddev/group_pluginsupport/bld.inf' mmp='t:/sf/mw/mmmw/mmmiddlewarefws/mmfw/sounddev/PlatSec/MMPFiles/Sounddevice/aacdecoderconfigci.mmp' config='winscw_udeb.whatlog'>
#<resource>t:/epoc32/data/z/resource/plugins/aacdecoderconfigci.rsc</resource>
#<resource>t:/epoc32/release/winscw/udeb/z/resource/plugins/aacdecoderconfigci.rsc</resource>
#<resource>t:/epoc32/release/winscw/urel/z/resource/plugins/aacdecoderconfigci.rsc</resource>
#<resource>t:/epoc32/localisation/aacdecoderconfigci/rsc/aacdecoderconfigci.rpp</resource>
#<resource>t:/epoc32/localisation/group/aacdecoderconfigci.info</resource>
#</whatlog>


my $keepgoing = 1;
main();

sub cleanpath($)
{
  my $str = lc(shift); #drop the case.
  $str =~ s/^\S://; #remove drive letter
  $str =~ s/^\///; # some custom makefiles report aboslute path
  $str =~ s/\\/\//g; #switch the path
  $str =~ s/\/\//\//g;#we have some double slashes in some resources... 
  return $str;
}
sub ext($)
{
  my $str = shift;
  $str =~ s/\S+\.//; #may fail...
  return $str;
}

sub main()
{
  my $path = shift @ARGV;
#  my @files = glob($path."/*whatlog*WHAT_*compile.log");
  my @files = glob($path."/*whatlog*_*compile.log"); 
  foreach my $filename (@files)
  {
#    print $filename."\n";
    parsefile($filename);
  }  
}
sub parsefile($filename)
{  
  my $filename = shift;
  open(FILE,"<$filename") or die "Couldn't open filename\n";
  
  #I'm using previous formatting stypes from the flm parsing...ie location, bld.inf, makefile, type, target,extension
  print "location,bldinf,makefile,type,target,extension\n";
  my $bldinf = "";
  my $makefile = "";
  my $inrecipe = 0;
  
  my $linecount = 0;
  while(my $line = <FILE>)
  {
    ++$linecount;
    if($line =~ m/^<whatlog bldinf='(\S+)' mmp='(\S*)' config='\S+'>/) #brittle
    {
      $bldinf = $1;
      $makefile = $2;
    }
    elsif($line =~ m/^<\/whatlog>/)
    {
      $bldinf = "";
      $makefile = "";
    }
    elsif($line =~ m/^<bitmap>(\S+)<\/bitmap>/)
    {
      if($bldinf eq "" || $makefile eq "" && !$keepgoing)
      {
        die "$filename($linecount) bldinf=$bldinf makefile=$makefile: $line\n";
      }
      print "$filename($linecount),".cleanpath($bldinf).",".cleanpath($makefile).",bitmap,".cleanpath($1).",".ext($1)."\n";        
    }
    elsif($line =~ m/^<build>(\S+)<\/build>/)
    {
      if($bldinf eq "" || $makefile eq "" && !$keepgoing)
      {
        die "$filename($linecount) bldinf=$bldinf makefile=$makefile : $line\n";
      }
      print "$filename($linecount),".cleanpath($bldinf).",".cleanpath($makefile).",binary,".cleanpath($1).",".ext($1)."\n";        
    }
    elsif($line =~ m/^<resource>(\S+)<\/resource>/)
    {
      if($bldinf eq "" || $makefile eq "" && !$keepgoing)
      {
        die "$filename($linecount) bldinf=$bldinf makefile=$makefile : $line\n";
      }
      print "$filename($linecount),".cleanpath($bldinf).",".cleanpath($makefile).",resource,".cleanpath($1).",".ext($1)."\n";        
    }
    
    #<export destination='t:/epoc32/rom/include/sipstatemachine.iby' source='t:/sf/mw/ipappprotocols/sipconnproviderplugins/sipstatemachine/group/SipStateMachine.iby'/>
    elsif($line =~ m/^<export destination='(\S+)' source='(\S+)'\/>/)
    {
      if($bldinf eq "" )
      {
        die "$filename($linecount) bldinf=$bldinf: $line\n";
      }
      print "$filename($linecount),".cleanpath($bldinf).",".cleanpath($2).",export,".cleanpath($1).",".ext($1)."\n";    
    }
  #<recipe name='tem' target='91e4e9b4af8b5c84bbac43a2419a4ce3_RELEASABLES' host='LON-ENGBUILD87' layer='os' component='localesupport' bldinf='t:/sf/os/kernelhwsrv/localisation/localesupport/bld.inf' mmp='' config='winscw_urel.whatlog' platform='WINSCW' phase='BITMAP' source='copy_default.mk_RELEASABLES'>
    
    elsif($line =~ m/<recipe.+bldinf='(\S+)'.+source='(\S+)'>/)
    {
      $bldinf = $1;
      $makefile = $2;
      $inrecipe = 1;    
    }
    elsif($inrecipe && $line =~ m/^(\S:\S+)/)
    {
      if($bldinf eq "" || $makefile eq "" && !$keepgoing)
      {
        die "$filename($linecount) bldinf=$bldinf makefile=$makefile : $line\n";
      }
      my $str = cleanpath($1);
      
      print "$filename($linecount),".cleanpath($bldinf).",".cleanpath($makefile).",custom,".cleanpath($str).",".ext($str)."\n";
    }
    
    elsif($line =~ m/<\/recipe>/)
    {
      $bldinf = "";
      $makefile = "";
      $inrecipe = 0;
    }
    
      
  }
  close FILE;
}