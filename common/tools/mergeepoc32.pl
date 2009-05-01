#!perl -w
use strict;

# Assume that all rnd repositories are delivered onto the build machine in \rnd\category\name\epoc32\...

my @rndRepositories = glob "/rnd/*/*";
foreach (@rndRepositories)
{
	s{/}{\\}g;
	system("xcopy /Q/C/I/Y/E $_\\epoc32 \\epoc32 > nul:");
}

