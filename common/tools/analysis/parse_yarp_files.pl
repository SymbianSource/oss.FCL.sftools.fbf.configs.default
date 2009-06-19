
my @asYarpFiles = ();
open(FILE, $ARGV[0]);
while ( <FILE> )
{
	if ( m/[^,]*,([^,]*)/ )
	{
		my $sFile = $1;
		chomp $sFile;
#		print "$sFile\n";
		push(@asYarpFiles, $sFile);
	}
}
close(FILE);
my $nYarpFiles = scalar(@asYarpFiles);
#print "Found $nYarpFiles files to parse\n";

my $nTotalRecipeErrors = 0;
for my $nYarpFile (@asYarpFiles)
{
	my $nRecipeErrors = 0;
#	print "Parsing file $nYarpFile...\n";
	open(FILE, $nYarpFile);
	while ( <FILE> )
	{
		if ( m/^Raptor recipe failures:\s+(\d+)$/ )
		{
			#print "Attempt: $1 Errors: $2\n";
			$nRecipeErrors = $1;
		}
	}
	close(FILE);
#	print "Recipe errors in $nYarpFile: $nRecipeErrors\n";
	$nTotalRecipeErrors += $nRecipeErrors;
}
#print "Total recipe errors across all files: $nTotalRecipeErrors\n";

print "$nTotalRecipeErrors";