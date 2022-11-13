
## cc65 floating point support

The current goal is to implement ieee754 support in the compiler, using the
"float" datatype as the common 32bit float. ANYTHING ELSE COMES LATER

You can not use any of this to write software yet. Dont bother. This is for
people who want to help pushing the floating point support further.

- build the compiler/toolchain/libs from this fptest branch
- now you can build the programs in this directory

right now you'll have to use the cbm kernal wrapper library, as that is pretty
much the only one that somewhat works :)

feel free to work on "real" ieee754 functions (see below)

## The Compiler

- for the time being i will handle and test only expressions where left and
  right side are both floats. that will enable me to fix and test a fair portion
  of what has to be done, before i will have to dive into the ugly areas of type
  conversion and casting.

NOT WORKING YET:

- float values as in "12.34f" work, but "12.34" does not - should it?

- addition, float const + float var
- substraction, float const - float var
- division, float const / float var

- substraction, float const - float const (Invalid operands for binary operator)
- substraction, float var - float const (Invalid operands for binary operator)

(and probably more :))

TODO (much later):

- add a cmdline option to the compiler to switch the float binary type (754, cbm,
  woz, ...). -> remember the code in fp.c/h

### Files & Functions

#### codegen.c
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
(invalid) g_mod, g_or, g_xor, g_and, g_asr
```
#### datatype.c

```
ArithmeticConvert
```

#### expr.c

```
LimitExprValue
parseadd
parsesub
```

#### initdata.c

```
DefineData          Output a data definition for the given expression
```

#### loadexpr.c

```
LoadExpr
```

#### locals.c

```
ParseAutoDecl
```

#### assignment.c

```
OpAssignArithmetic  Parse an "=" (if 'Gen' is 0) or "op=" operation for arithmetic lhs
```

#### fp.c

wrapper for doing floating point operations on target floats

```
FP_D_As32bitRaw     converts double into 32bit (float) and then returns its raw content as a 32bit int
```

--------------------------------------------------------------------------------

## The Library

cbmkernal:

this is a wrapper to the CBM kernal functions

- this one is fairly complete and should be OK to use when fixing/adding basic
  things in the compiler
- the only missing functions are ftosrsubeax, fnegeax - which can be easily
  added once testcode is found that actually triggers it :)

ieee754:

this should become a freestanding ieee754 library

- basically everything missing except addition/substraction
- compare functions are missing
- mul, div functions are missing
- type conversion functions are missing

woz:

historical float routines by woz :) unfortunately not ieee754

- (optionally) converting from/to ieee754 format is missing (compile time option)
- compare functions are missing
- type conversion functions are missing

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

fnegeax     Primary = -Primary                  -       -       -       -       g_neg
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
