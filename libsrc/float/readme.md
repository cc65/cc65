
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

math.h is available for C64 when linking agains fp754kernal.o (see below)

Related to fp stuff are:

    samples/floattest.c
    samples/mandelfloat.c
    samples/mathtest.c (requires math.h)
    samples/tgisincos.c (requires math.h)

    test/val/float-basic-const-const.c
    test/val/float-basic-const-intconst.c
    test/val/float-basic-const-intvar.c
    test/val/float-basic-const-var.c
    test/val/float-basic-intconst-const.c
    test/val/float-basic-intconst-var.c
    test/val/float-basic-intvar-const.c
    test/val/float-basic-intvar-var.c
    test/val/float-basic-var-const.c
    test/val/float-basic-var-intconst.c
    test/val/float-basic-var-intvar.c
    test/val/float-basic-var-var.c
    test/val/float-bnegate.c
    test/val/float-cmp-const-const.c
    test/val/float-cmp-const-intconst.c
    test/val/float-cmp-const-intvar.c
    test/val/float-cmp-const-var.c
    test/val/float-cmp-intconst-const.c
    test/val/float-cmp-intconst-var.c
    test/val/float-cmp-intvar-const.c
    test/val/float-cmp-intvar-var.c
    test/val/float-cmp-var-const.c
    test/val/float-cmp-var-intconst.c
    test/val/float-cmp-var-intvar.c
    test/val/float-cmp-var-var.c
    test/val/float-const-convert.c
    test/val/float-conv.c
    test/val/float-conv-float-to-char.c
    test/val/float-conv-float-to-schar.c
    test/val/float-minimal.c
    test/val/float-mixed.c
    test/val/float-negate.c
    test/val/float-ternary.c

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
  files are provided for the C64 and VIC20 (fp754kernal.o). The samples will
  automatically use the overrides.

### Wanted

- For the time being, i will not look at writing IEEE754 functions in assembly.
  Please see below for more info on what is needed to do this, should you be
  interested in doing this.
- It might be possible to produce a similar kernal- or OS- wrapper override file
  for other targets (or port the CBM one to other CBM targets).
  - If you create a new one, keep in mind that the compiler *right now* will
    currently work with IEEE754 floats, which your library calls must also work
    with (which will involve converting forth and back to whatever other format
    at runtime)

### Roadmap

- Test/Fix using the Softfloat lib some more
- When all obvious tests have been created and work OK, we can merge

After the merge, the following things can be done more or less independent from
each other (not necessarily by me :)):

- implement ieee754 library
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

    /test/val/float-basic-const-intvar.c (subtraction)
    /test/val/float-basic-const-intconst.c (addition, subtraction)
    /test/val/float-basic-intconst-var.c (addition, subtraction, multiplication, division)
    /test/val/float-basic-intconst-const.c (addition, subtraction)
    /test/val/float-basic-intvar-const.c (subtraction)
    /test/val/float-basic-intvar-var.c (subtraction, multiplication, division)
    /test/val/float-basic-var-intconst.c (subtraction, multiplication, division)
    /test/val/float-basic-var-intvar.c (subtraction)

    /test/val/float-cmp-const-intvar.c
    /test/val/float-cmp-intvar-const.c
    /test/val/float-cmp-var-intvar.c

- float values written as "12.34f" work, but "12.34" does not - should it?

### Files & Functions

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
ArithmeticConvert
```

#### expr.c

    src/cc65/expr.c

```
LimitExprValue
parseadd
parsesub
```

#### initdata.c

    src/cc65/initdata.c

```
DefineData          Output a data definition for the given expression
```

#### loadexpr.c

    src/cc65/loadexpr.c

```
LoadExpr
```

#### locals.c

    src/cc65/locals.c

```
ParseAutoDecl
```

#### assignment.c

    src/cc65/assignment.c

```
OpAssignArithmetic  Parse an "=" (if 'Gen' is 0) or "op=" operation for arithmetic lhs
```

#### fp.c

    src/common/fp.c
    src/common/fp.h

wrapper for doing floating point operations on target floats

```
FP_D_As32bitRaw     converts double into 32bit (float) and then returns its raw content as a 32bit int
```

#### typeconv.c

    src/cc65/typeconv.c

```
DoConversion        Emit code to convert the given expression to a new type
```

#### scanner.c

    src/cc65/scanner.c

Note: Scanner fixes should be directly promoted to upstream. Any differences in
      this branch should be related to debugging/logging.

--------------------------------------------------------------------------------

## The Library

### variants

The floating point support calls library functions for any operations on non
constant values

#### softfloat

This is a Port of "Berkeley SoftFloat Release 2c". It is currently used by
default for all targets.

- missing are all math functions (we might port another existing lib for that)

#### cbmkernal

This is a wrapper to the CBM kernal functions.

- this one is fairly complete, including math functions
- the only missing function is ftosrsubeax
  - WANTED: which can be easily added once testcode is found that actually triggers it :)

#### ieee754

This should become a freestanding IEEE754 library, which can completely replace
the softfloat library.

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

these must be available in the runtime library
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
ftosrsubeax Primary = Primary - TOS             -       -       -       -       g_rsub
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
### extra functions

optional utility functions.
```
func        description                       softfloat cbmfp   wozfp   754

char *_ftostr(char *d, float s)                 *       *       ?       ?       for printf family
float _strtof(char *d)                          -       *       -       -       for scanf family
```
### math.h functions

these are optional, required for standard libm
```
func        description                       softfloat cbmfp   wozfp   754

/* C99 */
float powf(float f, float a)                    -       *       -       -
float sinf(float s)                             -       *       -       -
float cosf(float s)                             -       *       -       -
float logf(float x)                             -       *       *       -
float expf(float x)                             -       *       -       -
float sqrtf(float x)                            -       *       -       -
float tanf(float x)                             -       *       -       -
float atanf(float x)                            -       *       -       -
float fabsf(float x)                            -       *       -       -
float roundf(float x)                           -       *       -       -
float truncf(float x)                           -       *       -       -
```
--------------------------------------------------------------------------------

- https://www.geeksforgeeks.org/ieee-standard-754-floating-point-numbers/
- https://www.h-schmidt.net/FloatConverter/IEEE754.html
