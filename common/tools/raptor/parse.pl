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
# Run the raptor parsers

use strict;
use FindBin;
use lib $FindBin::Bin;
use RaptorReleaseable;
use RaptorError;
use RaptorWarning;
use RaptorInfo;
use RaptorUnreciped;
use RaptorRecipe;

use XML::SAX;
use RaptorSAXHandler;
use Getopt::Long;

my @logfiles;
my $releaseable_module = 0;
my $error_module = 0;
my $warning_module = 0;
my $info_module = 0;
my $unreciped_module = 0;
my $recipe_module = 0;
our $basedir = '';
our $raptor_config = '';
my $append = 0;
my $help = 0;
GetOptions((
	'log:s' => \@logfiles,
	'releaseable!' => \$releaseable_module,
	'error!' => \$error_module,
	'warning!' => \$warning_module,
	'info!' => \$info_module,
	'unreciped!' => \$unreciped_module,
	'recipe!' => \$recipe_module,
	'basedir=s' => \$basedir,
	'config=s' => \$raptor_config,
	'append!' => \$append,
	'help!' => \$help
));

$help = 1 if (!@logfiles);

if ($help)
{
	print "Run the raptor parsers\n";
	print "Usage: perl parse.pl [MODULES] --log=FILE1 --log=FILE2 ... [OPTIONS]\n";
	print "where MODULES are:\n";
	print "\t--releaseable Extract releaseable (whatlog) information\n";
	print "\t--error Extracts raptor errors, i.e. content of <error> tags\n";
	print "\t--warning Extracts raptor warnings, i.e. content of <warning> tags\n";
	print "\t--info Extracts raptor info text i.e. content of <info> tags from a raptor log file\n";
	print "\t--unreciped Extracts output text in <buildlog> context which doesn't belong to any <recipe> tags\n";
	print "\t--recipe Extract, analyzes and dumps raptor recipes i.e. content of <recipe> tags from a raptor log file\n";
	print "where OPTIONS are:\n";
	print "\t--basedir=DIR Generate output file under DIR\n";
	print "\t--append Do not stop if basedir exists but append newly extracted info to already existing.\n";
	exit(0);
}

if (!$basedir)
{
	$basedir = time;
	
	print "Using $basedir as basedir.\n";
}
if (-d $basedir)
{
	if ($append)
	{
		print "Directory $basedir exists. Appending new info to it.\n";
	}
	else
	{
		print "Directory $basedir exists. Quitting.\n";
		exit(1);
	}
}
mkdir($basedir);
#print "Created dir $basedir.\n";

my $saxhandler = RaptorSAXHandler->new();
if ($releaseable_module)
{
	print "Adding RaptorReleaseable module to the observers\n";
	$saxhandler->add_observer('RaptorReleaseable', $RaptorReleaseable::reset_status);
}
if ($error_module)
{
	print "Adding RaptorError module to the observers\n";
	$saxhandler->add_observer('RaptorError', $RaptorError::reset_status);
}
if ($warning_module)
{
	print "Adding RaptorWarning module to the observers\n";
	$saxhandler->add_observer('RaptorWarning', $RaptorWarning::reset_status);
}
if ($info_module)
{
	print "Adding RaptorInfo module to the observers\n";
	$saxhandler->add_observer('RaptorInfo', $RaptorInfo::reset_status);
}
if ($unreciped_module)
{
	print "Adding RaptorUnreciped module to the observers\n";
	$saxhandler->add_observer('RaptorUnreciped', $RaptorUnreciped::reset_status);
}
if ($recipe_module)
{
	print "Adding RaptorRecipe module to the observers\n";
	$saxhandler->add_observer('RaptorRecipe', $RaptorRecipe::reset_status);
}
my $parser = XML::SAX::ParserFactory->parser(Handler=>$saxhandler);
for (@logfiles)
{
	$parser->parse_uri($_);
}

