CPU Detect Tests
----------------

These tests all assemble the same file "cpudetect.s" which contains several
conditionals for several CPUs, only using every option known to the "--cpu"
command-line switch of ca65/cl65.

Reference (".ref") Files
------------------------

Some hints about creating new files:
Make an empty file with the CPU's name prepended to "-cpudetect.ref". Run the
tests; one of them will fail due to a mismatch. Review the output of the
".lst" file pedantically, then copy the ".bin" over the empty ".ref" file.

