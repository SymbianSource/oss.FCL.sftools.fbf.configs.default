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
#
# Description:
# Extracts output text in <buildlog> context which doesn't belong to <recipe>'s

use strict;
use Getopt::Long;

my $XALAN_PATH = "xalan-j_2_7_1";
my $XSLT_FILE = "sysdefdowngrade.xsl";

my $modelfile = '';
my $csvsourcesfile = '';
my $help = 0;
GetOptions((
	'model=s' => \$modelfile,
	'sources=s' => \$csvsourcesfile,
	'help!' => \$help
));

$help = 1 if (!$modelfile or !$csvsourcesfile);

if ($help)
{
	print "Checks that a model file is not in version 3.0.0. If it is transforms it to a version usable by Raptor\n";
	print "Usage: perl sysdefdowngrade.pl --model=MODELFILE --sources=SOURCESFILE\n\n";
	exit(0);
}

my $isversion3 = 0;
open(MODELFILE, "$modelfile") or die("Can't open model file $modelfile");
while(<MODELFILE>)
{
	if ($_ =~ /<SystemDefinition schema="3\.0\.0"/)
	{
		$isversion3 = 1;
		last;
	}
}
close(MODELFILE);

if (!$isversion3)
{
	print "Model file is not version 3.0.0. Nothing to do.";
	exit(0);
}

my $param_path = '';
open(SOURCESFILE, "$csvsourcesfile") or die("Can't open sources file $csvsourcesfile");
while(<SOURCESFILE>)
{
	if ($_ =~ m{[^,]*,([\\/]sf[\\/][^,]*),[^,]*,[^,]*,[^,]*})
	{
		$param_path = $1;
		#print "param_path= $param_path\n";
		last;
	}
}
close(SOURCESFILE);

if (!$param_path)
{
	print "Error: could not determine path parameter from sources file. Quitting.\n";
	exit(0);
}

$param_path =~ s,\\,/,g;

my $transformcmd = "java -jar $XALAN_PATH\\xalan.jar -xsl $XSLT_FILE -in $modelfile -out $modelfile.transformed";
$transformcmd .= " -param Path $param_path";

print "Executing: $transformcmd\n";
system("$transformcmd");

# ren doesn't work with forward slashes
$modelfile =~ s,/,\\,g;

$modelfile =~ m,.*[\\/](.*),;
my $modelfile_name = $1;
print "Executing: ren $modelfile $modelfile_name.orig\n";
system("ren $modelfile $modelfile_name.orig");

print "Executing: ren $modelfile.transformed $modelfile_name\n";
system("ren $modelfile.transformed $modelfile_name");
