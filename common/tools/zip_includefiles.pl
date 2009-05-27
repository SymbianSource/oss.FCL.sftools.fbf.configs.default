#!perl -w

use strict;

my @includefiles = glob "*_includefile.txt";
foreach (@includefiles)
{
    my $includefilename=$_;
	if (m/(.*)(_includefile.txt)/) {
		my $zipfilename = $1.".zip";
		system("7z a -tzip -i@".$includefilename . " " .$zipfilename);
	}
}