
Package Overview for Berkeley SoftFloat Release 2c

John R. Hauser
2015 January 30


----------------------------------------------------------------------------
Overview

Berkeley SoftFloat is a software implementation of binary floating-point
that conforms to the IEEE Standard for Floating-Point Arithmetic.
Release 2c updates an older version of SoftFloat that has for most purposes
been supplanted by Release 3 or later.  For the latest version of SoftFloat,
see Web page `http://www.jhauser.us/arithmetic/SoftFloat.html'.

SoftFloat is distributed in the form of C source code.  For Release 2c,
compiling the SoftFloat sources generates two things:

-- A SoftFloat object file (typically `softfloat.o') containing the complete
   set of IEC/IEEE floating-point routines.

-- A `timesoftfloat' program for evaluating the speed of the SoftFloat
   routines.  (The SoftFloat module is linked into this program.)

This version of the SoftFloat package is documented in four text files:

   SoftFloat.txt          Documentation for using the SoftFloat functions.
   SoftFloat-source.txt   Documentation for compiling SoftFloat.
   SoftFloat-history.txt  History of major changes to SoftFloat.
   timesoftfloat.txt      Documentation for using `timesoftfloat'.

Other files in the package comprise the source code for SoftFloat.

Please be aware that some work is involved in porting this software to other
targets.  It is not just a matter of getting `make' to complete without
error messages.  You should not attempt to compile this release of SoftFloat
without first reading both `SoftFloat.txt' and `SoftFloat-source.txt'.
Depending on your needs, you may find that newer versions of SoftFloat are
less work to port.


----------------------------------------------------------------------------
Legal Notice

SoftFloat was written by John R. Hauser.  Release 2c of SoftFloat was made
possible in part by the International Computer Science Institute, located
at Suite 600, 1947 Center Street, Berkeley, California 94704.  Funding
was partially provided by the National Science Foundation under grant
MIP-9311980.  The original version of this code was written as part of a
project to build a fixed-point vector processor in collaboration with the
University of California at Berkeley, overseen by Profs. Nelson Morgan and
John Wawrzynek.

THIS SOFTWARE IS DISTRIBUTED AS IS, FOR FREE.  Although reasonable effort
has been made to avoid it, THIS SOFTWARE MAY CONTAIN FAULTS THAT WILL AT
TIMES RESULT IN INCORRECT BEHAVIOR.  USE OF THIS SOFTWARE IS RESTRICTED TO
PERSONS AND ORGANIZATIONS WHO CAN AND WILL TOLERATE ALL LOSSES, COSTS, OR
OTHER PROBLEMS THEY INCUR DUE TO THE SOFTWARE WITHOUT RECOMPENSE FROM JOHN
HAUSER OR THE INTERNATIONAL COMPUTER SCIENCE INSTITUTE, AND WHO FURTHERMORE
EFFECTIVELY INDEMNIFY JOHN HAUSER AND THE INTERNATIONAL COMPUTER SCIENCE
INSTITUTE (possibly via similar legal notice) AGAINST ALL LOSSES, COSTS, OR
OTHER PROBLEMS INCURRED BY THEIR CUSTOMERS AND CLIENTS DUE TO THE SOFTWARE,
OR INCURRED BY ANYONE DUE TO A DERIVATIVE WORK THEY CREATE USING ANY PART OF
THE SOFTWARE.

The following are expressly permitted, even for commercial purposes:
(1) distribution of SoftFloat in whole or in part, as long as this and
other legal notices remain and are prominent, and provided also that, for a
partial distribution, prominent notice is given that it is a subset of the
original; and
(2) inclusion or use of SoftFloat in whole or in part in a derivative
work, provided that the use restrictions above are met and the minimal
documentation requirements stated in the source code are satisfied.


----------------------------------------------------------------------------
Contact Information

At the time of this writing, the most up-to-date information about SoftFloat
and the latest release can be found at the Web page `http://www.jhauser.us/
arithmetic/SoftFloat.html'.

