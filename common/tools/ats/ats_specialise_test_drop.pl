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

sub usage($);
sub help();
sub usage_error();
sub silent_command($);

my %optmap = (  'test-drop-name' => \$test_drop_name,
                'host-name' => \$host_name,
                'build-id' => \$build_id,
                'src' => \$srcfile,
                'dest' => \$destfile,
                'help' => \$help,
                'publish' => \$publish,
                'bld-drive' => \$bld_drive,
                'test-target' => \$test_target,
                'image-path' => \$image_path);

GetOptions(\%optmap,
          'test-drop-name=s',
          'host-name=s',
          'build-id=s',
          'src=s',
          'dest=s',
          'help!',
          'publish=s', 
          'bld-drive=s',
          'test-target=s',
          'image-path=s') 
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

# Parse the input XML into hashref.
my $test_drop = XMLin("./$xml_in", keeproot => 1,
    forcearray => [ 'name', 'id', 'owner', 'priority', 'buildid', 'postAction', 'type', 'target', 'device', 'property', 'command', 'param', 'plan'],#
        keyattr => [] );
    
# Insert the specified test drop name, if any.
$test_drop->{'test'}->{'name'}->[0] = $test_drop_name, if $test_drop_name;
# Insert the specified build id, if any.
$test_drop->{'test'}->{'buildid'}->[0] = $build_id, if $build_id;
# Insert the path to the ROM image
if ($image_path) {
	$test_drop->{'test'}->{'plan'}->[0]->{'flash'}->[0];
	my $flash_params = $test_drop->{'test'}->{'plan'}->[0]->{'flash'};
    $flash_params->[0] = { 'target-alias' => $test_target, 'images' => $image_path };
}

# Insert the FileStoreAction parameter
my $postaction_params = $test_drop->{'test'}->{'postAction'}->[0]->{'params'}->{'param'}, if $publish;
$postaction_params->[1] = { 'name' => "to-folder", 'value' => $publish }, if $publish;

if ($host_name) { # Also insert specified host name
	my $devices = $test_drop->{'test'}->{'target'}->[0]->{'device'};
    
    foreach my $device (@{$devices}) {
        my $device_properties = $device->{'property'};
        my $num_properties = @{$device_properties};
        
        $device_properties->[$num_properties] = { 'name' => "HOST", 'value' => "$host_name" };
        $device->{'property'} = $device_properties;
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
	if ($publish) { #Move <type>FileStoreAction</type> above <params>
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
            "  ats_specialise_test_drop.pl [--test-drop-name=TESTNAME] [--build-id=BUILDID] [--host-name=HOSTNAME] [--dest=FILE] --src=FILE \n" .
            "options:\n" .
            "  --help                        Display this help and exit\n" .
            "  --test-drop-name=TESTNAME     TESTNAME is the desired name of the test drop. If not specified then the test drop name is not modified.\n" .
            "  --build-id=BUILDID            BUILDID is id of the build being tested. If not specified then the build id is not modified.\n" .            
            "  --host-name=HOSTNAME        HOSTNAME  is the name of the ATS worker on which the test should be run. " .
            "If not specified then the test device name is not modified.\n" .
            "  --src=INFILE                  INFILE is the file containing the test drop XML to be modified, or else a zip file " .
            "                                containing the test drop XML in the file 'test.xml'. INFILE must have extension .xml or.zip\n" .
            "  --dest=OUTFILE                The modified output will be written to the file OUTFILE.EXT " .
            "where EXT is same extention, .xml or .zip, found on INFILE. " .
            "If OUTFILE is not specified then INFILE is modified\n";              
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
