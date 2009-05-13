#!/usr/bin/perl

use strict;
use Getopt::Long;

my $link;
my $target;
my $help;
my $force = 0;

sub usage($);
sub help();
sub usage_error();

my %optmap = (  'link' => \$link,
			    'target' => \$target,
			    'force' => \$force, 
                'help' => \$help);

GetOptions(\%optmap,
          'link=s',
          'target=s',
          'force!',
          'help!') 
          or usage_error();

if ($help) {
	help();
}

usage_error(), unless (defined($link) && defined($target));

my $junction_help = `junction /?`;
die("Need command \"junction\". Not found\n"), if ($junction_help =~ /is not recognised/);
die("Directory \"$target\" not found\n"), unless -d "$target";

if ( -e "$link") {
    print("\"$link\" already exists. ");
    if (!$force) {
        my $choice;
        while($choice ne 'y' and $choice ne 'n') {
            print "Delete? (y/n)? ";
            $choice = <STDIN>;
            chomp($choice);
            $choice = lc($choice);
        }
        $force = $choice eq 'y';
    }    
    if ($force) {
        system("junction -d \"$link\" > nul");
        if ($?) {
            die("Cannot delete \"$link\": $!\n");
        }
        else {
            print "Deleted \"$link\"\n";
        }
    }
} 

system("junction \"$link\" \"$target\" > nul");
if ($?) {
    die("Cannot cteate junction \"$link\" -> \"$target\": $!\n");
}
else {
    print("Created junction \"$link\" -> \"$target\"\n");
}
exit 0;

sub usage($)
{
    my $error = shift;
    my $fh = $error == 0 ? *STDOUT : *STDERR;
    print $fh "make_junction.pl\n" .
            "Create a Windows junction (a.k.a symbolic link)\n" .
            "usage:\n" .
            "  make_junction.pl --help\n" .
            "  make_junction.pl --link=LINKDIR --target=TARGDIR\n " .
            "options:\n" .
            "  --help                        Display this help and exit\n" .
            "  --link=LINKDIR                LINKDIR specifies the junction to be created. Last component is the junction. The rest must exist\n" .
            "  --target=TARGDIR              TARGDIR is directory to which the junction will point.\n";
    exit $error;            
}

sub help()
{
    usage(0);
}

sub usage_error()
{
    usage(1);
}             

# EOF
   