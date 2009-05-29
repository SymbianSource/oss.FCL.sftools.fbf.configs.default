Update 20090519 by MattD

Can now run parselistdirs.pl from anywhere witout lots of intermediate output files:
perl parselistdirs.pl <logs_dir> (<output_dir>)
Note that it will still put most of it's output to the console as before. The second dir is optional.

Added find_collisions.pl
Prefers to be run from the root on the build machine (as it's looking for files to compare)
It's a hacked up version of merge_csv.pl
Usage:
perl find_collisions.pl what_results.log >collision_result.txt
It creates a CSV based on the name of the file passed to it (ie what_results.log_collisions.csv)

Yes, They are all quick and dirty.

Older stuff:

F:\6.2\generated\parselistdirs.pl - creates sets from the listdirs outputs...
F:\6.2\generated\parsewhatlog.pl - creates a CSV-style log from the whatlogs.
F:\6.2\generated\merge_csv.pl - takes the output CSV from parsewhatlog.pl and merges in the output of parselistdirs.pl
in generated>
perl parselistdirs.pl ..\ >list_results.log
perl parsewhatlog.pl ..\ >what_results.log
perl merge_csv.pl what_results.log list_results.log >summary.txt

The Test dir is a directory filled with test text files...



