
## cc65 floating point support

The current main goal is to implement IEEE754 support *in the compiler*, using
the "float" datatype as the common 32bit float.

*** ANYTHING ELSE COMES LATER ***

You can not use any of this to write software yet. Don't bother. This is for
people who want to help pushing the floating point support further.

You can however try the current state of development. You should be able to
build small (and slow...) programs that use floats on any supported target.

- Build the compiler/toolchain/libs from this fptest branch.
- Now you can build the samples and/or tests.

    samples/floattest.c
    samples/mandelfloat.c
    samples/mathtest.c (requires full math.h)
    samples/tgisincos.c (requires sin/cos from math.h)

full math.h is available for C64 when linking agains fp754kernal.o (see below)

### Further info

- Right now by default all targets will use the Berkeley Softfloat Library. This
  solves the annoying "chicken and egg" problem of having to implement the float
  support both in the compiler and in the target library at the same time, before
  anything can be tested properly. Fortunately that also means we can use the
  simulator for running test programs, and test changes in the compiler using
  our test bench, and that against a library that is known to somewhat work
  correctly :)
- The default library can be overridden by linking an override file, similar to
  how you can use the soft80 implementation for conio. Right now such override
  files are provided for the C64 (c64-fp754kernal.o) and VIC20
  (vic20-fp754kernal.o). The samples will automatically use the overrides.

### WANTED

- For the time being, i will not look at writing IEEE754 functions in assembly.
  Please see below for more info on what is needed to do this, should you be
  interested in doing this. Please contact me before you are putting work into
  this, so we can discuss some things and prevent anyone wasting time :)
- It might be possible to produce a similar kernal- or OS- wrapper override file
  as the C64 one for other targets (or port the C64 one to other CBM targets).
  - If you create a new one, keep in mind that the compiler *right now* will
    currently work with IEEE754 floats, which your library calls must also work
    with (which will involve converting forth and back to whatever other format
    at runtime), and there is no easy way tp change that.
- Similar to the softfloat library, it would be nice to have a complete 
  reference implementation for the math stuff in softmath as well.
  - Also the math stuff should be implemented in assembly at some point :)

### Roadmap

- Test/Fix using the Softfloat lib some more, fix as much tests as possible
- Find some more generic math functions that we can use in softmath
- When all obvious tests have been created and work OK, we can merge
  - for the failing tests, create "negative" cases

After the merge, the following things can be done more or less independent from
each other (not necessarily by me :)):

- implement IEEE754 library
  - for generic support this will be the best compromise for standard compliance
    and speed, but will take more RAM than alternatives.
  - Once implemented, it will work for all targets.

- implement support for native FP routines
  - Some targets have FP routines in ROM (or OS) that can be used instead of
    providing our own in the library. This may or may not save RAM, and may or
    may not be faster than the default library.
  - The wrapper library must implement conversion from/to 32bit IEEE754

- implement support for native FP formats
  - for this the wrappers in fp.c must be used in the compiler at all places.
  - also we must *implement* the native format on the host in fp.c
  - if the native format uses a different number of bytes than 4 for its native
    format, we must add support for this in the compiler at various places
  - add a cmdline option to the compiler to switch the float binary type (754,
    cbm, woz, ...)
  - last not least a wrapper library that uses the native format must be created
  - it is not unlikely that we will need extra tests for the native format

## The Compiler

NOT WORKING YET:

    /test/val/float-basic-var-var.c                  +=, -=

    /test/val/float-basic-var-intvar.c      -,       +=, -=
    /test/val/float-basic-var-intconst.c    -, *, /, +=, -=, *=, /=

    /test/val/float-basic-intvar-const.c    -,       +=, -=, *=, /=
    /test/val/float-basic-intvar-var.c      -, *, /, +=,         /=
    /test/val/float-basic-intconst-var.c       *, /

    /test/val/float-cmp-const-intvar.c
    /test/val/float-cmp-intvar-const.c
    /test/val/float-cmp-var-intvar.c

- float values written as "12.34f" work, but "12.34" does not - should it?

TODO:

- more tests are needed
    - warnings
    - errors
    - register variables
    - more/all tests should check local/global/register variables

### Files & Functions

#### assignment.c

    src/cc65/assignment.c

```
OpAssignArithmetic  Parse an "=" or "op=" operation for arithmetic lhs
OpAddSubAssign      Parse a "+=" or "-=" operation
```

#### codegen.c

    src/cc65/codegen.c
    src/cc65/codegen.h

```
g_getimmed          Load a constant into the primary register
g_getstatic         Fetch an static memory cell into the primary register
g_getlocal          Fetch specified local object (local var) into the primary register
g_putstatic
g_reglong
g_regfloat          Make sure, the value in the primary register a float. Convert if necessary
g_typeadjust
g_typecast          Cast the value in the primary register to the operand size that is flagged by the lhs value
oper                Encode a binary operation.
g_push
g_push_float        Push the primary register or a constant value onto the stack
g_inc
g_dec
g_defdata
g_defdata_float

(see below) g_add, g_sub, g_mul, g_div, g_neg, g_bneg
(see below) g_eq, g_ne, g_lt, g_gt, g_le, g_ge
(invalid) g_mod, g_or, g_xor, g_and, g_asr, g_asl
```
#### datatype.c

    src/cc65/datatype.c

```
ArithmeticConvert   Perform the usual arithmetic conversions for binary operators.
```

#### expr.c

    src/cc65/expr.c

```
LimitExprValue
parseadd
parsesub
```

#### fp.c

    src/common/fp.c         (OK)
    src/common/fp.h         (OK)

wrapper for doing floating point operations on target floats

```
FP_D_As32bitRaw     converts double into 32bit (float) and then returns its raw content as a 32bit int
```

#### initdata.c

    src/cc65/initdata.c

```
DefineData          Output a data definition for the given expression
```

#### loadexpr.c

    src/cc65/loadexpr.c

```
LoadExpr            Load an expression into the primary register if it is not already there.
```

#### locals.c

    src/cc65/locals.c       (OK?)

```
ParseAutoDecl       Parse the declarator of an auto variable.
```

#### scanner.c

    src/cc65/scanner.c     (OK?)

```
NumericConst        Parse a numeric constant
```

Note: Scanner fixes should be directly promoted to upstream. Any differences in
      this branch should be related to debugging/logging.

#### typeconv.c

    src/cc65/typeconv.c     (OK?)

```
DoConversion        Emit code to convert the given expression to a new type
```

--------------------------------------------------------------------------------

## The Library

### variants

The floating point support calls library functions for any operations on non
constant values.

#### softfloat

This is a Port of "Berkeley SoftFloat Release 2c". It is currently used by
default for all targets.

#### softmath

Contains a collection of math functions, hopefully some day enough to completely
implement math.h in C. This is currently used by default for all targets.

#### cbmkernal

This is a wrapper to the CBM kernal functions. This is fairly complete,
including math functions. To use this, link against c64-fp754kernal.o. The c64
samples will do this by default.

#### ieee754

This should become a freestanding IEEE754 library, which can completely replace
the softfloat (and softmath) library.

- basically everything missing except addition/substraction
  - compare functions are missing
  - mul, div functions are missing
  - type conversion functions are missing

#### woz

historical float routines by woz :) unfortunately not ieee754

- (optionally) converting from/to ieee754 format is missing (compile time option)
- compare functions are missing
- type conversion functions are missing

-> It probably doesn't make a lot of sense to spend time on supporting these,
   this directory should probably be deleted before merging with master

--------------------------------------------------------------------------------

### runtime functions

These must be available in the runtime library.
```
func        description                       softfloat cbmfp   wozfp   754     codegen.c

aufloat     Primary 8bit unsigned -> float      -       *       -       -       g_regfloat
afloat      Primary 8bit signed -> float        -       *       -       -       g_regfloat
axufloat    Primary 16bit unsigned -> float     *       *       -       -       g_regfloat
axfloat     Primary 16bit signed -> float       *       *       -       -       g_regfloat
eaxufloat   Primary 32bit unsigned -> float     *       *       -       -       g_regfloat
eaxfloat    Primary 32bit signed -> float       *       *       -       -       g_regfloat

feaxint     Primary float -> 16bit int          -       *       -       -       g_regint
feaxlong    Primary float -> 32bit long         -       *       -       -       g_reglong

ftosaddeax  Primary = TOS + Primary             *       *       ?       ?       g_add
ftossubeax  Primary = TOS - Primary             *       *       ?       ?       g_sub
ftosrsubeax Primary = Primary - TOS             *       *       -       -       g_rsub
ftosmuleax  Primary = TOS * Primary             *       *       ?       -       g_mul
ftosdiveax  Primary = TOS / Primary             *       *       ?       -       g_div

fnegeax     Primary = -Primary                  *       *       -       -       g_neg
fbnegeax    Primary = !Primary (return bool!)   *       *       -       -       g_bneg

ftosgeeax   Test for greater than or equal to   *       *       -       -       g_ge
ftosgteax   Test for greater than               *       *       -       -       g_gt
ftosleeax   Test for less than or equal to      *       *       -       -       g_le
ftoslteax   Test for less than                  *       *       -       -       g_lt
ftosneeax   Test for not equal                  *       *       -       -       g_ne
ftoseqeax   Test for equal                      *       *       -       -       g_eq
```
### math.h functions

These are optional, required for standard libm.
```
func        description                       softmath  cbmfp   wozfp   754

float powf(float f, float a)                    -       *       -       -
float sinf(float s)                             *       *       -       -
float cosf(float s)                             *       *       -       -
float logf(float x)                             -       *       *       -
float expf(float x)                             -       *       -       -
float sqrtf(float x)                            -       *       -       -
float tanf(float x)                             -       *       -       -
float atanf(float x)                            -       *       -       -
float fabsf(float x)                            *       *       -       -
float roundf(float x)                           *       *       -       -
float truncf(float x)                           *       *       -       -
float ceilf(float x)                            *       -       -       -
```

### extra functions

Optional utility functions.
```
func        description                       softfloat cbmfp   wozfp   754

char *_ftostr(char *d, float s)                 *       *       ?       ?       for printf family
float _strtof(char *d)                          -       *       -       -       for scanf family
```

--------------------------------------------------------------------------------

- https://www.geeksforgeeks.org/ieee-standard-754-floating-point-numbers/
- https://www.h-schmidt.net/FloatConverter/IEEE754.html
