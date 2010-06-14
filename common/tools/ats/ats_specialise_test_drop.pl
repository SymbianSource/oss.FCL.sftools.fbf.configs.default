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
#	Brendan Donegan, brendand@symbian.org
#	Maciej Seroka, maciejs@symbian.org
#
# Description:
#   This is a tool for setting the name of an ATS test drop, and/or the build id
#   of the build being tested, and/or the name of the device on which
#   it is to be run, by modifying the XML test drop specificcation.

use strict;
use Getopt::Long;
use XML::Simple;
use File::Copy;
use Data::Dumper;

my $test_drop_name;	# Test drop name to be embedded in output XML
my $test_drop;
my $build_id;   # Build ID to be embedded in output XML 
my $host_name;  # Host name to be embedded in output XML
my $srcfile;	# Input test drop file, either .xml or .zip
my $destfile;	# Leafname for output file. Extension will be .xml or .zip as input.
				# If unspecified then the input file is updated. 
my $help;		# Do help?
my $srctype;	# Type of input file, .xml or .zip
my $dev_null = $^O =~ /^MSWin/ ? "nul" : "/dev/null";	# OS-dependent nul device.
my $zipper;		# Zip command, depending on whether zip or 7z is available.
my $unzipper;	# Unzip command, depending on whether [unzip or 7z is available.
my $xml_in;		# Name of the input xml file. Always 'test.xml' if extracted from zipped test drop.
my $temp_dest_name; # Leafname of temporary output file, if $destfile undefined.
my $publish;	# Publishing folder for ats reports.
my $bld_drive;  # Subst'd drive
my $test_target; # The target which the tests will be run on
my $image_path; # The path to the ROM image used for testing (not valid for WINSCW target)
my $ats_version; # The version of ATS used for running tests

sub usage($);
sub help();
sub usage_error();
sub silent_command($);

my %optmap = (  'test-drop-name' => \$test_drop_name,
                'host-name' => \$host_name,
                'build-id' => \$build_id,
                'src' => \$srcfile,
                'dest' => \$destfile,
                'publish' => \$publish,
                'bld-drive' => \$bld_drive,
				'test-target' => \$test_target,
                'image-path' => \$image_path,
				'ats-version' => \$ats_version,
                'help' => \$help);

GetOptions(\%optmap,
          'test-drop-name=s',
          'host-name=s',
          'build-id=s',
          'src=s',
          'dest=s',
          'publish=s', 
          'bld-drive=s',
		  'test-target=s',
          'image-path=s',
		  'ats-version=s',
          'help!') 
          or usage_error();

# Check if Tie::File module installed
eval("use Tie::File");
if ($@) { $publish = ''; $bld_drive = ''; };
		  
if ($help) {
	help();
}

# --src is mandatory.
usage_error(), unless (defined($srcfile));
if ($srcfile =~ /(\.xml$)/i) {
    $srctype = lc($1);
}
elsif ($srcfile =~ /(\.zip$)/i) {
    $srctype = lc($1);
}
else {
	# src file must have extension .xml or .zip
    usage_error();
}
die("File \"$srcfile\" does not exist"), unless -f $srcfile;

if (!$destfile) { # dest file unspecified. We will update the src file.
	$temp_dest_name = "ats_specialise_test_drop.out";
	$destfile = $temp_dest_name;
}
else {
    # If dest file has same extension as src then strip off, cos we'll append it anyway.
    $destfile =~ s/$srctype$//;
}

if ($srctype =~ /^\.zip$/) { # Test drop is zip. Must contain test.xml.
    $xml_in = "test.xml"; # This is our input XML.
    if ( -f $xml_in) {
		# Pre-emptively delete anystale xml file.
        unlink($xml_in) or die("Cannot delete stale $xml_in: $!\n");
    }
    if (!silent_command("zip -v")) { # See if we've got zip
        $zipper = "zip -m $destfile.zip test.xml";
        $unzipper = "unzip -o $srcfile $xml_in -d .";
    }
    elsif (!silent_command("7z -h")) { # Else see if we've got 7zip
        $zipper = "7z u $destfile.zip test.xml";
        $unzipper = "7z e $srcfile $xml_in";
    }
    else { # Got no archiver.
        die ("Need programs zip/unzip or 7z. Not found\n");
    }
    # Extract test.xml from zip.
    system("$unzipper > $dev_null") and die("Could not unzip \"$srcfile\"\n");
    die("Did not find $xml_in within \"$srcfile\"\n"), unless( -f "$xml_in");
    
}
else {
    $xml_in = $srcfile;
}

# --test-target is not mandatory.
if (!defined($test_target)) { # Set winscw by default
	$test_target = "winscw" 
}

# --ats-version is not mandatory.
if (!defined($ats_version)) { # Set ats3 by default
	$ats_version = "ats3" 
}

# Parse the input XML into hashref.
if (lc($ats_version) eq "ats4") {
	$test_drop = XMLin("./$xml_in", keeproot => 1,
        forcearray => [ 'metadata', 'meta', 'agents', 'agent', 'property', 'postActions', 'action', 'type', 'parameters', 'parameter', 'execution', 'initialization', 'task', 'alias' ],#
        keyattr => [] );
} else { # ats3 format
	$test_drop = XMLin("./$xml_in", keeproot => 1,
        forcearray => [ 'name', 'id', 'owner', 'priority', 'buildid', 'postAction', 'type', 'target', 'device', 'property', 'command', 'param', 'plan', 'session', 'set', 'alias' ],#
        keyattr => [] );
}
    
# Insert the specified test drop name, if any.
if (lc($ats_version) eq "ats4") {
	# Assumption: meta 'name' exists in test plan
	$test_drop->{'testrun'}->{'metadata'}->[0]->{'meta'}->[0] = { 'name' => "name", 'content' => $test_drop_name }, if $test_drop_name;
} else {
	$test_drop->{'test'}->{'name'}->[0] = $test_drop_name, if $test_drop_name;
}
# Insert the specified build id, if any.
if (lc($ats_version) eq "ats4") {
	if ($build_id) {
		my $meta_num = @{$test_drop->{'testrun'}->{'metadata'}->[0]->{'meta'}};
		$test_drop->{'testrun'}->{'metadata'}->[0]->{'meta'}->[$meta_num];
		$test_drop->{'testrun'}->{'metadata'}->[0]->{'meta'}->[$meta_num] = { 'name' => "diamonds-buildid", 'content' => $build_id };
		$test_drop->{'testrun'}->{'metadata'}->[0]->{'meta'}->[$meta_num+1];
		$test_drop->{'testrun'}->{'metadata'}->[0]->{'meta'}->[$meta_num+1] = { 'name' => "diamonds-testtype", 'content' => "Smoke" }; # Hard-coded Smoke
	}
} else {
	$test_drop->{'test'}->{'buildid'}->[0] = $build_id, if $build_id;
}
# Insert the path to the ROM image
if (lc($ats_version) eq "ats4") {
	if (($image_path) && (lc($test_target) ne "winscw")) {
		my $agent_alias = $test_drop->{'testrun'}->{'agents'}->[0]->{'agent'}->[0]->{'alias'};
		my $task_num = @{$test_drop->{'testrun'}->{'execution'}->[0]->{'initialization'}->[0]->{'task'}};
		$test_drop->{'testrun'}->{'execution'}->[0]->{'initialization'}->[0]->{'task'}->[$task_num];
		$test_drop->{'testrun'}->{'execution'}->[0]->{'initialization'}->[0]->{'task'}->[$task_num] = { 'agents' => $agent_alias };
		$test_drop->{'testrun'}->{'execution'}->[0]->{'initialization'}->[0]->{'task'}->[$task_num]->{'type'}->[0];
		$test_drop->{'testrun'}->{'execution'}->[0]->{'initialization'}->[0]->{'task'}->[$task_num]->{'type'}->[0] = "FlashTask";
		$test_drop->{'testrun'}->{'execution'}->[0]->{'initialization'}->[0]->{'task'}->[$task_num]->{'parameters'}->[0];
		$test_drop->{'testrun'}->{'execution'}->[0]->{'initialization'}->[0]->{'task'}->[$task_num]->{'parameters'}->[0]->{'parameter'}->[0];
		$test_drop->{'testrun'}->{'execution'}->[0]->{'initialization'}->[0]->{'task'}->[$task_num]->{'parameters'}->[0]->{'parameter'}->[0] = { 'name' => "image-1", 'value' => $image_path };
		$test_drop->{'testrun'}->{'execution'}->[0]->{'initialization'}->[0]->{'task'}->[$task_num]->{'parameters'}->[0]->{'parameter'}->[1];
		$test_drop->{'testrun'}->{'execution'}->[0]->{'initialization'}->[0]->{'task'}->[$task_num]->{'parameters'}->[0]->{'parameter'}->[1] = { 'name' => "timeout", 'value' => "1200" };
	}
} else {
	if (($image_path) && (lc($test_target) ne "winscw")) {
		my $device_alias;
		my $set_params;
		my $flash_params;
		foreach $set_params (@{$test_drop->{'test'}->{'plan'}->[0]->{'session'}->[0]->{'set'}}) {
			$device_alias = $set_params->{'target'}->[0]->{'device'}->[0]->{'alias'};
			$set_params->{'flash'}->[0];
			$flash_params = $set_params->{'flash'};
			$flash_params->[0] = { 'target-alias' => $device_alias, 'images' => $image_path };
		}
	}
}

# Insert the FileStoreAction parameter
if (lc($ats_version) eq "ats4") {
	if ($publish) {
		my $action_num = @{$test_drop->{'testrun'}->{'postActions'}->[0]->{'action'}};
		$test_drop->{'testrun'}->{'postActions'}->[0]->{'action'}->[$action_num];
		$test_drop->{'testrun'}->{'postActions'}->[0]->{'action'}->[$action_num]->{'type'}->[0];
		$test_drop->{'testrun'}->{'postActions'}->[0]->{'action'}->[$action_num]->{'type'}->[0] = "FileStoreAction";
		$test_drop->{'testrun'}->{'postActions'}->[0]->{'action'}->[$action_num]->{'parameters'}->[0];
		$test_drop->{'testrun'}->{'postActions'}->[0]->{'action'}->[$action_num]->{'parameters'}->[0]->{'parameter'}->[0];
		$test_drop->{'testrun'}->{'postActions'}->[0]->{'action'}->[$action_num]->{'parameters'}->[0]->{'parameter'}->[0] = { 'name' => "dst", 'value' => $publish };
	}
} else {
	if ($publish) {
		my $postaction_params = $test_drop->{'test'}->{'postAction'}->[0]->{'params'}->{'param'};
		$postaction_params->[1] = { 'name' => "to-folder", 'value' => $publish };
	}
}

# Also insert specified host name
if (lc($ats_version) eq "ats4") {
	if ($host_name) {
		# Assumption: 'agent' element with 'property' exists in test plan
		my $agent_name = uc($test_target) . " on " . $host_name;
		$test_drop->{'testrun'}->{'agents'}->[0]->{'agent'}->[0]->{'property'}->[0] = { 'name' => "name", 'value' => $agent_name };
	}
} else {
	if ($host_name) { 
		my $devices = $test_drop->{'test'}->{'target'}->[0]->{'device'};
		
		foreach my $device (@{$devices}) {
			my $device_properties = $device->{'property'};
			my $num_properties = @{$device_properties};
			
			$device_properties->[$num_properties] = { 'name' => "HOST", 'value' => "$host_name" };
			$device->{'property'} = $device_properties;
		}
	}
}

if ($srctype =~ /^\.xml$/i ) { # Input file was XML
    # Write new XML to dest file.
    open OUT,">$destfile.xml" or die("Cannot open file \"$destfile.xml\" for writing. $!\n");
    print OUT XMLout($test_drop, keeproot => 1);
    close OUT;    
}
else { #Input file was a zip.
    # Write new XML to test.xml
    open OUT,">test.xml" or die("Cannot open file \"test.xml\" for writing. $!\n");
    print OUT XMLout($test_drop, keeproot => 1);
    close OUT;
	if (($publish) && (lc($ats_version) eq "ats3")) { #Move <type>FileStoreAction</type> above <params>
		my @lines;
		tie @lines, 'Tie::File', "test.xml" or die("Cannot tie file \"test.xml\". $!\n");
		my $current_line = 0;
		for (@lines) {
			if (/FileStoreAction/) {
				my $line_to_move = @lines[$current_line];
				splice @lines, $current_line, 1;
				splice @lines, $current_line-4, 0, $line_to_move;
				last;
			}
		  $current_line++;
	    }
	untie @lines;
	}
	if ($bld_drive) { #Replace D:\ATS\winscw_smoketest path with subst'd drive
		my @LINE;
		tie @LINE, 'Tie::File', "test.xml" or die("Cannot tie file \"test.xml\". $!\n");
		for (@LINE) {
			s/D:\\ATS\\winscw_smoketest/$bld_drive/g;
		}
	untie @LINE;	
	}
    if ( -f "$destfile.zip") {
        unlink("$destfile.zip") or die("Could not delete \"$destfile.zip\": $!\n");
    } 
    copy("$srcfile","$destfile.zip") or die("Could not copy \"$srcfile\" -> \"$destfile.zip\": $!\n");
    system("$zipper > $dev_null") and die("Could not zip \"test.xml\" into \"$destfile\"\n");    
}

if ($temp_dest_name) { # We are to update the src file. dest file is a temporary.
   # Delete src file 
   unlink($srcfile) or die("Cannot delete file \"$srcfile\" to replace: $!\n");
   # Replace with temporary.
   move("$destfile$srctype","$srcfile") or die("Could not move \"$destfile$srctype\" -> \"$srcfile\": $!\n");
}

print "OK\n";
exit 0;

sub usage($)
{
    my $error = shift;
    my $fh = $error == 0 ? *STDOUT : *STDERR;
    print $fh "ats_specialise_test_drop.pl\n" .
            "Specify the name, build id and target device in an ATS XML test drop\n" .
            "synopsis:\n" .
            "  ats_specialise_test_drop.pl --help\n" .
            "  ats_specialise_test_drop.pl [--test-drop-name=TESTNAME] [--build-id=BUILDID] [--host-name=HOSTNAME] [--dest=FILE] --src=FILE [--publish=DIR] [--bld-drive=DRIVE] [--test-target=TARGET] [--image-path=PATH] [--ats-version=VERSION]\n" .
            "options:\n" .
            "  --help                        Display this help and exit\n" .
            "  --test-drop-name=TESTNAME     TESTNAME is the desired name of the test drop. If not specified then the test drop name is not modified.\n" .
            "  --build-id=BUILDID            BUILDID is id of the build being tested. If not specified then the build id is not modified.\n" .            
            "  --host-name=HOSTNAME          HOSTNAME  is the name of the ATS worker on which the test should be run. " .
            "If not specified then the test device name is not modified.\n" .
            "  --src=INFILE                  INFILE is the file containing the test drop XML to be modified, or else a zip file " .
            "                                containing the test drop XML in the file 'test.xml'. INFILE must have extension .xml or.zip\n" .
            "  --dest=OUTFILE                The modified output will be written to the file OUTFILE.EXT " .
            "where EXT is same extention, .xml or .zip, found on INFILE. " .
            "If OUTFILE is not specified then INFILE is modified\n" . 
            "  --publish=DIR                 DIR is the publishing folder for ats reports (optional).\n" .
            "  --bld-drive=DRIVE             DRIVE is the subst'd drive on which the build is stored (optional).\n" .
			"  --test-target=TARGET          TARGET is the target which the tests will be run on. If not specified WINSCW will be set by default\n" .
            "  --image-path=PATH             PATH is the path to the ROM image used for testing (not valid for WINSCW target).\n" .
            "  --ats-version=VERSION         VERSION is the version of ATS which will be used to run the tests. If not specified ATS3 will be set by default.\n";
			
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

sub silent_command($)
{
    my $cmd = shift;
    system("$cmd 1> $dev_null 2>&1");
    return $? >> 8;
}
