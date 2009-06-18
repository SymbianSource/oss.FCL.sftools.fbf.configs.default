#!/usr/bin/perl

use strict;
use File::Copy;
use File::Path;

my $epoc=$ENV{'EPOCROOT'} . "epoc32/";
copy("BC_PLAT.h",	$epoc . "include");
