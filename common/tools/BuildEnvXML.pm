## @file BuildEnvXML.pm
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
use strict;
use XML::DOM;
use Cwd;
use Exporter;
our @ISA = qw(Exporter);
our @EXPORT_OK = qw(&getToolEnvironment &getLastErrorMsg &setDbgFile);

my $sToolEnv  = "";
my $sLocalDir = cwd();
my $hParser      = XML::DOM::Parser->new();
my $sErrorMsg = "";
my $toolSummary = "";
my $execLog = "";
my $dbgfile = "";

sub getToolEnvironment
{
 my $sBuildEnvXML = shift;
 my (@aCat) = @_;

 my $hXMLfile     = $hParser->parsefile ($sBuildEnvXML);
 my $hNodes = $hXMLfile->getElementsByTagName ("context");
 my $hSystemTools, my $hP4Tools, my $hCBRTools, my $sCat, my $nNumNodes;
 
 if ($dbgfile ne "")
 {
	print "Logging output to file $dbgfile\n";
	open DBGFILE, ">>$dbgfile" or die; 
 }

 $sErrorMsg = "";
 $sToolEnv  = "";
 my $nCatDepth = scalar(@aCat);
 subTrace("Context Depth: $nCatDepth\n");
 my $nLevel = ($nCatDepth-1);
 $sCat = $aCat[$nLevel];
 $nNumNodes = $hNodes->getLength;

 my $hParentNode, my $sParentId;
 for (my $nI = 0; $nI < $nNumNodes ; $nI++)
 {
     my $hNode = $hNodes->item ($nI);
     my $hId = $hNode->getAttributeNode ("id");
     # Discriminating Stages
     subTrace("Context found: \"".$hId->getValue()."\"");
     if (uc($hId->getValue()) eq uc($sCat))
     {
     	subTrace("---> requested! analyzing...");
     	$hParentNode = $hNode->getParentNode;
	     for (;$nLevel>0;$nLevel--)
	     {
		    $sParentId = $hParentNode->getAttributeNode("id")->getValue;
	    	subTrace("...from Parent context= ".$sParentId);
		    if ($sParentId ne $aCat[$nLevel-1]){ $nLevel =-1;}
		    $hParentNode = $hParentNode->getParentNode;
	     };
	     if (($nLevel>-1)&&($hParentNode->getNodeName ne "context"))
	     {
		     $hSystemTools = $hNode->getElementsByTagName ("tool", 0);
		     $nI = $nNumNodes;
	     }
     }
 }
 if (!defined $hSystemTools)
 {
	 subTrace("No Stage found for category $sCat !");
	 subTrace("Program Terminated.");
	 exit(0);
 }
 subTrace(""); 
 subTrace("----------------------");
 subTrace("Number of $sCat tools: ".$hSystemTools->getLength);
 for (my $nI = 0; $nI < $hSystemTools->getLength; $nI++)
 {
     my $hTool     = $hSystemTools->item ($nI);
     my $hToolName = $hTool->getAttributeNode ("name");
     subTrace("");
     subTrace("----------------------");
     subTrace("Verifying tool: ".$hToolName->getValue());
	 my $isToolNotFound = "true";

     # Location element
     my $hToolLocation = $hTool->getElementsByTagName("location")->item(0);
     my $hToolLocationPath, my $hToolLocationStrict;
     if (defined $hToolLocation)
     {
             $hToolLocationPath   = $hToolLocation->getAttributeNode("value");
             $hToolLocationStrict = $hToolLocation->getAttributeNode("strict");
	     subTrace("Expected Location: ".$hToolLocationPath->getValue());    
     }

     # Verify element
	 my $hToolVerifys           = $hTool->getElementsByTagName("verify");
	 my $nNumOfVerifys			= $hToolVerifys->getLength;
	 for (my $nZ = 0; $nZ < $nNumOfVerifys; $nZ++)
	 {
		 my $hToolVerify            = $hTool->getElementsByTagName("verify")->item($nZ);
		 my $hToolVerifyVersion     = $hToolVerify->getAttributeNode("version");
		 my $hToolVerifyShowStopper = $hToolVerify->getAttributeNode("showstopper");
		 subTrace("Expected Version: ".$hToolVerifyVersion->getValue());    
		 subPrint("\nVerifying tool: ".$hToolName->getValue()." (version ".$hToolVerifyVersion->getValue().")\n");
		 
		 # Tool checks
		 my $hToolChecks = $hToolVerify->getElementsByTagName("check");
		 my $nNumOfChecks= $hToolChecks->getLength;
		 my $nCheckNum;
		 for (my $nJ = 0; $nJ < $nNumOfChecks; $nJ++)
		 {
			 $nCheckNum = $nJ+1;
			 subTrace("");
			 subTrace("Check #$nCheckNum:");
			 subPrint("Check #$nCheckNum");
			 my $hCheck = $hToolChecks->item($nJ);
			 my $hCheckCmd    = $hCheck->getAttributeNode ("execute");
			 my $hCheckVar    = $hCheck->getAttributeNode ("var");
			 my $hCheckValues = $hCheck->getAttributeNode ("values");
			 my $hCheckLog    = $hCheck->getAttributeNode ("log");
			 my $hCheckExclu  = $hCheck->getAttributeNode ("exclusive");
			 my $hCheckName   = $hCheck->getAttributeNode ("name");
			 my $hCheckLocator= $hCheck->getAttributeNode ("locator");
			 my $hCheckSilent = $hCheck->getAttributeNode ("silent");
			 my $sCommand, my $sPattern, my $sToolCmd, my $sOut, my $sOutLocal, my $sCheckName;
			 my $versionFound = "";
			 my $sMsg = "";
			 # If Check is exclusive, we will only test this one:
			 if ($hCheckExclu->getValue() eq "true")
			 {
				 $nJ = $nNumOfChecks;
				 subTrace("This check is exclusive: the remaining checks will not be tested");
			 }
			 # Preparing system command to execute
			 if (defined $hCheckCmd) 
			 {
				 $sCommand = $hCheckCmd->getValue();
				 if (defined $hToolLocation)
				 {
					 $sToolCmd= $hToolLocationPath->getValue();
					 $sCommand =~ s/\%location\%/$sToolCmd/g;
				 }
				 
				 # Execute the command
				 subTrace("Check cmd: ".$sCommand."");
				 $sOut = `$sCommand 2>&1`;
				 $sOutLocal = $sOut;
				 
				 # Check if tool cannot be found at expected location
				 if ($sOut =~/(not\srecognized)|(system\scannot\sfind)/)
				 {
					 $sOutLocal = "";
					 subTrace("Tool cannot be found at location: ".$hToolLocationPath->getValue());
					 subLogErrorMsg($hToolLocationStrict->getValue(), $hToolName->getValue()." CANNOT BE FOUND IN THE EXPECTED LOCATION: "
								 .$hToolLocationPath->getValue());
					 if ($hToolLocationStrict->getValue() eq "true")
					 {
						 subTrace("LOCATION STRICT activated, program terminated");
						 $sMsg .= "[ERROR] wrong location\n";
						 #return "";
					 }
					 else
					 {
						 $sMsg .= "[WARNING] wrong location\n";
					 }	 
				 }
				 else
				 {
					 if ((defined $hToolLocation)&&(defined $hCheckCmd))
					 {
						$sCommand =~ /(.*)(\\).*$/i;
						my $sTmp  = $1;
						$sTmp =~ s/^\s+//;
						$sTmp =~ s/\s+$//;
						$sToolEnv = $sTmp.";".$sToolEnv;
						subTrace("Tool path OK.");
						subTrace("New appender: \"".$sToolEnv."\"");
					 }
					 $isToolNotFound = "false";
				 }
				 
				 # Check if tool can be found with the Environment PATH
				 $sCommand =~ s/.*(\\)(.*)$/\2/i;
				 subTrace("Try command from ENV PATH: ".$sCommand);
				 if (!defined $hCheckName) 
				 {
					$sCheckName = $sCommand;
				 }
				 else
				 {
					$sCheckName = $hCheckName->getValue();
				 }
				 subPrint(" [$sCheckName]: ");
				 $sOut = `$sCommand 2>&1`;
				 if ($sOut =~/(not\srecognized)|(system\scannot\sfind)/)
				 {
					 $sOut = "";
					 my $type = "ERROR";
					 if ($isToolNotFound eq "false") {$type = "WARNING";}
					 subTrace("$type: ".$hToolName->getValue()." CANNOT BE FOUND IN THE SYSTEM PATH");
					 subLogErrorMsg($isToolNotFound, $hToolName->getValue()." CANNOT BE FOUND IN THE SYSTEM PATH");
					 $sMsg .= "[$type] NOT FOUND IN SYSTEM PATH\n";
					 if ($isToolNotFound eq "true") {next;}
					 #return "";
				 }
			 }
			 else
			 {	 # Check by verifying an env variable
				 subPrint(" [".$hCheckVar->getValue()."]: ");
				 $sCommand = "ECHO %".$hCheckVar->getValue()."%";
				 subTrace("Check var: ".$hCheckVar->getValue());		     
				 $sOut = `$sCommand 2>&1`;
				 $sOut =~ s/^\s*//;
				 $sOut =~ s/\s*$//;
			 }
			 
			 # Log the output if necessary 
			 if ($hCheckLog->getValue() eq "true")
			 {
					 subTrace("command: $sCommand");
					 subTrace("logged output: ");
					 subLog("Verifying \"".$hToolName->getValue()." [version ".$hToolVerifyVersion->getValue().
					 "]\" -- check #$nCheckNum:");
					 subLog("Executing: $sCommand");
					 subLog($sOut);
			 }
			 
			 # Perform the check against given values
			 if (defined $hCheckValues)
			 {
				 my $sOutCopy = $sOut;
				 my $bResultOk = "false";
				 $sPattern = $hCheckValues->getValue();
				 if (defined $hCheckLocator)
				 {
					 my $sLocator = $hCheckLocator->getValue();
					 $sOut =~ s/.*$sLocator.*/\1/msi;
					 subTrace("Searching in locator: ".$sOut);
					 $sOutLocal =~ s/.*$sLocator.*/\1/msi;
				 }
				 else
				 {
					$sOut =~ s/.*($sPattern).*/\1/smi;
					$sOutLocal =~ s/.*($sPattern).*/\1/smi;
				 }
				 
				# Log the current version found
				$versionFound = $sOut.$sOutLocal;
				if ($sOut eq $sOutLocal)
				{
					$versionFound = $sOut;
				}
				elsif (($sOut.$sOutLocal ne $sOut) and ($sOut.$sOutLocal ne $sOutLocal) and ($sOut ne $sOutLocal))
				{
					$versionFound = "$sOut or $sOutLocal [Value Mismatch]";
				}
				if ($versionFound eq "") {$versionFound = "not found";}
				 
				 #$sPattern =~ s/(\\|\/|\.|\*|\[|\]|\(|\)|\$|\{|\})/\\\1/g;
				 #$sPattern = "($sPattern)";
				 subTrace("Check against pattern: ".$sPattern);
				 # Check the Environment Variable
				 if (defined $hCheckVar)
				 {
				    $sOut = $sOutCopy;
					my $valueTested = $hCheckVar->getValue();
					if ($sOut =~ /%$valueTested%/) {$sOut = "not defined!";}
					elsif ( $sOut =~ m/$sPattern/smi )
					{
						$bResultOk = "true";
						if ($hCheckSilent->getValue() eq "true") { $sOut = "ok. ";}
					}
					$versionFound = "$sOut";
				 } # Or Check the command executed (for version checking...)
				 elsif ( ( ( $sOut =~ s/.*($sPattern).*/\1/smi ) and (( $sOutLocal =~ s/.*($sPattern).*/\1/smi ) or ($sOutLocal eq "")) ) || 
				      ( ( $sOutLocal =~ s/.*($sPattern).*/\1/smi ) and ($sOut eq "") ) )
				 {
					$bResultOk = "true";
					subTrace("Value OK");
					if ($versionFound eq "" || $hCheckSilent->getValue() eq "true") { $versionFound = "ok. ";}
					
				 }
				 elsif (!defined $hCheckLocator)
				 {
					$versionFound = "";
				 }

				 # Log the warnings/errors regarding version checking
				 if ($bResultOk eq "false")
				 {
					 if ($versionFound eq "") { $versionFound = "wrong value. ";}
					 subTrace("ERROR: VALUE CHECK FAILED");
					 subLogErrorMsg($hToolVerifyShowStopper->getValue(), $hToolName->getValue()." VALUE INCORRECT");
					 if ($hToolVerifyShowStopper->getValue() eq "true")
					 {
						 $sMsg .= "[ERROR] WRONG VALUE\n";
						 subTrace("SHOWSTOPPER RAISED: program should be terminated");
					 }
					 else
					 {
						 subTrace("No Showstopper, continuing analysis...");
						 $sMsg .= "[WARNING] Wrong Value\n";
					 }
					 
				 }
			 }
			 chomp($versionFound);
			 &subPrint ("$versionFound\n$sMsg");
			 &printToolSummary;
			 &printLog;
		 }		
	 }

     
 }
 close DBGFILE;
 return $sToolEnv;
}

sub subTrace
{
  my $sString = $_[0];
  if ($dbgfile ne "")
  {
	print DBGFILE "$sString\n";
  }
}

sub subPrint
{
  my $sString = $_[0];
  $toolSummary .= "$sString";
}

sub printToolSummary
{
  print $toolSummary."\n";
  $toolSummary="";
}

sub subLog
{
  my $sString = $_[0];
  $execLog .= "[LOG] $sString\n";
  
  if ($dbgfile ne "")
  {
	print DBGFILE "[LOG] $sString\n";
  }
}

sub printLog
{
  #print $execLog;
  $execLog="";
}

sub subLogErrorMsg
{
  my $isError = shift;
  my $msg = shift;
  my $type = "WARNING";
  if ($isError eq "true") {$type = "ERROR";} 
  $sErrorMsg = $sErrorMsg."\n[$type] ".$msg;
}

sub getLastErrorMsg
{
  return $sErrorMsg;
}

sub setDbgFile
{
  $dbgfile = shift;
}

1;
