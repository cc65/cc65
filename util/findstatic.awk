#!/usr/bin/awk -f
#
#	List all variables/functions that can be made static.
#	findstatic.awk *.o
#
#	(C) Lauri Kasanen, under the cc65 license
#

BEGIN {
	delete ARGV[0]
	importbase = "od65 --dump-imports "
	exportbase = "od65 --dump-exports "

	for (i in ARGV) {
		importcmd = importbase ARGV[i] " "
		exportcmd = exportbase ARGV[i] " "

		while ((importcmd | getline) > 0) {
			if (! /Name:/) continue
			sub(/Name:/, "")
			gsub(/\"/, "")

			imports[$1] = 1
		}

		while ((exportcmd | getline) > 0) {
			if (! /Name:/) continue
			sub(/Name:/, "")
			gsub(/\"/, "")

			exports[ARGV[i]][$1] = 1
		}
	}

	for (file in exports) {
		printed = 0
		for (sym in exports[file]) {
			if (sym in imports)
				continue

			if (!printed) {
				printed = 1
				print "Unique symbols in " file ":"
			}

			print "\t" sym
		}
	}
}
