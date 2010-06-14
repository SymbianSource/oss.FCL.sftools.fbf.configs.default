#!/usr/bin/perl

# Copyright (c) 2009 Symbian Foundation Ltd
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Symbian Foundation Ltd - initial contribution.
#   Mike Kinghan, mikek@symbian.org
#
# Contributors:
#   Maciej Seroka, maciejs@symbian.org
#
# Description:
#   This is a tool for sending an ATS testdrop to an ATS server.

use strict;
use Getopt::Long;
use File::Copy;
use File::Spec;

my $username = "admin"; # ATS server user name
my $password = "admin"; # ATS server password
my $host;   # ATS server hostname
my $schedule;   # Schedule string for test drop, if any
my $host_drop_path; # Directory path on the ATS server where the test drop will be placed.
                    # This must be physically the same path as $local_drop_path, via drive mapping. 
my $local_drop_path; # Local directory path where the test drop will be placed.
                    # This must be physically the same path as $host_drop_path, via drive mapping.
my $local_test_pkg; # Local pathname of the test drop (zip or xml file). The test drop will be
                    # copied from here to $local_drop_path to make it accessible on the server,
                    # unless $local_test_pkg begins with "...". This indicates the test drop is already
                    # present in $local_drop_path and does need to be copied there. The remainder
                    # following "..." is the filename of the test drop.    
my $help;
my $dev_null = $^O =~ /^MSWin/ ? "nul" : "/dev/null";
my $ats_version; # The version of ATS used for running tests
my $ats_worker_path;

sub usage($);
sub help();
sub usage_error();

my %optmap = (  'host' => \$host,
			    'local-test-pkg' => \$local_test_pkg,
				'host-drop-path' => \$host_drop_path,
				'local-drop-path' =>\$local_drop_path,
				'username' => \$username,
				'password' => \$password,
				'schedule' => \$schedule,
				'ats-version' => \$ats_version,
				'ats-worker-path' => \$ats_worker_path,
                'help' => \$help);

GetOptions(\%optmap,
          'host=s',
          'local-test-pkg=s',
          'host-drop-path=s',
          'local-drop-path=s',
          'username=s',
          'password=s',
          'schedule=s',
		  'ats-version=s',
		  'ats-worker-path=s',
          'help!') 
          or usage_error();

if ($help) {
	help();
}

usage_error(), unless (defined($host) && defined($local_test_pkg) && defined($local_drop_path) && defined($host_drop_path));

# --ats-version is not mandatory.
if (!defined($ats_version)) { # Set ats3 by default
	$ats_version = "ats3" 
}

# --ats-worker-path is not mandatory.
if (!defined($ats_worker_path)) { # Set ats3 by default
	$ats_worker_path = "c:\\apps\\$ats_version\\bin" 
}

my $curl_version = `curl --version 1> $dev_null 2>&1`;
die("Need program \"curl\". Not found"), if ($?);
die("Test drop path \"$local_drop_path\" not found"), unless ( -d "$local_drop_path");

if (lc($ats_version) eq "ats4") {
	$host = "http:\/\/" . $host . ":7780\/ServerService", unless ($host =~ /:\d+$/);
} else {
	$host .= ":8080", unless ($host =~ /:\d+$/);
}

my ($vol,$dir,$pkg);
my $local_test_drop;
my $host_test_drop;

if ($local_test_pkg =~ /^\.\.\.(.+)/ ) { # Local test package name begins with "...". No need to copy. 
    $pkg = $1; # Get the filename of the test package.
    # Compose the full name of the test drop within the local drop path.
    $local_test_drop = File::Spec->catfile($local_drop_path,$pkg);    
    die("Test package file \"$local_test_drop\" not found"), unless ( -f "$local_test_drop");        
}
else { # Local test package must be copied to local drop path.
    die("Test package file \"$local_test_pkg\" not found"), unless ( -f "$local_test_pkg");
    # Compose the full name of the test drop within the local drop path.    
    ($vol,$dir,$pkg) = File::Spec->splitpath($local_test_pkg);
    $local_test_drop = File::Spec->catfile($local_drop_path,$pkg);
    if ( -f "$local_test_drop") {
        # A state test drop of the same name exists. Delete.
        unlink($local_test_drop) or die("Can't delete stale test drop \"$local_test_drop\". $!");
        print("A stale test drop \"$local_test_drop\" existed. Deleted\n");
    }
    # Copy the test drop to the local drop path.
    copy("$local_test_pkg","$local_test_drop")
        or die("Cannot copy \"$local_test_pkg\" -> \"$local_test_drop\". $!");
}
# Compose the full name of the test drop within the host drop path.
$host_test_drop = File::Spec->catfile($host_drop_path,$pkg);
# Compose URL to notify server of test drop, requesting run.
my $url;
if (lc($ats_version) eq "ats4") {
	$url ="-Xmx512M -cp  $ats_worker_path\/..\/plugins\/*;$ats_worker_path\/..\/lib\/util.jar com.nokia.ats.util.server.CommandLineClient -url $host -path $local_test_pkg";
} else {
	$url ="http://$host/ats3/XTestRunExecute.do?username=$username&password=$password&testrunpath=$host_test_drop";
	$url .= "&schedule=$schedule", if (defined($schedule));
}
# Post URL to server with cURL.
my $curl_cmd;
if (lc($ats_version) eq "ats4") {
	$curl_cmd = "java $url";
} else {
	$curl_cmd = "curl \"$url\"";
}
my $curl_response = `$curl_cmd 2>&1`;
#die("\nTest drop failed: $!\n"), if ($?);
die("\nTest drop failed: $curl_response\n"), if ($?);
if (lc($ats_version) eq "ats4") {
	print "$curl_response\n";
} else { 
	if ($curl_response =~ /(TEST_RUN_REF_ID=\d+)/) {
		#extract test run id from cURL response.
		print "$1\n";
	}
	else {
		print "$curl_response\n";
		die("Test drop failed\n");
	} 
}
print("Test drop done\n");

exit 0;

sub usage($)
{
    my $error = shift;
    my $fh = $error == 0 ? *STDOUT : *STDERR;
    print $fh "ats_testdrop.pl\n" .
            "Send a test drop to an ATS server for execution\n" .
            "usage:\n" .
            "  ats_testdrop.pl --help\n" .
            "  ats_testdrop.pl --host=HOSTNAME --local-test-pkg=PKGFILE --local-drop-path=LOCALPATH " .
                " --host-drop-path=HOSTPATH " .
                "[--username=ATS3USERNAME] [--password=ATS3PASSWORD] [--schedule=DD.MM.YYYY-HH:MM] [--ats-version=VERSION] [--ats-worker-path=ATSPATH]\n" .
            "options:\n" .
            "  --help                        Display this help and exit\n" .
            "  --host=HOSTAME                HOSTNAME is ATS server\n" .
            "  --local-test-pkg=PKGFILE      PKGFILE is the test package.\n" .
            "                                If PKGFILE begins \"...\" a filename in LOCALPATH is assumed\n" .
            "  --local-drop-path=LOCALPATH   Path to local directory where PKGFILE will be dropped\n" .            
            "  --host-drop-path=HOSTPATH     Host directory that is mapped to LOCALPATH.\n" .
            "                                Must agree with the properties of the registered device that the test package nominates\n" .
            "  --username=ATS3USERNAME       ATS3 user to whome the test will belong. Default=admin\n" .
            "  --password=ATS3PASSWORD       Password of ATS3 user. Default=admin\n" .
            "  --schedule=DD.MM.YYYY-HH:MM   Date-time at which test is to run. Default=as soon as possible\n" .
            "  --ats-version=VERSION         Version of ATS which will be used to run the tests. If not specified ATS3 will be set by default.\n" .
            "  --ats-worker-path=ATSPATH     Path to the ATS bin directory. If not specified c:\\apps\\\$ats_version\\bin will set by default.\n";
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
   