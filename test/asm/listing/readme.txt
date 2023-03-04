Overall test:
-------------

These testcases can be used to test different aspects of the assembler.
The name of a test is everything in the form <test>.s.

The following reference files can be added:

- ref/<test>.bin-ref:
  This is a reference for the resulting binary.
  The binary as binary tested against this file.
  If they are not equal, the test fails.

- ref/<test>.list-ref
  This is a reference for the resulting listing output
  This file *must* have the first line of the listing removed, as that
  contains a ca65 version string, and almost always this will be changed!

- ref/<test>.err-ref
  This is a reference for the resulting ca65 stdout (>) output.

- ref/<test>.err2-ref
  This is a reference for the resulting ca65 stderr (2>) output.

- ref/<test>.ld65err-ref
  This is a reference for the resutling ld65 stdout (>) output.

- ref/<test>.ld65err2-ref
  This is a reference for the resulting ld65 stderr (2>) output.

The following control files can be added to control the tests.
These files are empty (contents ignored), and only the filename is used:

- control/<test>.err
  Test is expected to produce an error.

- control/<test>.no-ld65
  Skip the ld65 step.


Note that the resulting .bin file is generated twice: Once with no listing
file, and once with listing file. This way, one can find out if the listing
file generation changes anything with the resulting binary output.
