This directory contains test code for automatic regression testing of the CC65
compiler.


/val  - the bulk of tests are contained here, individual tests should exit with
        an exit code of EXIT_SUCCESS when they pass, or EXIT_FAILURE on error

/ref  - these tests produce output that must be compared with reference output

/err  - contains tests that MUST NOT compile

/misc - a few tests that need special care of some sort


to run the tests use "make" in this (top) directory, the makefile should exit
with no error.

when a test failed you can use "make continue" to run further tests

--------------------------------------------------------------------------------

TODO:

- reduce usage of "common.h" to a minimum
- convert more tests from using reference output to returning an exit code
