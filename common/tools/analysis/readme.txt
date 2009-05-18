


F:\6.2\generated\parselistdirs.pl - creates sets from the listdirs outputs...

F:\6.2\generated\parsewhatlog.pl - creates a CSV-style log from the whatlogs.

F:\6.2\generated\merge_csv.pl - takes the output CSV from parsewhatlog.pl and merges in the output of parselistdirs.pl


in generated>
perl parselistdirs.pl ..\ >list_results.log
perl parsewhatlog.pl ..\ >what_results.log
perl merge_csv.pl what_results.log list_results.log >summary.txt

The Test dir is a directory filled with test text files...



