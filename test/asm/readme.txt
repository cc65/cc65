Assembler Testcases
===================

Opcode Tests:
-------------

these go into opcodes/. Refer to opcodes/readme.txt


CPU Detect Tests
----------------

these go into cpudetect/. Refer to cpudetect/readme.txt


Overall tests:
--------------

These go into listing/. Refer to listing/readme.txt

Val:
----

Works very much like the /val directory used to test the compiler -  individual
tests are run in the simulator and should exit with an exit code of 0 when they
pass, or either -1 or a number indicating what part of the test failed on error.
