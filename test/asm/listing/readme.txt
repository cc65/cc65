Overall test:
-------------

These testcases can be used to test different aspects of the assembler.
The name of a test is everything in the form <test>.s.

The following reference files can be added:

- <test>.bin-ref:
  This is a reference for the resulting binary.
  The binary as binary tested against this file.
  If they are not equal, the test fails.

- <test>.list-ref
  This is a reference for the resulting listing output
  This file *must* have the first line of the listing removed, as that
  contains a ca65 version string, and almost always this will be changed!


Note that the resulting .bin file is generated twice: Once with no listing
file, and once with listing file. This way, one can find out if the listing
file generation changes anything with the resulting binary output.


TODO:
- add the possibility to test for specific error output that are to be
  expected
