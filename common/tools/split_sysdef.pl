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
# Split a system model file into many package model files (one per package)

use strict;

use Getopt::Long;
use XML::DOM;
use File::Path;

my $DEFAULT_OUTDIR = "models";

my $sysdef = '';
my $outdir = $DEFAULT_OUTDIR;
my $help = 0;
GetOptions((
	'sysdef=s' => \$sysdef,
	'outdir:s' => \$outdir,
	'help!' => \$help
));

$help = 1 if (!$sysdef);

if ($help)
{
	print "Split a system model file into many package model files (one per package)\n";
	print "Usage: perl split_sysdef.pl --sysdef=FILE [OPTIONS]\n";
	print "where:\n";
	print "\tFILE is the input file to split\n";
	print "and OPTIONS are:\n";
	print "\t--outdir=DIR Generate package model files under DIR (default: $DEFAULT_OUTDIR)\n";
	exit(0);
}

mkpath("$outdir") if (!-d $outdir);

my $parser = new XML::DOM::Parser;
my $doc = $parser->parsefile ($sysdef);

# fix bldFile and mrp paths
for my $unit ( $doc->getElementsByTagName("unit") )
{
	my $bldfile = $unit->getAttribute("bldFile");
	if ($bldfile)
	{
		$bldfile =~ s,\\,/,g;
		$bldfile = "/$bldfile" if ( $bldfile !~ m,^/, );
		$bldfile = "/sf$bldfile" if ( $bldfile !~ m,^/sf, );
		if ($bldfile =~ m,^/sf/(os|mw|app|tools|ostools|adaptation)/,)
		{
			$unit->setAttribute("bldFile", $bldfile);
		}
		else
		{
			print "WARNING: unexpected path in bldFile: $bldfile. Keeping as is\n";
		}
	}

	my $mrp = $unit->getAttribute("mrp");
	if ($mrp)
	{
		$mrp =~ s,\\,/,g;
		$mrp = "/$mrp" if ( $mrp !~ m,^/, );
		$mrp = "/sf$mrp" if ( $mrp !~ m,^/sf, );
		if ($mrp =~ m,^/sf/(os|mw|app|tools|ostools|adaptation)/,)
		{
			$unit->setAttribute("mrp", $mrp);
		}
		else
		{
			print "WARNING: unexpected path in mrp: $mrp. Keeping as is\n" ;
		}
	}
}

my $packagedef_header = '';
my $packagedef_trailer = '';

my $systemdefinition = $doc->getElementsByTagName("SystemDefinition")->item(0);
my $systemdefinition_attributes = $systemdefinition->getAttributes();
my $systemdefinition_attributes_tostring = '';
for ($systemdefinition_attributes->getValues) {$systemdefinition_attributes_tostring .= " ".$_->getName."=\"".$_->getValue."\"";}
$packagedef_header .= "<SystemDefinition$systemdefinition_attributes_tostring>\n";
$packagedef_trailer = "\n</SystemDefinition>".$packagedef_trailer;

my $systemmodel = $systemdefinition->getElementsByTagName("systemModel")->item(0);
my $systemmodel_attributes = $systemmodel->getAttributes();
my $systemmodel_attributes_tostring = '';
for ($systemmodel_attributes->getValues) {$systemmodel_attributes_tostring .= " ".$_->getName."=\"".$_->getValue."\"";}
$packagedef_header .= " <systemModel$systemmodel_attributes_tostring>\n";
$packagedef_trailer = "\n </systemModel>".$packagedef_trailer;

for my $layer ( $systemmodel->getElementsByTagName("layer") )
{
	my $layer_name = $layer->getAttribute("name");
	
	my $layer_attributes = $layer->getAttributes();
	my $layer_attributes_tostring = '';
	for ($layer_attributes->getValues) {$layer_attributes_tostring .= " ".$_->getName."=\"".$_->getValue."\"";}
	my $layer_header = "  <layer$layer_attributes_tostring>\n   ";
	my $layer_trailer = "\n  </layer>";
	
	for my $block ( $layer->getElementsByTagName("block") )
	{
		my $block_name = $block->getAttribute("name");
		
		mkdir("$outdir/$layer_name") if (!-d "$outdir/$layer_name");
		mkdir("$outdir/$layer_name/$block_name") if (!-d "$outdir/$layer_name/$block_name");
		
		# what source code doesn't belong to this package ?
		for my $unit ( $block->getElementsByTagName("unit") )
		{
			my $bldinf = $unit->getAttribute("bldFile");
			if ($bldinf and $bldinf !~ m,^/sf/$layer_name/$block_name,)
			{
				print "WARNING: $bldinf is in package: $layer_name/$block_name\n";
			}
		}
		
		open(FILE, ">$outdir/$layer_name/$block_name/package_definition.xml");
		print FILE $doc->getXMLDecl->toString."\n" if ($doc->getXMLDecl);
		print FILE $doc->getDoctype->toString."\n" if ($doc->getDoctype);
		print FILE $packagedef_header;
		print FILE $layer_header;
		print FILE $block->toString;
		print FILE $layer_trailer;
		print FILE $packagedef_trailer;
		close(FILE);
	}
}

$doc->dispose;

