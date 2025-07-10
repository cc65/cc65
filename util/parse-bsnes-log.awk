#!/usr/bin/awk -nf
#
#	Parse the bsnes-plus cpu log for the 65816 address mode ranges.
#	Since the log doesn't show the op size, we have to be careful
#	and add +3 to the range end - this may need manual cleanup.
#
#	Grep the input for only one bank's content beforehand.
#
#	(C) Lauri Kasanen, under the cc65 license
#

{
	addr = $1

	IGNORECASE = 1
	if ($9 ~ /^nv/) {
		val = $9
	} else if ($10 ~ /^nv/) {
		val = $10
	} else {
		val = $11
	}
	IGNORECASE = 0

	val = substr(val, 3, 2)
	if (val == "1B") # emulation mode
		val = "MX"

	addrs["0x" substr(addr, 3)] = val
}

END {
	PROCINFO["sorted_in"] = "@ind_num_asc"

	start = -1
	prevval = ""

	for (addr in addrs) {
		cur = addrs[addr]
		if (start == -1) { # first range
			start = addr
		} else {
			if (prevval != cur || addr - prevaddr > 4) { # start a new range
				#print "diff " addr - prevaddr ", addr " addr ", pa " prevaddr
				if (prevaddr + 3 >= addr + 0)
					fmt = sprintf("%04x", addr - 1)
				else
					fmt = sprintf("%04x", prevaddr + 3)
				print "RANGE { START $" substr(start, 3) "; END $" fmt "; ADDRMODE \"" prevval "\"; TYPE Code;};"
				start = addr
			}
		}
		#print "\t" addr " " addrs[addr]

		prevval = cur
		prevaddr = addr
	}

	fmt = sprintf("%04x", prevaddr + 3)
	print "RANGE { START $" substr(start, 3) "; END $" fmt "; ADDRMODE \"" prevval "\"; TYPE Code;};"
}
