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
        Normally the reference output is produced by compiling the program on the
        host (using gcc mostly) and then running them on the host. Tests should
        be tweaked to produce the same output as on the host in the cases where
        it would be different.

        The Makefile also handles some special cases (add the tests to the
        respective list in the makefile):

        - Sometimes we want to check the warnings produced by the compiler. In
        that case use the CUSTOMSOURCES list. Whatever output the compiler writes
        to stderr will be compared against the matching .cref file. There is an
        example in custom-reference.c/.cref

        - Sometimes we want to check what kind of output the compiler produces
        for a file that does not compile. In that case use the ERRORSOURCES list.
        There is an example in custom-reference-error.c/.cref

        Warning: please understand that comparing the compiler output against
        a reference produces a moving target, ie the tests may break randomly
        at any time when the compiler output changes for whatever reason. So
        only ever use this as a last resort when something can not be tested by
        other means.

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


These tests only require a subset of the platform libraries. In the (top)
directory above this one, "make libtest" can be used to build only those
libraries needed for testing, instead of "make lib".

To run the tests use "make" in this (test) directory, the makefile should exit
with no error.

When a test failed you can use "make continue" to run further tests.

--------------------------------------------------------------------------------

TODO:

- reduce usage of "common.h" to a minimum
- convert more tests from using reference output to returning an exit code
