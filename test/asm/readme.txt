Assembler Testcases
===================

cpudetect
---------

Tests the --cpu command line option of ca65/ld65.
Refer to cpudetect/readme.txt


opcodes
-------

Test of assembler opcodes for each CPU.
Refer to opcodes/readme.txt


err:
----

Used to test assembler errors. These tests MUST NOT assemble without error.


listing:
--------

This is the most versatile assembler test form, allowing control customizations,
reference tests for binary output, stdout and error text ouput, error tests,
listings, custom linker configuration, etc. as needed.
Refer to listing/readme.txt


val:
----

Runtime assembly tests using sim65 that should end with an exit code of 0 if
they pass. If they fail the exit code should be either -1, or a number
indicating what part of the test failed.
