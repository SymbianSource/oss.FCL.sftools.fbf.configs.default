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
# Generate build summary with BRAG status

use Getopt::Long;

my $buildid = '';
my $basedir = '';
my $help = 0;
GetOptions((
	'buildid=s' => \$buildid,
	'basedir=s' => \$basedir,
	'help!' => \$help
));

$help = 1 if (!$buildid or !$basedir);

if ($help)
{
	print "Generate build summary with BRAG status\n";
	print "Usage: perl brag_script.pl --buildid=ID --basedir=DIR\n";
	exit(0);
}

$buildid =~ /^([^_]*)_([^.]*)\./;
my $project = $1;
my $codeline = $2;

my $logdir = "$basedir\\$buildid\\output\\logs";

opendir(DIR, $logdir);
my @dir_content = readdir(DIR);
close(DIR);
#my @asSimilarDirs = grep(/^$sBaseName(\.|$)/, @asDirs);

my $nfiles = scalar(@dir_content);


open(FILE, ">$logdir\\build_summary.html");
print FILE "<html><body>build summary<br/>build id: $buildid<br/>log files: $nfiles</body></html>";
close(FILE);

my $copy_cmd = "copy $logdir\\build_summary.html \\\\bishare\\sf_builds\\$project\\builds\\$codeline\\$buildid";
print "Exec: $copy_cmd\n";
system($copy_cmd);


