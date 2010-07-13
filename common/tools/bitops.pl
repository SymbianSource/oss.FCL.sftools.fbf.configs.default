use strict;

use FindBin;
use lib "$FindBin::Bin/lib";

use Date::Calc qw{Delta_DHMS This_Year};
use DBI;

use Getopt::Long;

my $help = 0;
my $create = 0;
my $id = '';
my $file = '';
my $verbose = 0;
my $dryrun = 0;
GetOptions((
	'create!' => \$create,
	'file=s' => \$file,
	'id=s' => \$id,
	'help!' => \$help,
	'verbose!' => \$verbose,
	'dryrun!' => \$dryrun,
));

if ($help||!$file)
{
  print <<_EOH;
bitops
Executes operations on the BIT db

Usage: bitops.pl [-c|-i ID] -f FILE

Options:
  -h, --help            Show this help message and exit
  -c, --create          Create entry for a new build then set the attributes to
                        the values provided in FILE. Returns the build id for
                        the newly created build.
  -i ID, --id ID        Update entry for build number ID with the values
                        provided in FILE.
  -f FILE, --file FILE  Use attributes in FILE to create/update the build info
                        See below for file format.
  -v, --verbose         Increase info level
  -d, --dryrun          Only show what would happen, do not actually interact
                        with the DB
  
File format:
  File must be a list of attribute-value-pairs. One avp per line, on each line
  attribute and value are separated by a tab.
  These are the supported attributes (in brackets the syntax of the value):
  - build_id_string
  - package (VALUE)
  - platform (VALUE)
  - started (VALUE)
  - finished (VALUE)
  - creator (VALUE)
  - machine (VALUE)
  - build_brag (VALUE)
  - test_brag (VALUE)
  - baseline_short (VALUE)
  - sources_short (VALUE)
  - envinfo (TOOL,VERSION)
  - failure (CATEGORY,COUNT)
  - report (NAME,URL,TYPE)
  - content (NAME,URL,REVISION)
  - label (VALUE)
_EOH
	exit(0);
}

my $db = undef;

my $builds_entry = {};
my $envinfo = [];
my $failures = [];
my $reports = [];
my $content = [];
my $labels = [];
my $testing_entry = {};

sub ConnectToDB()
{
	$db = DBI->connect('DBI:mysql:bit:v800016:3306','fbf','mysql')
		or die("Couldn't connect to database: " . DBI->errstr()) if (!$dryrun);
}

sub DisconnectFromDB()
{
	$db->disconnect() if (!$dryrun);
}

sub parse_file
{
  my ($file) = @_;
  
  print "Reading $file...\n" if ($verbose);
  
  open(FILE, $file) or die "Can't open file $file";
  
  while (<FILE>)
  {
    my $line = $_;
    chomp $line;
    $line =~ s/^\s//g;
    $line =~ s/\s$//g;
    next if ($line !~ /[^\s]/);
    
    if ($line =~ /([^\t]+)\t([^\t]+)/)
    {
      my $attr = $1;
      my $value = $2;
      
      if ($attr =~ /^(build_id_string|package|platform|started|finished|creator|machine|build_brag|test_brag|baseline_short|sources_short)$/i)
      {
        $attr = lc($attr);
        print " found ($attr,$value) for table 'build'\n" if ($verbose);
        $builds_entry->{$attr}=$value;
      }
      elsif ($attr =~ /^(envinfo)$/i)
      {
        $attr = lc($attr);
        if ($value =~ /([^,]*),([^,]*)/)
        {
          my $tool = $1;
          my $version = $2;
          
          if (!$tool || !$version)
          {
            print "ERROR: Tool or version empty: \"$value\"\n";
            return 1;
          }
          print " found ($tool,$version) for table 'envinfo'\n" if ($verbose);
          push(@{$envinfo}, {tool=>$tool, version=>$version});
        }
        else
        {
          print "ERROR: Could not understand value of envinfo: \"$value\"\n";
          return 1;
        }
      }
      elsif ($attr =~ /^(failure)$/i)
      {
        $attr = lc($attr);
        if ($value =~ /([^,]*),([^,]*)/)
        {
          my $category = $1;
          my $count = $2;
          
          if (!$category || !$count)
          {
            print "ERROR: Category or count empty: \"$value\"\n";
            return 1;
          }
          print " found ($category,$count) for table 'failures'\n" if ($verbose);
          push(@{$failures}, {category=>$category, count=>$count});
        }
        else
        {
          print "ERROR: Could not understand value of failure: \"$value\"\n";
          return 1;
        }
      }
      elsif ($attr =~ /^(report)$/i)
      {
        $attr = lc($attr);
        if ($value =~ /([^,]*),([^,]*),([^,]*)/)
        {
          my $name = $1;
          my $url = $2;
          my $type = $3;
          
          if (!$name || !$url || !$type)
          {
            print "ERROR: Name, url or type empty: \"$value\"\n";
            return 1;
          }
          print " found ($name,$url,$type) for table 'reports'\n" if ($verbose);
          push(@{$reports}, {name=>$name, url=>$url, type=>$type});
        }
        else
        {
          print "ERROR: Could not understand value of report: \"$value\"\n";
          return 1;
        }
      }
      elsif ($attr =~ /^(content)$/i)
      {
        $attr = lc($attr);
        if ($value =~ /([^,]*),([^,]*),([^,]*)/)
        {
          my $name = $1;
          my $url = $2;
          my $revision = $3;
          
          if (!$name || !$url || !$revision)
          {
            print "ERROR: Name, url or revision empty: \"$value\"\n";
            return 1;
          }
          print " found ($name,$url,$revision) for table 'content'\n" if ($verbose);
          push(@{$content}, {name=>$name, url=>$url, revision=>$revision});
        }
        else
        {
          print "ERROR: Could not understand value of content: \"$value\"\n";
          return 1;
        }
      }
      elsif ($attr =~ /^(label)$/i)
      {
        print " found ($attr,$value) for table 'labels'\n" if ($verbose);
        push(@{$labels}, {label=>$value});
      }
      else
      {
        print "ERROR: Could not understand avp: \"$line\"\n";
        return 1;
      }
      
    }
    else
    {
      print "WARNING: line \"$line\" does not seem a correctly formed attribute-value-pair (tab-separated)\n";
      return 1;
    }
  }
  
  close(FILE);
  return 0;
}

# MAIN PROGRAM

my $r=parse_file($file);
if ($r)
{
  print "Error while parsing file $file\n";
  exit(1);
}

print "Executing SQL commands...\n" if ($verbose);

ConnectToDB();
my $newbuildid = -1;
if ($create)
{
  if (keys %{$builds_entry})
  {
    my $field_list = '';
    my $qm_list = '';
    my @fields = ();
    for my $field (keys %{$builds_entry})
    {
      $field_list .= "$field,";
      $qm_list .= "?,";
      push(@fields,$builds_entry->{$field});
    }
    $field_list =~ s/,$//;
    $qm_list =~ s/,$//;
    
    my $query = $db->prepare("insert into builds ($field_list) values ($qm_list)")
      or die("Couldn't prepare query insert into builds: " . $db->errstr()) if (!$dryrun);

    $query->execute(@fields)
		  or print "WARNING: Couldn't execute insert into builds (@fields): " . $db->errstr() . "\n" if (!$dryrun);
		  
		$newbuildid = $db->last_insert_id(undef, undef, undef, undef) if (!$dryrun);
  }
  if (@{$envinfo})
  {
    my $query = $db->prepare_cached("insert into envinfo (build_id,tool,version) values ($newbuildid,?,?)")
      or die("Couldn't prepare query insert into envinfo: " . $db->errstr()) if (!$dryrun);
      
    for my $entry (@{$envinfo})
    {
      my $tool = $entry->{tool};
      my $version = $entry->{version};
      
      $query->execute($tool, $version)
		    or print "WARNING: Couldn't execute insert into envinfo ($tool,$version): " . $db->errstr() . "\n" if (!$dryrun);
    }
  }
  if (@{$failures})
  {
    print " prepare_cached 'insert into failures (build_id,category,count) values ($newbuildid,?,?)'\n" if ($verbose);
    my $query = $db->prepare_cached("insert into failures (build_id,category,count) values ($newbuildid,?,?)")
      or die("Couldn't prepare query insert into failures: " . $db->errstr()) if (!$dryrun);
      
    for my $entry (@{$failures})
    {
      my $category = $entry->{category};
      my $count = $entry->{count};
    
      print " execute '$category, $count'\n" if ($verbose);  
      $query->execute($category, $count)
		    or print "WARNING: Couldn't execute insert into failures ($category,$count): " . $db->errstr() . "\n" if (!$dryrun);
    }
  }
  if (@{$reports})
  {
    print " prepare_cached 'insert into reports (build_id,name,url,type) values ($newbuildid,?,?,?)'\n" if ($verbose);
    my $query = $db->prepare_cached("insert into reports (build_id,name,url,type) values ($newbuildid,?,?,?)")
      or die("Couldn't prepare query insert into reports: " . $db->errstr()) if (!$dryrun);
      
    for my $entry (@{$reports})
    {
      my $name = $entry->{name};
      my $url = $entry->{url};
      my $type = $entry->{type};
    
      print " execute '$name, $url, $type'\n" if ($verbose);  
      $query->execute($name, $url, $type)
		    or print "WARNING: Couldn't execute insert into reports ($name,$url,$type): " . $db->errstr() . "\n" if (!$dryrun);
    }
  }
  if (@{$content})
  {
    print " prepare_cached 'insert into content (build_id,name,url,revision) values ($newbuildid,?,?,?)'\n" if ($verbose);
    my $query = $db->prepare_cached("insert into content (build_id,name,url,revision) values ($newbuildid,?,?,?)")
      or die("Couldn't prepare query insert into content: " . $db->errstr()) if (!$dryrun);
      
    for my $entry (@{$content})
    {
      my $name = $entry->{name};
      my $url = $entry->{url};
      my $revision = $entry->{revision};
    
      print " execute '$name, $url, $revision'\n" if ($verbose);  
      $query->execute($name, $url, $revision)
		    or print "WARNING: Couldn't execute insert into content ($name,$url,$revision): " . $db->errstr() . "\n" if (!$dryrun);
    }
  }
  if (@{$labels})
  {
    print " prepare_cached 'insert into labels (build_id,label) values ($newbuildid,?)'\n" if ($verbose);
    my $query = $db->prepare_cached("insert into labels (build_id,label) values ($newbuildid,?)")
      or die("Couldn't prepare query insert into labels: " . $db->errstr()) if (!$dryrun);
      
    for my $entry (@{$labels})
    {
      my $label = $entry->{label};
    
      print " execute '$label'\n" if ($verbose);  
      $query->execute($label)
		    or print "WARNING: Couldn't execute insert into revision ($label): " . $db->errstr() . "\n" if (!$dryrun);
    }
  }
  print "new build id: $newbuildid\n";
}
else
{
  print "Updating build $id\n";
  
  if (keys %{$builds_entry})
  {
    my $field_list = '';
    my @fields = ();
    for my $field (keys %{$builds_entry})
    {
      $field_list .= "$field=?,";
      push(@fields,$builds_entry->{$field});
    }
    $field_list =~ s/,$//;
    
    print " prepare 'update builds set $field_list where id=$id'\n" if ($verbose);
    my $query = $db->prepare("update builds set $field_list where id=$id")
      or die("Couldn't prepare query update builds: " . $db->errstr()) if (!$dryrun);

    print " execute '@fields'\n" if ($verbose);
    $query->execute(@fields)
		  or print "WARNING: Couldn't execute update builds (@fields): " . $db->errstr() . "\n" if (!$dryrun);
  }
  if (@{$envinfo})
  {
    print " prepare 'delete from envinfo where build_id=$id'\n" if ($verbose);
    my $delete_query = $db->prepare("delete from envinfo where build_id=$id")
      or die("Couldn't prepare query delete from envinfo: " . $db->errstr()) if (!$dryrun);
    
    print " execute ''\n" if ($verbose);  
    $delete_query->execute()
		    or print "WARNING: Couldn't execute delete from envinfo: " . $db->errstr() . "\n" if (!$dryrun);
    
    print " prepare 'insert into envinfo (build_id,tool,version) values ($id,?,?)'\n" if ($verbose);
    my $query = $db->prepare_cached("insert into envinfo (build_id,tool,version) values ($id,?,?)")
      or die("Couldn't prepare query insert into envinfo: " . $db->errstr()) if (!$dryrun);
      
    for my $entry (@{$envinfo})
    {
      my $tool = $entry->{tool};
      my $version = $entry->{version};
    
      print " execute '$tool,$version'\n" if ($verbose);
      $query->execute($tool, $version)
		    or print "WARNING: Couldn't execute insert into envinfo ($tool,$version): " . $db->errstr() . "\n" if (!$dryrun);
    }
  }
  if (@{$failures})
  {
    print " prepare 'delete from failures where build_id=$id'\n" if ($verbose);
    my $delete_query = $db->prepare("delete from failures where build_id=$id")
      or die("Couldn't prepare query delete from failures: " . $db->errstr()) if (!$dryrun);
    
    print " execute ''\n" if ($verbose);  
    $delete_query->execute()
		    or print "WARNING: Couldn't execute delete from failures: " . $db->errstr() . "\n" if (!$dryrun);
    
    print " prepare 'insert into failures (build_id,category,count) values ($id,?,?)'\n" if ($verbose);
    my $query = $db->prepare_cached("insert into failures (build_id,category,count) values ($id,?,?)")
      or die("Couldn't prepare query insert into failures: " . $db->errstr()) if (!$dryrun);
      
    for my $entry (@{$failures})
    {
      my $category = $entry->{category};
      my $count = $entry->{count};
    
      print " execute '$category,$count'\n" if ($verbose);  
      $query->execute($category, $count)
		    or print "WARNING: Couldn't execute insert into failures ($category,$count): " . $db->errstr() . "\n" if (!$dryrun);
    }
  }
  if (@{$reports})
  {
    print " prepare 'delete from reports where build_id=$id'\n" if ($verbose);
    my $delete_query = $db->prepare("delete from reports where build_id=$id")
      or die("Couldn't prepare query delete from reports: " . $db->errstr()) if (!$dryrun);
    
    print " execute ''\n" if ($verbose);  
    $delete_query->execute()
		    or print "WARNING: Couldn't execute delete from reports: " . $db->errstr() . "\n";
    
    print " prepare 'insert into reports (build_id,name,url,type) values ($id,?,?,?)'\n" if ($verbose);
    my $query = $db->prepare_cached("insert into reports (build_id,name,url,type) values ($id,?,?,?)")
      or die("Couldn't prepare query insert into failures: " . $db->errstr()) if (!$dryrun);
      
    for my $entry (@{$reports})
    {
      my $name = $entry->{name};
      my $url = $entry->{url};
      my $type = $entry->{type};
    
      print " execute '$name,$url,$type'\n" if ($verbose);  
      $query->execute($name, $url, $type)
		    or print "WARNING: Couldn't execute insert into reports ($name,$url,$type): " . $db->errstr() . "\n" if (!$dryrun);
    }
  }
  if (@{$content})
  {
    print " prepare 'delete from content where build_id=$id'\n" if ($verbose);
    my $delete_query = $db->prepare("delete from content where build_id=$id")
      or die("Couldn't prepare query delete from content: " . $db->errstr()) if (!$dryrun);
    
    print " execute ''\n" if ($verbose);  
    $delete_query->execute()
		    or print "WARNING: Couldn't execute delete from content: " . $db->errstr() . "\n" if (!$dryrun);
    
    print " prepare 'insert into content (build_id,name,url,revision) values ($id,?,?,?)'\n" if ($verbose);
    my $query = $db->prepare_cached("insert into content (build_id,name,url,revision) values ($id,?,?,?)")
      or die("Couldn't prepare query insert into content: " . $db->errstr()) if (!$dryrun);
      
    for my $entry (@{$content})
    {
      my $name = $entry->{name};
      my $url = $entry->{url};
      my $revision = $entry->{revision};
    
      print " execute '$name,$url,$revision'\n" if ($verbose);  
      $query->execute($name, $url, $revision)
		    or print "WARNING: Couldn't execute insert into content ($name,$url,$revision): " . $db->errstr() . "\n" if (!$dryrun);
    }
  }
  if (@{$labels})
  {
    print " prepare 'delete from labels where build_id=$id'\n" if ($verbose);
    my $delete_query = $db->prepare("delete from labels where build_id=$id")
      or die("Couldn't prepare query delete from labels: " . $db->errstr()) if (!$dryrun);
    
    print " execute ''\n" if ($verbose);  
    $delete_query->execute()
		    or print "WARNING: Couldn't execute delete from labels: " . $db->errstr() . "\n" if (!$dryrun);
    
    print " prepare 'insert into labels (build_id,label) values ($id,?)'\n" if ($verbose);
    my $query = $db->prepare_cached("insert into label (build_id,label) values ($id,?)")
      or die("Couldn't prepare query insert into label: " . $db->errstr()) if (!$dryrun);
      
    for my $entry (@{$labels})
    {
      my $label = $entry->{label};
      
      print " execute '$label'\n" if ($verbose);  
      $query->execute($label)
		    or print "WARNING: Couldn't execute insert into labels ($label): " . $db->errstr() . "\n" if (!$dryrun);
    }
  }
}
DisconnectFromDB();


