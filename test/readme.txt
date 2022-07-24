This directory contains test code for automatic regression testing of the CC65
compiler and tools.

/asm  - contains the assembler regression tests

/dasm - contains the disassembler regression tests


/val, /ref and /err generally contain the tests that are used to verify that the
compiler is working as expected (when the tests behave as described):

/val  - The bulk of tests are contained here, individual tests should exit with
        an exit code of EXIT_SUCCESS when they pass, or EXIT_FAILURE on error.

/standard - like the tests in /val, the tests must exit with EXIT_SUCCESS on
        success. Unlike the tests in /val these are not compiled for every
        combination of optimizer options, but instead always with -Osir and then
        for each supported C-standard (C89, C99, CC65). The goal is to use these
        to check for regressions in standard conformance of the compiler and the
        library.

/ref  - These tests produce output that must be compared with reference output.

/err  - contains tests that MUST NOT compile


/todo and /misc generally contain the tests that fail because of known bugs:

/todo - These tests fail due to open compiler issues.

        The makefile in this directory _expects_ the tests to fail, because of
        that when an issue was fixed it will break the CI. The test should get 
        moved to /val in the PR fixing the issue, which will make CI pass again.
        No changes to makefiles are required!


/misc - a few tests that need special care of some sort

        Tests that (incorrectly) fail to compile and other tests that fail and
        do NOT return an exit code are collected here. The makefile _expects_
        those tests to fail, so when an issue is fixed it will break the CI.
        When this happens, the PR fixing the issue should also "invert" the
        failing condition in the makefile by adding a $(NOT) before the
        offending line (or removing it when it is already there), which will
        make the CI pass again. The test should then be moved elsewhere later,
        which will require additional changes to the makefile(s).


To run the tests use "make" in this (top) directory, the makefile should exit
with no error.

When a test failed you can use "make continue" to run further tests.

--------------------------------------------------------------------------------

TODO:

- reduce usage of "common.h" to a minimum
- convert more tests from using reference output to returning an exit code
