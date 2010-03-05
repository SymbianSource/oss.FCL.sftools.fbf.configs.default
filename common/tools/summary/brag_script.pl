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
# Arnaud Lenoir
# Dario Sestito
# Description:
# Generate build summary with BRAG status

use Getopt::Long;
use strict;

# Data used for the script
my $buildid = '';
my $basedir = '';
my $help = 0;
GetOptions((
	'buildid=s' => \$buildid,
	'basedir=s' => \$basedir,
	'help!' => \$help
));

#---------------------------------------------------------------------------------------
# Files locations

# Command to build a package build
# F:\Dev\mercurial_local\bootstrap>perl build_package.pl --subproj=3k/mw/serviceapi

#brag_script.pl location
#F:\Dev\mercurial_local\pkgbuild\common\tools\summary
# my package log files directory is: F:\fbf_job\serviceapi_3k.T014\output\logs
# my platform log files directory is: F:\Dev\fbf_job\platform_MCL.PDK-101\output\logs

# F:\fbf_job\serviceapi_3k.T014\output\logs
# console_bootstrap_d+.txt	empty file
# console_sfbuildall_d+.txt	empty file
# serviceapi_3k.T014_ant_env.log

# F:\fbf_project\serviceapi\sf-config
# console_sfbuildall_32428.txt
# console_sfprep_32428.txt
# console_sfsummary_32428.txt

# F:\Dev\fbf_job\platform_MCL.PDK-101\output\logs
# platform_MCL.PDK-101_ant_env.log
# in _ant_env.log
# Do a search for env.ANT_CMD_LINE_ARGS to find out the type of build done as armv5, winscw, ...

#---------------------------------------------------------------------------------------


# If no parameters entered or help selected, display help
$help = 1 if (!$buildid or !$basedir);

if ($help)
{
	print "Generate build summary with BRAG status\n";
	print "Usage: perl brag_script.pl --buildid=ID --basedir=DIR\n";
	print "Typical command line from script location: <perl brag_script.pl --buildid=serviceapi_3k.T014 --basedir=F:\\fbf_job>\n";
	exit(0);
}
# End section related to help

# Determine build number
$buildid =~ /^([^_]*)_([^.]*)\./;
my $project = $1;
my $codeline = $2;

# Define variable logdir
my $logdir = "$basedir\\$buildid\\output\\logs";

# General data information
print "\$builid = $buildid\n";
print "\$basedir = $basedir\n";
print "\$logdir = $logdir\n";
print "\$project = $project\n";
print "\$codeline = $codeline\n";

open(BUILDSUMMARYHTML, ">$logdir\\build_summary.html");	# !!!!! First time we are accessing the file build_summary.html, therefore create it or replace it, AFTR THAT WE NEED TO APPEND IT ONLY!!!!!

print BUILDSUMMARYHTML "#********** build_summary.html **********#<br/><br/><br/>";

close(BUILDSUMMARYHTML); # Close file build_summary.html

# *****************************************************************************
# Construction "GENERAL STATUS" page in build_summary.html
# *****************************************************************************

# Fill html file with buildir data
open(BUILDSUMMARYHTML, ">>$logdir\\build_summary.html"); # Open build_summary.html in APPEND MODE!

print BUILDSUMMARYHTML "<br/>#************************************************#<br/><br/>";
print BUILDSUMMARYHTML "<br/>#********** GENERAL STATUS starts here **********#<br/><br/>";
print BUILDSUMMARYHTML "buildid_val=$buildid<br/>";
print BUILDSUMMARYHTML "basedir_val=$basedir<br/>";
print BUILDSUMMARYHTML "logdir_val=$logdir<br/>";
print BUILDSUMMARYHTML "project_val=$project<br/>";
print BUILDSUMMARYHTML "codeline_val=$codeline<br/>";

# Calculate the number of files in the directory
opendir(DIR, $logdir);
my @dir_content = readdir(DIR);
close(DIR);
my $nbfilesinlogdir = scalar(@dir_content);
print BUILDSUMMARYHTML "nbfilesinlogdir_val=$nbfilesinlogdir<br/>";

# Define what we tried to build (target, winscw, armv5, TOOLS2, platform, package, ...)
# Information in the file buildid_ant_env.log that can be found in the log directory

# F:\Dev\fbf_job\platform_MCL.PDK-101\output\logs
# platform_MCL.PDK-101_ant_env.log
# in _ant_env.log
# Do a search for env.ANT_CMD_LINE_ARGS to find out the type of build done as armv5, winscw, ...

# Declare any variable that will need to be used globaly otherwise can't be used only between the curly brackets!
my $antenvlog = "";

#print "$logdir\\$buildid\_ant\_env\.log\n"; # Test to find out that _ and . need backslash up front to work
my $antenvlogfile = "$logdir\\$buildid\_ant\_env\.log";
print "$antenvlogfile\n";
open (ANTENVLOG, "<$antenvlogfile");
{
	local $/=undef; # Technic used to get a file in one unique string accessible via a variable
	$antenvlog = <ANTENVLOG>;	
}
close(ANTENVLOG); # Close file $buildid_ant_env.log

# List of data to extract from the file $buildid_ant_env.log
#Fri Aug 14 17:04:54 BST 2009 on the second line of the file
#base_release.getenv_options=-I tools
#bom.log=\\\\v800008\\Builds01\\SF_builds\\platform\\builds\\MCL\\platform_MCL.PDK-101/logs/platform_MCL.PDK-101_bom.xml
#build.family=platform or package		->DONE!!
#build.id=platform_MCL.PDK-101			->1st thing done in this script!!
#build.name=platform or serviceapi		->DONE!!
#build.number=PDK-101					->DONE!!
#build.version=MCL.PDK-101				->DONE!!
#core.build.version=MCL					->DONE!!
#diamonds.host=v800002.ad-sfpd.intra	->DONE!!
#env.ANT_CMD_LINE_ARGS=-Dhelium.dir "C\:\\Symbian\\Tools\\PDT_1.0\\helium" sf-build -Dbuild.drive m\: -Dsf.spec.job.codeline MCL -Dsf.spec.job.number PDK-101 -Dsf.project.location f\:\\\\Dev\\maintools\\build\\config\\foundation -Dsf.spec.job.rootdir f\:\\\\Dev\\fbf_job -Dsf.spec.sbs.config winscw_udeb.whatlog -Dsf.spec.test.sendpkg.enable false
#env.COMPUTERNAME=UK-ARNAUDL			->DONE!!
#env.USERNAME=arnaudl					->DONE!!
#publish.dir=\\\\v800008\\Builds01\\SF_builds\\platform\\builds\\MCL\\platform_MCL.PDK-101		->DONE!!
#sf.spec.sbs.config=winscw				->DONE!!
#sbs.config=armv5						->DONE!!
#sf.spec.baseline.enable=true			->DONE!!
#sf.spec.baseline.getenv_options=-I tools
#sf.spec.bccheck.baseline.s60.version=5.1	->DONE!!
#sf.spec.bccheck.enable=false			->DONE!!
#sf.spec.build.target=all				->DONE!!
#sf.spec.build.testcode.enable=false	->DONE!!
#sf.spec.job.codeline=MCL				->Information already available under another name
#sf.spec.job.number=PDK-101				->Information already available under another name
#sf.spec.sourcesync.enable=true			->DONE!!
#sf.spec.test.epocroot=D\:\\ATS3\\winscw_smoketest
#sf.spec.test.host.name=v800005			->DONE!!
#sf.spec.toolsbaseline.getenv_options=-i emu
#user.country=GB						->Any use????


# Data extraction
# Type of Line to read is: "env.COMPUTERNAME=UK-ARNAUDL" with a end of line \\n character

# env.COMPUTERNAME=UK-ARNAUDL
if ($antenvlog =~ /env\.COMPUTERNAME=([^=]*)\n/)
{
	print "Computer name is: $1\n";
	print BUILDSUMMARYHTML "computername_val=$1<br/>";
}
else
{
	print "Computer name is unknown\n";
	print BUILDSUMMARYHTML "computername_val=?<br/>";
}

#env.USERNAME=arnaudl
if ($antenvlog =~ /env\.USERNAME=([^=]*)\n/)
{
	print "User name is: $1\n";
	print BUILDSUMMARYHTML "username_val=$1<br/>";
}
else
{
	print "user name is unknown\n";
	print BUILDSUMMARYHTML "username_val=?<br/>";
}

#build.family=platform or package
if ($antenvlog =~ /build\.family=([^=]*)\n/)
{
	print "Build family is: $1\n";
	print BUILDSUMMARYHTML "buildfamily_val=$1<br/>";
}
else
{
	print "Build family is unknow\n";
	print BUILDSUMMARYHTML "buildfamily_val=?<br/>";
}

#build.name=platform or serviceapi
if ($antenvlog =~ /build\.name=([^=]*)\n/)
{
	print "Build name is: $1\n";
	print BUILDSUMMARYHTML "buildname_val=$1<br/>";
}
else
{
	print "Build name is unknow\n";
	print BUILDSUMMARYHTML "buildname_val=?<br/>";
}

#diamonds.host=v800002.ad-sfpd.intra - Define the server used
if ($antenvlog =~ /diamonds\.host=([^=]*)\n/)
{
	print "Build server used is: $1\n";
	print BUILDSUMMARYHTML "buildserver_val=$1<br/>";
}
else
{
	print "Build server used is unknown\n";
	print BUILDSUMMARYHTML "buildserver_val=?<br/>";
}

#build.version=MCL.PDK-101
if ($antenvlog =~ /build\.version=([^=]*)\n/)
{
	print "Build version is: $1\n";
	print BUILDSUMMARYHTML "buildversion_val=$1<br/>";
}
else
{
	print "Build version is unknown\n";
	print BUILDSUMMARYHTML "buildversion_val=?<br/>";
}

#build.number=PDK-101
if ($antenvlog =~ /build\.number=([^=]*)\n/)
{
	print "Build number is: $1\n";
	print BUILDSUMMARYHTML "buildnumber_val=$1<br/>";
}
else
{
	print "Build number is unknown\n";
	print BUILDSUMMARYHTML "buildnumber_val=?<br/>";
}

#core.build.version=MCL
if ($antenvlog =~ /core\.build\.version=([^=]*)\n/)
{
	print "Core build version is: $1\n";
	print BUILDSUMMARYHTML "corebuildversion_val=$1<br/>";
}
else
{
	print "Core build version is unknown\n";
	print BUILDSUMMARYHTML "corebuildversion_val=?<br/>";
}

#publish.dir=\\\\v800008\\Builds01\\SF_builds\\platform\\builds\\MCL\\platform_MCL.PDK-101
if ($antenvlog =~ /publish\.dir=([^=]*)\n/)
{
	print "Publish directory is: $1\n";
	print BUILDSUMMARYHTML "publishdir_val=$1<br/>";
}
else
{
	print "Publish directory is unknown\n";
	print BUILDSUMMARYHTML "publishdir_val=?<br/>";
}

#sf.spec.baseline.enable=true
if ($antenvlog =~ /sf\.spec\.baseline\.enable=([^=]*)\n/)
{
	print "Baseline retrieval enabled?:: $1\n";
	print BUILDSUMMARYHTML "baselineretrievalen_val=$1<br/>";
}
else
{	
	print "Baseline retrieval status is unknown\n";
	print BUILDSUMMARYHTML "baselineretrievalen_val=?<br/>";
}

#sf.spec.sourcesync.enable=true
if ($antenvlog =~ /sf\.spec\.sourcesync\.enable=([^=]*)\n/)
{
	print "Source code sync enabled?: $1\n";
	print BUILDSUMMARYHTML "sourcecodesyncen_val=$1<br/>";
}
else
{	
	print "Source code sync status is unknown\n";
	print BUILDSUMMARYHTML "sourcecodesyncen_val=?<br/>";
}

#sf.spec.build.testcode.enable=false
# Smoke tests????? SALT tests????
if ($antenvlog =~ /sf\.spec\.build\.testcode\.enable=([^=]*)\n/)
{
	print "Tests execution enabled?: $1\n";
	print BUILDSUMMARYHTML "testexecutionen_val=$1<br/>";
}
else
{	
	print "Tests execution status is unknown\n";
	print BUILDSUMMARYHTML "testexecutionen_val=?<br/>";
}

#sf.spec.test.host.name=v800005
if ($antenvlog =~ /sf\.spec\.test\.host\.name=([^=]*)\n/)
{
	print "Tests host server is: $1\n";
	print BUILDSUMMARYHTML "testhostserver_val=$1<br/>";
}
else
{	
	print "Tests host server is unknown\n";
	print BUILDSUMMARYHTML "testhostserver_val=?<br/>";
}

#sf.spec.bccheck.enable=false
if ($antenvlog =~ /sf\.spec\.bccheck\.enable=([^=]*)\n/)
{
	print "BCC check enabled?: $1\n";
	print BUILDSUMMARYHTML "bccchecken_val=$1<br/>";
}
else
{	
	print "BCC check status is unknown\n";
	print BUILDSUMMARYHTML "bccchecken_val=?<br/>";
}

#sf.spec.bccheck.baseline.s60.version=5.1
if ($antenvlog =~ /sf\.spec\.bccheck\.baseline\.s60\.version=([^=]*)\n/)
{
	print "BCC check S60 baseline version used is: $1\n";
	print BUILDSUMMARYHTML "bccchecks60baselinever_val=$1<br/>";
}
else
{	
	print "BCC check S60 baseline version used is unknown\n";
	print BUILDSUMMARYHTML "bccchecks60baselinever_val=?<br/>";
}


#sbs.config=armv5 possible values are armv5 (default) and winscw
if ($antenvlog =~ /sbs\.config=([^=]*)\n/)
{
	print "SBS config is: $1\n";
	print BUILDSUMMARYHTML "sbsconfig_val=$1<br/>";
}
else
{	
	print "SBS config is unknown\n";
	print BUILDSUMMARYHTML "sbsconfig_val=?<br/>";
}

#sf.spec.sbs.config=winscw
if ($antenvlog =~ /sf\.spec\.sbs\.config=([^=]*)\n/)
{
	print "Specific SBS config is: $1\n";
	print BUILDSUMMARYHTML "specsbsconfig_val=$1<br/>";
}
else
{	
	print "Specific SBS config is unknown\n";
	print BUILDSUMMARYHTML "specsbsconfig_val=?<br/>";
}

#sf.spec.build.target=all
if ($antenvlog =~ /sf\.spec\.build\.target=([^=]*)\n/)
{
	print "specific build target is: $1\n";
	print BUILDSUMMARYHTML "specificbuildtarget_val=$1<br/>";
}
else
{	
	print "specific build target is: unknown\n";
	print BUILDSUMMARYHTML "specificbuildtarget_val=?<br/>";
}

# Extract data to define the type build done
# Find out what is expected to be done and what has been actually done.
# name="WINS" abldTarget="wins" description="MSVC Compiler"
# name="WINS_REL" abldTarget="wins urel" description="MSVC Compiler"
# name="WINS_DEB" abldTarget="wins udeb" description="MSVC Compiler"
# name="WINSCW" abldTarget="winscw" description="CodeWarrior Compiler"
# name="WINSCW_REL" abldTarget="winscw urel" description="CodeWarrior Compiler"
# name="WINSCW_DEB" abldTarget="winscw udeb" description="CodeWarrior Compiler"
# name="TOOLS" abldTarget="tools" description="MSVC Compiler for Tools"
# name="TOOLS_REL" abldTarget="tools rel" description="MSVC Compiler for Tools Release mode only"
# name="TOOLS2" abldTarget="tools2" description="MinGW GCC Compiler for Tools"
# name="TOOLS2_REL" abldTarget="tools2 rel" description="MinGW GCC Compiler for Tools Release mode only"
# name="ARMV5" abldTarget="armv5" description="RVCT Compiler"
# name="ARMV5_REL" abldTarget="armv5 urel" description="RVCT Compiler"
# name="ARMV5_DEB" abldTarget="armv5 udeb" description="RVCT Compiler"


#-------------
# End data extraction from the file $buildid_ant_env.log
#-------------

print BUILDSUMMARYHTML "<br/>#********** GENERAL STATUS ends here **********#<br/><br/>";

close(BUILDSUMMARYHTML); # Close file build_summary.html

# *****************************************************************************
# Construction "SUMMARISE" page in build_summary.html
# *****************************************************************************

# Build info - Describe the completion of the diffrent steps in the build process

# Fill html file with buildir data
open(BUILDSUMMARYHTML, ">>$logdir\\build_summary.html"); # Open build_summary.html in APPEND MODE!

# Indicates beginning of the SUMMARISE section in the file
print BUILDSUMMARYHTML "<br/>#*******************************************#<br/><br/>";
print BUILDSUMMARYHTML "<br/>#********** SUMMARISE starts here **********#<br/><br/>";

#{ # Used for local $/=undef
#local $/=undef; # indicates that the file will be put in one variable as one string
#
## Find the string "BUILD SUCCESSFUL" in the file console_bootstrap_\d+\.txt
## 1st open file
#open (CONSOLEBOOTSTRAP, "<$logdir\\console_bootstrap_\d+\.txt");
#
#my $searchconsolebootstrap= <CONSOLEBOOTSTRAP>; # Copy the full file in one string because of /$=undef
#
#if ($searchconsolebootstrap =~ /BUILD SUCCESSFUL/){
#	print "Found string BUILD SUCCESSFUL\n";	
#	print BUILDSUMMARYHTML "BuildSuccessful_val=Yes<br/>";
#	}
#	else {
#		print "Can't Find string BUILD SUCCESSFUL\n";
#		print BUILDSUMMARYHTML "BuildSuccessful_val=No<br/>";
#		}
#
#close (CONSOLEBOOTSTRAP); # Close file console_bootstrap_\d+\.txt
## End - Find the string "BUILD SUCCESSFUL" in the file console_bootstrap_\d+\.txt
#
## Check all the intermediate steps of the build process
## 1st open file console_sfbuildall_\d+\.txt
#open (CONSOLESFBUILDALL, "<$logdir\\console_sfbuildall_\d+\.txt");
#
#my $searchconsolesfbuildall= <CONSOLESFBUILDALL>; # Copy the full file in one string because of /$=undef
#
#if ($searchconsolesfbuildall =~ /x/){
#	print "Found string x\n";	
#	print BUILDSUMMARYHTML "x=Yes<br/>";
#	}
#	else {
#		print "Can't Find string x\n";
#		print BUILDSUMMARYHTML "x=No<br/>";
#		}
#
#close (CONSOLESFBUILDALL); # Close file console_sfbuildall_\d+\.txt
#} # End use for local $/=undef

# Declare any variable that will need to be used globaly otherwise can't be used only between the curly brackets!
my $targettimeslog = "";

{ # Used for local $/=undef
local $/=undef; # indicates that the file will be put in one variable as one string

# The file targetTimesLog.csv contains all the information relative to the build process including time needed for each steps (generated by helium script)
# This file is available whatever the build we want to do (platform or package)
my $targettimeslogfile = "$logdir\\targetTimesLog\.csv";
print "$targettimeslogfile\n";
open (TARGETTIMESLOG, "<$targettimeslogfile");
{
	local $/=undef; # Technic used to get a file in one unique string accessible via a variable
	$targettimeslog = <TARGETTIMESLOG>;
}
close (TARGETTIMESLOG); # Close file targetTimesLog.csv
} # End use for local $/=undef

# Extract useful data from the file targettimeslog.csv
# step, time in second to do the operation

# For sure
#"^preparation-getenv:$" - the baseline retrieval starts
#preparation-getenv,1417
if ($targettimeslog =~ /(preparation-getenv[^,]*),(\d+[^,]*)\n/)
{
	print "Baseline retrieval started: OK \$1= $1, \$2= $2\n";
	print BUILDSUMMARYHTML "baselineretrievalstarted_val=yes<br/>baselineretrievalstarted_duration=$2<br/>";
}
else
{	
	print "Baseline retrieval started: NOK \$1= $1, \$2= $2\n";
	print BUILDSUMMARYHTML "baselineretrievalstarted_val=no<br/>baselineretrievalstarted_duration=?<br/>";
}

#"^\s+[exec] .+: done fetching environment$" - the baseline retrieval ends
#????

#"^sf-prebuild-0:$" - the source code sync starts
#sf-prebuild-0,363
if ($targettimeslog =~ /(sf-prebuild[^,]*),(\d+[^,]*)\n/)
{
	print "Source code sync started: OK \$1= $1, \$2= $2\n";
	print BUILDSUMMARYHTML "sourcecodestarted_val=yes<br/>sourcecodestarted_duration=$2<br/>";
}
else
{	
	print "Source code sync started: NOK \$1= $1, \$2= $2\n";
	print BUILDSUMMARYHTML "sourcecodestarted_val=no<br/>sourcecodestarted_duration=?<br/>";
}

#"^sf-unpack-rnd:$" - source code sync ends
#sf-unpack-rnd,20
if ($targettimeslog =~ /(sf-unpack[^,]*),(\d+[^,]*)\n/)
{
	print "Source code sync ended: OK \$1= $1, \$2= $2\n";
	print BUILDSUMMARYHTML "sourcecodeended_val=yes<br/>sourcecodeended_duration=$2<br/>";
}
else
{	
	print "Source code sync ended: NOK \$1= $1, \$2= $2\n";
	print BUILDSUMMARYHTML "sourcecodeended_val=no<br/>sourcecodeended_duration=?<br/>";
}

#"^sf-compile:$" - compilation starts
#sf-compile,3
if ($targettimeslog =~ /(sf-compile[^,]*),(\d+[^,]*)\n/)
{
	print "Compilation startd: OK \$1= $1, \$2= $2\n";
	print BUILDSUMMARYHTML "commpilationstarted_val=yes<br/>commpilationstarted_duration=$2<br/>";
}
else
{	
	print "Compilation startd: NOK \$1= $1, \$2= $2\n";
	print BUILDSUMMARYHTML "commpilationstarted_val=no<br/>commpilationstarted_duration=?<br/>";
}

#"^sf-postbuild:$" - compilation ends
#sf-postbuild,0
if ($targettimeslog =~ /(sf-postbuild[^,]*),(\d+[^,]*)\n/)
{
	print "Compilation ended: OK \$1= $1, \$2= $2\n";
	print BUILDSUMMARYHTML "compilationended_val=yes<br/>compilationended_duration=$2<br/>";
}
else
{	
	print "Compilation ended: NOK \$1= $1, \$2= $2\n";
	print BUILDSUMMARYHTML "compilationended_val=no<br/>compilationended_duration=?<br/>";
}

#"^sf-run-analysis:$" - analysis starts
#sf-run-analysis-raptor,1
if ($targettimeslog =~ /(sf-run-analysis-raptor[^,]*),(\d+[^,]*)\n/)
{
	print "Analysis started: OK \$1= $1, \$2= $2\n";
	print BUILDSUMMARYHTML "analysisstarted_val=yes<br/>analysisstarted_duration=$2<br/>";
}
else
{	
	print "Analysis started: NOK \$1= $1, \$2= $2\n";
	print BUILDSUMMARYHTML "analysisstarted_val=no<br/>analysisstarted_duration=?<br/>";
}

# Maybe????????

#"^sf-zip-logs:$" - analysis ends
#sf-zip-content,8
#?????


#"^publish:$" - publication starts
#????

#"\[copy\] Copying \d+ files to \\\\v800008\\Builds01\\SF_builds\\" - publication ends
#????

#BRAG
#sf-summary,5
#?????

# End - Check all the intermediate steps of the build process

# Copy Raptor summarise by DarioS
{
local $/=undef; # indicates that the file will be put in one variable as one string
open(RAPTORSUMINDEX, "<$logdir\\html\\index.html");

my $filecontent = <RAPTORSUMINDEX>;


# Like index.html will be called from the build_summary.html file, this means that is not calling the sub html file from the same directory \html.
# When copy index.html in the file build_summary.html, we need to include the path to \html directory
# We have:
# file:///F:/fbf_job/serviceapi_3k.T014/output/logs/raptor_unreciped.html
# We need:
# file:///F:/fbf_job/serviceapi_3k.T014/output/logs/html/raptor_unreciped.html
# This means we need to go from:
# <tr><td><a href='raptor_unreciped.html'>raptor_unreciped</a></td><td>0</td><td>0</td><td>190</td><td>0</td></tr>
# To:
# <tr><td><a href='html\raptor_unreciped.html'>raptor_unreciped</a></td><td>0</td><td>0</td><td>190</td><td>0</td></tr>
$filecontent =~ s/href=\'/href=\'html\\/ig;

print BUILDSUMMARYHTML "<br/>$filecontent<br/>"; # copy the content of the index.html file into the build_summary.html file.

close (RAPTORSUMINDEX);
}

# Copy html files for raptor summarise to v800020
# options used for xcopy
# /E -> Copies directories and subdirectories, including empty ones.
# /F -> Displays full source and destination file names while copying.
# /I -> If destination does not exist and copying more than one file, assumes that destination must be a directory.
my $copy_html = "xcopy $logdir\\html \\\\v800020\\Publish\\sf_builds\\$project\\builds\\$codeline\\$buildid\\html \/E \/F \/I";
print "Exec: $copy_html\n";
system($copy_html);


# Indicates end of the SUMMARISE section in the file
print BUILDSUMMARYHTML "<br/>#********** SUMMARISE ends here **********#<br/><br/>";

close(BUILDSUMMARYHTML); # Close build_summary.html

# End - Build info - Describe the completion of the diffrent steps in the build process


# *****************************************************************************
# Construction "DATA BREAKDOWN" page in build_summary.html
# *****************************************************************************

# Hyperlink to log files
open(BUILDSUMMARYHTML, ">>$logdir\\build_summary.html"); # Open build_summary.html in APPEND MODE!

# Indicates beginning of DATA BREAKDOWN section in the file
print BUILDSUMMARYHTML "<br/>#************************************************#<br/><br/>";
print BUILDSUMMARYHTML "<br/>#********** DATA BREAKDOWN starts here **********#<br/><br/>";


# Get tree for the directory where the log files are (v800020 probably the best place???)
# Should be already exported there, but maybe in a zip file)
# Extract each file with directory, then create a hyperlink to that file to make sure that user can click on the link and open the file!


#ex: platform_MCL.PDK-101_summary.log.xml
#######print FILE "buildid_summary.log.xml=<a class ="hoverlink" href ="file://///$logdir/$buildid_summary.log.xml">$buildid_summary.log.xml<br>";                        

# example of html hyperlink
#<a class ="hoverlink" href ="file://///builds01/devbuilds/MasterSF/logs/MSF00159_Symbian_OS_vtb92sf/MSF00159_Symbian_OS_vtb92sf.summary.html#AdvisoryNotesByComponent_Scan for S60 distribution policy files">Scan for S60 distribution policy files (1162) <br></a></td>                        
#<a class ="hoverlink" href ="file://///builds01/devbuilds/MasterSF/MSF00159_Symbian_OS_vtb92sf/logs/GT.summary.html#errorsByComponent_SBS: Error">SBS: Error (4) <br></a></td>
#<a class ="hoverlink" href ="file://///builds01/devbuilds/MasterSF/MSF00159_Symbian_OS_vtb92sf/logs/MSF00159_Symbian_OS_vtb92sf_cbr.summary.html#AdvisoryNotesByOverall_Total">Total (1) <br></a></td>
#<a class ="hoverlink" href="file://///builds01/devbuilds/MasterSF/logs/MSF00159_Symbian_OS_vtb92sf/AutoSmokeTest/EMULATOR/WINSCW/Test Summary Report.htm">WINSCW ( EMULATOR ) </a></td><td>

# All log files hyperlink have been created, close build_summary.html


# Indicates end of DATA BREAKDOWN section in the file
print BUILDSUMMARYHTML "<br/>#********** DATA BREAKDOWN ends here **********#<br/><br/>";
print BUILDSUMMARYHTML "<br/>#********** summary_build.html ends here ******#<br/><br/>";

close(BUILDSUMMARYHTML); # Close file build_summary.html



# *****************************************************************************
# Final step, copy build_summary.html to \\v800020 drive
# *****************************************************************************

# Copy html file to v800020
my $copy_cmd = "copy $logdir\\build_summary.html \\\\v800020\\Publish\\sf_builds\\$project\\builds\\$codeline\\$buildid";
print "Exec: $copy_cmd\n";
system($copy_cmd);


