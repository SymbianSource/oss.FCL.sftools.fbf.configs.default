## @file BuildEnvXML.pl
# @ingroup userscripts clientscripts
#
# Uses information which is defined in the BuildEnvXML.xml to set up a machine
# specific environment, such as path for compilers. 
#
# @todo Document this script.
#
# Copyright (c) 2009 Symbian Foundation  Ltd. All rights reserved.
#

package BuildEnvXML;

use Getopt::Long;
use BuildEnvXML qw(&getToolEnvironment &getLastErrorMsg);

my $toolenvxml = "";
my $dbgfile = "";
my $sbsconfig = "";
my $toolsconfig = "";

GetOptions ("xml=s" => \$toolenvxml, "dbg:s" => \$dbgfile, "sbs:s" => \$sbsconfig, "tools:s" => \$sbsconfig); 

my @contexts = @ARGV;

if ($toolenvxml eq "")
{
	print ("ERROR: XML file cannot be found!\n");
}

# Checking the SBS config to test only what's necessary for that build
if ($sbsconfig =~ /armv5/i)
{
	push @contexts, "ARMv5";
}

# Checking the tools config to test only what's necessary for that build
if ($sbsconfig =~ /tools[^2]?(,|$|_)/i)
{
	push @contexts, "tools";
}

my $sErrMsg;
my $result = 0;

if (-e $dbgfile)
{
	unlink($dbgfile);
}

&setDbgFile($dbgfile);

foreach my $context (@contexts){
	print "\n\n### Checking $context Tools ###\n";
	my $sSystemPath = &getToolEnvironment($toolenvxml, $context);

	$sErrMsg = &getLastErrorMsg;
	if ($sErrMsg eq "") { $sErrMsg = "\nAll OK.\n";}
	if($sErrMsg =~ /ERROR/)
	{
		print "\n$context Tools VERIFICATION FAILED:";
		print $sErrMsg."\n\n";
		$result = -1;
	}else{
		print "\n$context tools VERIFICATION PASSED:";
		print $sErrMsg."\n\n";
	}
}
if ($result ne 0)
{
	print "\n ==> PROGRAM STOPPED!!\n";
}
exit($result);
