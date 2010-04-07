use strict;

my $SYSDEFTOOLS_PATH = "packages\\sysdeftools";
my $XALAN_J = "java -jar $SYSDEFTOOLS_PATH\\xalan.jar";
my $XALAN_C = "packages\\sysmodelgen\\rsc\\installed\\Xalan\\Xalan.exe";

system("rmdir /S /Q tmp") if (-d "tmp");
mkdir("tmp");
chdir("tmp");

print "\n\n### CLONE FCL/sftools/fbf/projects/packages REPO ###\n";
system("hg clone -r HighFidelityModel http://developer.symbian.org/oss/FCL/sftools/fbf/projects/packages");
my $changeset = `hg -R packages identify -i`;
chomp $changeset;
print "-->$changeset<--\n";
my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime(time);
my $timestamp = sprintf "%4d%02d%02d%02d%02d%02d",$year+1900,$mon+1,$mday,$hour,$min,$sec;
#print "\n\n### CLONE MCL/sf/os/buildtools REPO ###\n";
#system("hg clone -r RCL_3 http://developer.symbian.org/oss/MCL/sf/os/buildtools");
print "\n\n### CLONE FCL/sftools/fbf/projects/platforms REPO ###\n";
system("hg clone -r default http://developer.symbian.org/oss/FCL/sftools/fbf/projects/platforms");

# get the codelines from the packages repo
opendir(DIR, "packages");
my @codelines = grep(($_ !~ /^\.\.?$/ and $_ =~ /^symbian/), readdir(DIR));
close(DIR);

# loop over codelines
for my $codeline (@codelines)
{
	mkdir($codeline);
	
	my $ROOT_SYSDEF = "packages\\$codeline\\os\\deviceplatformrelease\\foundation_system\\system_model\\system_definition.xml";

	# Full model in schema 3.0.0 format, including all of the test units.
	print "\n\n### GENERATE FULL MODEL ###\n";
	my $full_cmd = "$XALAN_C -o $codeline\\full_system_model_3.0.xml $ROOT_SYSDEF $SYSDEFTOOLS_PATH\\joinsysdef.xsl";
	print "$full_cmd\n";
	system($full_cmd);

	# Filter the model to remove the test and techview units
	print "\n\n### REMOVE UNDESIRED UNITS ###\n";
	my $filter_cmd = "$XALAN_C -o $codeline\\system_model_3.0.xml -p filter \"'!test,!techview'\" -p filter-type 'has' $codeline\\full_system_model_3.0.xml $SYSDEFTOOLS_PATH\\filtering.xsl";
	print "$filter_cmd\n";
	system($filter_cmd);

	# Downgrade the model to schema 2.0.1 for use with Helium and Raptor
	print "\n\n### DOWNGRADE TO SCHEMA 2.0.1 ###\n";
	my $downgrade_cmd = "$XALAN_C -o $codeline\\system_model.xml $codeline\\system_model_3.0.xml $SYSDEFTOOLS_PATH\\sysdefdowngrade.xsl";
	print "$downgrade_cmd\n";
	system($downgrade_cmd);

	print "\n\n### PUSH TO PLATFORMS REPOSITORY (auto) ###\n";
	my $isdifferent = 1;
	#compare to latest
	if (-d "platforms\\$codeline\\single\\sysdefs\\auto")
	{
		opendir(DIR, "platforms\\$codeline\\single\\sysdefs\\auto");
		my @files = grep(($_ !~ /^\.\.?$/ and $_ =~ /^model_/), readdir(DIR));
		@files = sort { $b cmp $a } @files;
		close(DIR);
		
		my $mostrecent = shift @files;
		print "mostrecent $mostrecent\n";
		
		#compare
		my $file1 = '';
		my $file2 = '';
		{
			local $/ = undef;
			open(FILE1, "platforms\\$codeline\\single\\sysdefs\\auto\\$mostrecent");
			{
				$file1 = <FILE1>;
			}
			close(FILE1);
			open(FILE2, "$codeline\\system_model.xml");
			{
				$file2 = <FILE2>;
			}
			close(FILE2);
		}
		$isdifferent = 0 if ($file1 eq $file2);
	}
	if ($isdifferent)
	{
		mkdir("platforms\\$codeline") if (!-d "platforms\\$codeline");
		mkdir("platforms\\$codeline\\single") if (!-d "platforms\\$codeline\\single");
		mkdir("platforms\\$codeline\\single\\sysdefs") if (!-d "platforms\\$codeline\\single\\sysdefs");
		mkdir("platforms\\$codeline\\single\\sysdefs\\auto") if (!-d "platforms\\$codeline\\single\\sysdefs\\auto");
		system("copy $codeline\\system_model.xml platforms\\$codeline\\single\\sysdefs\\auto\\model_$timestamp\_$changeset.xml");
		system("hg -R platforms add");
		system("hg -R platforms commit -m \"Add auto generated $codeline system model (packages\@$changeset)\" -u\"Dario Sestito <darios\@symbian.org>\"");
		system("hg -R platforms push http://darios:symbian696b\@developer.symbian.org/oss/FCL/sftools/fbf/projects/platforms");
	}
}

