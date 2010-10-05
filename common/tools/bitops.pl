use strict;

use FindBin;
use lib "$FindBin::Bin/lib";

use Date::Calc qw{Delta_DHMS This_Year};
use DBI;

use Getopt::Long;

my $help = 0;
my $create = 0;
my $stringcheck = 0;
my $file = '';
my $id = '';
my $append = 0;
my $delete = '';
my $verbose = 0;
my $dryrun = 0;
GetOptions((
	'create!' => \$create,
	'stringcheck!' => \$stringcheck,
	'file=s' => \$file,
	'update=s' => \$id,
	'append!' => \$append,
	'remove=s' => \$delete,
	'help!' => \$help,
	'verbose!' => \$verbose,
	'dryrun!' => \$dryrun,
));

if ($help||(($create||$id)&&!$file)||(!$create&&!$id&&!$delete))
{
  print <<_EOH;
bitops
Executes operations on the BIT db

Usage:
  bitops.pl -c [-n] -f FILE
  bitops.pl -u ID [-a] -f FILE
  bitops.pl -r ID

Options:
  -h, --help            Show this help message and exit
  -c, --create          Create entry for a new build then set the attributes to
                        the values provided in FILE. Returns the build id for
                        the newly created build.
  -s, --stringcheck     Check that the build_id_string is unique, otherwise do
                        not create build.
  -u ID, --update ID    Update entry for build number ID with the values
                        provided in FILE. For the update operation only the
                        entry in the BUILDS table will be actually updated, the
                        entries in the other tables will be deleted, then the
                        data supplied will be inserted (this behaviour can be
                        changed with the -a option)
  -a, --append          Do not delete entries in the tables different than
                        BUILDS before inserting the data supplied. This option
                        must be used in conjuntion with -u (see info for -u
                        option for more information)
  -r, --remove          Remove build ID (entries in all tables) from the db
  -f FILE, --file FILE  Use attributes in FILE to create/update the build info
                        See below for file format.
  -v, --verbose         Increase info level
  -d, --dryrun          Only show what would happen, do not actually interact
                        with the DB
  
File format:
  File must be a list of attribute-value-pairs. One avp per line, on each line
  attribute and value are separated by a tab.
  These are the supported attributes (in brackets the syntax of the value) with
  the table they belong to:
  - build_id_string - BUILDS
  - package (VALUE) - BUILDS
  - platform (VALUE) - BUILDS
  - started (VALUE) - BUILDS
  - finished (VALUE) - BUILDS
  - creator (VALUE) - BUILDS
  - machine (VALUE) - BUILDS
  - build_brag (VALUE) - BUILDS
  - test_brag (VALUE) - BUILDS
  - baseline_short (VALUE) - BUILDS
  - sources_short (VALUE) - BUILDS
  - envinfo (TOOL,VERSION) - ENVINFO
  - failure (CATEGORY,COUNT) - FAILURES
  - report (NAME,URL,TYPE) - REPORTS
  - content (NAME,URL,REVISION) - CONTENT
  - baseline (TYPE,PATH) - BASELINES
  - label (VALUE) - LABELS
  - test (NAME,TYPE,TARGET,STARTED,FINISHED,CASES_TOTAL,CASES_PASSED,
      CASES_FAILED,CASES_NOT_RUN,CASES_NOT_AVAILABLE,PASS_RATE) - TESTING
  
Note on the update operation:
_EOH
	exit(0);
}

my $db = undef;

my $builds_entry = {};
my $envinfo = [];
my $failures = [];
my $reports = [];
my $content = [];
my $baselines = [];
my $labels = [];
my $testing = [];

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
      
      if ($attr =~ /^(build_id_string|package|platform|started|finished|creator|machine|build_brag|test_brag|baseline_short|sources_short|id)$/i)
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
          
          if (!$category || !defined $count || $count eq '')
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
      elsif ($attr =~ /^(baseline)$/i)
      {
        $attr = lc($attr);
        if ($value =~ /([^,]*),([^,]*)/)
        {
          my $type = $1;
          my $path = $2;
          
          if (!$type || !$path)
          {
            print "ERROR: Type or path empty: \"$value\"\n";
            return 1;
          }
          print " found ($type,$path) for table 'baselines'\n" if ($verbose);
          push(@{$baselines}, {type=>$type, path=>$path});
        }
        else
        {
          print "ERROR: Could not understand value of baseline: \"$value\"\n";
          return 1;
        }
      }
      elsif ($attr =~ /^(label)$/i)
      {
        print " found ($attr,$value) for table 'labels'\n" if ($verbose);
        push(@{$labels}, {label=>$value});
      }
      elsif ($attr =~ /^(test)$/i)
      {
        $attr = lc($attr);
        if ($value =~ /([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*)/)
        {
          my $name=$1;
          my $type=$2;
          my $target=$3;
          my $started=$4;
          my $finished=$5;
          my $cases_total=$6;
          $cases_total=0 if (!$cases_total);
          my $cases_passed=$7;
          $cases_passed=0 if (!$cases_passed);
          my $cases_failed=$8;
          $cases_failed=0 if (!$cases_failed);
          my $cases_not_run=$9;
          $cases_not_run=0 if (!$cases_not_run);
          my $cases_not_available=$10;
          $cases_not_available=0 if (!$cases_not_available);
          my $pass_rate=$11;
          $pass_rate="0.0" if (!$pass_rate);
          
          if (!$name)
          {
            print "ERROR: Name is empty: \"$value\"\n";
            return 1;
          }
          print " found ($name,$type,$target,$started,$finished,$cases_total,$cases_passed,$cases_failed,$cases_not_run,$cases_not_available,$pass_rate) for table 'testing'\n" if ($verbose);
          push(@{$testing}, {name=>$name,type=>$type,target=>$target,started=>$started,finished=>$finished,
            cases_total=>$cases_total,cases_passed=>$cases_passed,cases_failed=>$cases_failed,cases_not_run=>$cases_not_run,
            cases_not_available=>$cases_not_available,pass_rate=>$pass_rate});
        }
        else
        {
          print "ERROR: Could not understand value of envinfo: \"$value\"\n";
          return 1;
        }
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
my $r=0;
$r=parse_file($file) if ($create||$id);
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
  if ($stringcheck && defined $builds_entry->{build_id_string} && !$dryrun)
  {
    print "Check that the build_id_string is unique\n" if ($verbose);
    my $query=$db->prepare("select id from builds where build_id_string='$builds_entry->{build_id_string}';");
    $query->execute();
    my @ids_existing=();
    my $id_existing=-1;
    $query->bind_columns(\$id_existing);
    while($query->fetch())
    {
      push(@ids_existing, $id_existing);
    }
    if (scalar(@ids_existing))
    {
      print "ERROR: The supplied build_id_string already exists for build id: @ids_existing. Not creating build.\n";
      exit(1);
	}
  }
  
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
		  or print "WARNING: Couldn't execute insert into builds ($field_list) values (@fields): " . $db->errstr() . "\n" if (!$dryrun);
		  
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
  if (@{$baselines})
  {
    print " prepare_cached 'insert into baselines (build_id,type,path) values ($newbuildid,?,?)'\n" if ($verbose);
    my $query = $db->prepare_cached("insert into baselines (build_id,type,path) values ($newbuildid,?,?)")
      or die("Couldn't prepare query insert into baselines: " . $db->errstr()) if (!$dryrun);
      
    for my $entry (@{$baselines})
    {
      my $type = $entry->{type};
      my $path = $entry->{path};
    
      print " execute '$type, $path'\n" if ($verbose);  
      $query->execute($type, $path)
		    or print "WARNING: Couldn't execute insert into baselines ($type,$path): " . $db->errstr() . "\n" if (!$dryrun);
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
  
  if (@{$testing})
  {
    print " prepare_cached 'insert into testing (build_id,name,type,target,started,finished,cases_total,cases_passed,cases_failed,cases_not_run,cases_not_available,pass_rate) values ($newbuildid,?,?,?,?,?,?,?,?,?,?,?)'\n" if ($verbose);
    my $query = $db->prepare_cached("insert into testing (build_id,name,type,target,started,finished,cases_total,cases_passed,cases_failed,cases_not_run,cases_not_available,pass_rate) values ($newbuildid,?,?,?,?,?,?,?,?,?,?,?)")
      or die("Couldn't prepare query insert into testing: " . $db->errstr()) if (!$dryrun);
      
    for my $entry (@{$testing})
    {
      my $name = $entry->{name};
      my $type = $entry->{type};
      my $target = $entry->{target};
      my $started = $entry->{started};
      my $finished = $entry->{finished};
      my $cases_total = $entry->{cases_total};
      my $cases_passed = $entry->{cases_passed};
      my $cases_failed = $entry->{cases_failed};
      my $cases_not_run = $entry->{cases_not_run};
      my $cases_not_available = $entry->{cases_not_available};
      my $pass_rate = $entry->{pass_rate};
    
      print " execute '$name,$type,$target,$started,$finished,$cases_total,$cases_passed,$cases_failed,$cases_not_run,$cases_not_available,$pass_rate'\n" if ($verbose);  
      $query->execute($name,$type,$target,$started,$finished,$cases_total,$cases_passed,$cases_failed,$cases_not_run,$cases_not_available,$pass_rate)
		    or print "WARNING: Couldn't execute insert into testing ($name,$type,$target,$started,$finished,$cases_total,$cases_passed,$cases_failed,$cases_not_run,$cases_not_available,$pass_rate): " . $db->errstr() . "\n" if (!$dryrun);
    }
  }
  print "new build id: $newbuildid\n";
}
elsif ($id)
{
  print "Updating build $id\n";
  
  if (defined $builds_entry->{id} && $id != $builds_entry->{id})
  {
    print "ERROR: The build id specified in the input file is different than the one on the command line. Not updating build.\n";
    exit(1);
  }
  
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
	if (!$append)
	{
      print " prepare 'delete from envinfo where build_id=$id'\n" if ($verbose);
      my $delete_query = $db->prepare("delete from envinfo where build_id=$id")
        or die("Couldn't prepare query delete from envinfo: " . $db->errstr()) if (!$dryrun);
    
      print " execute ''\n" if ($verbose);  
      $delete_query->execute()
	    or print "WARNING: Couldn't execute delete from envinfo: " . $db->errstr() . "\n" if (!$dryrun);
    }
    
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
    if (!$append)
	{
      print " prepare 'delete from failures where build_id=$id'\n" if ($verbose);
      my $delete_query = $db->prepare("delete from failures where build_id=$id")
        or die("Couldn't prepare query delete from failures: " . $db->errstr()) if (!$dryrun);
    
      print " execute ''\n" if ($verbose);  
      $delete_query->execute()
	    or print "WARNING: Couldn't execute delete from failures: " . $db->errstr() . "\n" if (!$dryrun);
    }
	    
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
    if (!$append)
	{
      print " prepare 'delete from reports where build_id=$id'\n" if ($verbose);
      my $delete_query = $db->prepare("delete from reports where build_id=$id")
        or die("Couldn't prepare query delete from reports: " . $db->errstr()) if (!$dryrun);
    
      print " execute ''\n" if ($verbose);  
      $delete_query->execute()
	    or print "WARNING: Couldn't execute delete from reports: " . $db->errstr() . "\n";
    }
    
    print " prepare 'insert into reports (build_id,name,url,type) values ($id,?,?,?)'\n" if ($verbose);
    my $query = $db->prepare_cached("insert into reports (build_id,name,url,type) values ($id,?,?,?)")
      or die("Couldn't prepare query insert into reports: " . $db->errstr()) if (!$dryrun);
      
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
    if (!$append)
	{
	  print " prepare 'delete from content where build_id=$id'\n" if ($verbose);
      my $delete_query = $db->prepare("delete from content where build_id=$id")
        or die("Couldn't prepare query delete from content: " . $db->errstr()) if (!$dryrun);
    
      print " execute ''\n" if ($verbose);  
      $delete_query->execute()
	    or print "WARNING: Couldn't execute delete from content: " . $db->errstr() . "\n" if (!$dryrun);
    }
    
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
  if (@{$baselines})
  {
    if (!$append)
	{
	  print " prepare 'delete from baselines where build_id=$id'\n" if ($verbose);
      my $delete_query = $db->prepare("delete from baselines where build_id=$id")
        or die("Couldn't prepare query delete from baselines: " . $db->errstr()) if (!$dryrun);
    
      print " execute ''\n" if ($verbose);  
      $delete_query->execute()
	    or print "WARNING: Couldn't execute delete from baselines: " . $db->errstr() . "\n" if (!$dryrun);
    }
    
    print " prepare 'insert into baselines (build_id,type,path) values ($id,?,?)'\n" if ($verbose);
    my $query = $db->prepare_cached("insert into baselines (build_id,type,path) values ($id,?,?)")
      or die("Couldn't prepare query insert into baselines: " . $db->errstr()) if (!$dryrun);
      
    for my $entry (@{$baselines})
    {
      my $type = $entry->{type};
      my $path = $entry->{path};
    
      print " execute '$type,$path'\n" if ($verbose);  
      $query->execute($type, $path)
		    or print "WARNING: Couldn't execute insert into baselines ($type,$path): " . $db->errstr() . "\n" if (!$dryrun);
    }
  }
  if (@{$labels})
  {
    if (!$append)
	{
	  print " prepare 'delete from labels where build_id=$id'\n" if ($verbose);
      my $delete_query = $db->prepare("delete from labels where build_id=$id")
        or die("Couldn't prepare query delete from labels: " . $db->errstr()) if (!$dryrun);
    
      print " execute ''\n" if ($verbose);  
      $delete_query->execute()
	    or print "WARNING: Couldn't execute delete from labels: " . $db->errstr() . "\n" if (!$dryrun);
    }
    
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
  if (@{$testing})
  {
    if (!$append)
	{
	  print " prepare 'delete from testing where build_id=$id'\n" if ($verbose);
      my $delete_query = $db->prepare("delete from testing where build_id=$id")
        or die("Couldn't prepare query delete from testing: " . $db->errstr()) if (!$dryrun);
    
      print " execute ''\n" if ($verbose);  
      $delete_query->execute()
	    or print "WARNING: Couldn't execute delete from testing: " . $db->errstr() . "\n";
    }
    
    print " prepare 'insert into testing (build_id,name,type,target,started,finished,cases_total,cases_passed,cases_failed,cases_not_run,cases_not_available,pass_rate) values ($id,?,?,?,?,?,?,?,?,?,?,?)'\n" if ($verbose);
    my $query = $db->prepare_cached("insert into testing (build_id,name,type,target,started,finished,cases_total,cases_passed,cases_failed,cases_not_run,cases_not_available,pass_rate) values ($id,?,?,?,?,?,?,?,?,?,?,?)")
      or die("Couldn't prepare query insert into testing: " . $db->errstr()) if (!$dryrun);
      
    for my $entry (@{$testing})
    {
	  my $name = $entry->{name};
      my $type = $entry->{type};
      my $target = $entry->{target};
      my $started = $entry->{started};
      my $finished = $entry->{finished};
      my $cases_total = $entry->{cases_total};
      my $cases_passed = $entry->{cases_passed};
      my $cases_failed = $entry->{cases_failed};
      my $cases_not_run = $entry->{cases_not_run};
      my $cases_not_available = $entry->{cases_not_available};
      my $pass_rate = $entry->{pass_rate};
    
      print " execute '$name,$type,$target,$started,$finished,$cases_total,$cases_passed,$cases_failed,$cases_not_run,$cases_not_available,$pass_rate'\n" if ($verbose);
      $query->execute($name,$type,$target,$started,$finished,$cases_total,$cases_passed,$cases_failed,$cases_not_run,$cases_not_available,$pass_rate)
		    or print "WARNING: Couldn't execute insert into testing ($name,$type,$target,$started,$finished,$cases_total,$cases_passed,$cases_failed,$cases_not_run,$cases_not_available,$pass_rate): " . $db->errstr() . "\n" if (!$dryrun);
    }
  }
}
elsif ($delete)
{
  print "Removing build $delete\n";
  
  print " prepare 'delete from builds where id=$delete'\n" if ($verbose);
  my $delete_query = $db->prepare("delete from builds where id=$delete")
    or die("Couldn't prepare query delete from builds: " . $db->errstr()) if (!$dryrun);
  print " execute ''\n" if ($verbose);  
  $delete_query->execute()
    or print "WARNING: Couldn't execute delete from builds: " . $db->errstr() . "\n" if (!$dryrun);
  
  print " prepare 'delete from envinfo where build_id=$delete'\n" if ($verbose);
  $delete_query = $db->prepare("delete from envinfo where build_id=$delete")
    or die("Couldn't prepare query delete from envinfo: " . $db->errstr()) if (!$dryrun);
  print " execute ''\n" if ($verbose);  
  $delete_query->execute()
    or print "WARNING: Couldn't execute delete from envinfo: " . $db->errstr() . "\n" if (!$dryrun);
  
  print " prepare 'delete from failures where build_id=$delete'\n" if ($verbose);
  $delete_query = $db->prepare("delete from failures where build_id=$delete")
    or die("Couldn't prepare query delete from failures: " . $db->errstr()) if (!$dryrun);
  print " execute ''\n" if ($verbose);  
  $delete_query->execute()
    or print "WARNING: Couldn't execute delete from failures: " . $db->errstr() . "\n" if (!$dryrun);
    
  print " prepare 'delete from reports where build_id=$delete'\n" if ($verbose);
  $delete_query = $db->prepare("delete from reports where build_id=$delete")
    or die("Couldn't prepare query delete from reports: " . $db->errstr()) if (!$dryrun);
  print " execute ''\n" if ($verbose);  
  $delete_query->execute()
    or print "WARNING: Couldn't execute delete from reports: " . $db->errstr() . "\n" if (!$dryrun);

  print " prepare 'delete from content where build_id=$delete'\n" if ($verbose);
  $delete_query = $db->prepare("delete from content where build_id=$delete")
    or die("Couldn't prepare query delete from content: " . $db->errstr()) if (!$dryrun);
  print " execute ''\n" if ($verbose);  
  $delete_query->execute()
    or print "WARNING: Couldn't execute delete from content: " . $db->errstr() . "\n" if (!$dryrun);
    
  print " prepare 'delete from baselines where build_id=$delete'\n" if ($verbose);
  $delete_query = $db->prepare("delete from baselines where build_id=$delete")
    or die("Couldn't prepare query delete from baselines: " . $db->errstr()) if (!$dryrun);
  print " execute ''\n" if ($verbose);  
  $delete_query->execute()
    or print "WARNING: Couldn't execute delete from baselines: " . $db->errstr() . "\n" if (!$dryrun);

  print " prepare 'delete from labels where build_id=$delete'\n" if ($verbose);
  $delete_query = $db->prepare("delete from labels where build_id=$delete")
    or die("Couldn't prepare query delete from labels: " . $db->errstr()) if (!$dryrun);
  print " execute ''\n" if ($verbose);  
  $delete_query->execute()
    or print "WARNING: Couldn't execute delete from labels: " . $db->errstr() . "\n" if (!$dryrun);
    
  print " prepare 'delete from testing where build_id=$delete'\n" if ($verbose);
  $delete_query = $db->prepare("delete from testing where build_id=$delete")
    or die("Couldn't prepare query delete from testing: " . $db->errstr()) if (!$dryrun);
  print " execute ''\n" if ($verbose);  
  $delete_query->execute()
    or print "WARNING: Couldn't execute delete from testing: " . $db->errstr() . "\n" if (!$dryrun);
}
DisconnectFromDB();


