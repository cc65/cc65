
/*============================================================================

This C header file template is part of the Berkeley SoftFloat IEEE Floating-
Point Arithmetic Package, Release 2c, by John R. Hauser.

THIS SOFTWARE IS DISTRIBUTED AS IS, FOR FREE.  Although reasonable effort has
been made to avoid it, THIS SOFTWARE MAY CONTAIN FAULTS THAT WILL AT TIMES
RESULT IN INCORRECT BEHAVIOR.  USE OF THIS SOFTWARE IS RESTRICTED TO PERSONS
AND ORGANIZATIONS WHO CAN AND WILL TOLERATE ALL LOSSES, COSTS, OR OTHER
PROBLEMS THEY INCUR DUE TO THE SOFTWARE WITHOUT RECOMPENSE FROM JOHN HAUSER OR
THE INTERNATIONAL COMPUTER SCIENCE INSTITUTE, AND WHO FURTHERMORE EFFECTIVELY
INDEMNIFY JOHN HAUSER AND THE INTERNATIONAL COMPUTER SCIENCE INSTITUTE
(possibly via similar legal notice) AGAINST ALL LOSSES, COSTS, OR OTHER
PROBLEMS INCURRED BY THEIR CUSTOMERS AND CLIENTS DUE TO THE SOFTWARE, OR
INCURRED BY ANYONE DUE TO A DERIVATIVE WORK THEY CREATE USING ANY PART OF THE
SOFTWARE.

Derivative works require also that (1) the source code for the derivative work
includes prominent notice that the work is derivative, and (2) the source code
includes prominent notice of these three paragraphs for those parts of this
code that are retained.

=============================================================================*/

/*----------------------------------------------------------------------------
| Software IEEE floating-point types.
*----------------------------------------------------------------------------*/
typedef unsigned long float32;
typedef struct {
    unsigned long high, low;
} float64;

/*----------------------------------------------------------------------------
| Software IEEE floating-point underflow tininess-detection mode.
*----------------------------------------------------------------------------*/
extern signed char float_detect_tininess;
enum {
    float_tininess_after_rounding  = 0,
    float_tininess_before_rounding = 1
};

/*----------------------------------------------------------------------------
| Software IEEE floating-point rounding mode.
*----------------------------------------------------------------------------*/
extern signed char float_rounding_mode;
enum {
    float_round_nearest_even = 0,
    float_round_to_zero      = 1,
    float_round_down         = 2,
    float_round_up           = 3
};

/*----------------------------------------------------------------------------
| Software IEEE floating-point exception flags.
*----------------------------------------------------------------------------*/
extern signed char float_exception_flags;
enum {
    float_flag_inexact   =  1,
    float_flag_underflow =  2,
    float_flag_overflow  =  4,
    float_flag_divbyzero =  8,
    float_flag_invalid   = 16
};

/*----------------------------------------------------------------------------
| Routine to raise any or all of the software IEEE floating-point exception
| flags.
*----------------------------------------------------------------------------*/
void float_raise( signed char );

/*----------------------------------------------------------------------------
| Software IEEE integer-to-floating-point conversion routines.
*----------------------------------------------------------------------------*/
float32 int32_to_float32( int32 );
float64 int32_to_float64( int32 );

/*----------------------------------------------------------------------------
| Software IEEE single-precision conversion routines.
*----------------------------------------------------------------------------*/
int32 float32_to_int32( float32 );
int32 float32_to_int32_round_to_zero( float32 );
float64 float32_to_float64( float32 );

/*----------------------------------------------------------------------------
| Software IEEE single-precision operations.
*----------------------------------------------------------------------------*/
float32 float32_round_to_int( float32 );
float32 float32_add( float32, float32 );
float32 float32_sub( float32, float32 );
float32 float32_mul( float32, float32 );
float32 float32_div( float32, float32 );
float32 float32_rem( float32, float32 );
float32 float32_sqrt( float32 );
unsigned char float32_eq( float32, float32 );
unsigned char float32_le( float32, float32 );
unsigned char float32_lt( float32, float32 );
unsigned char float32_eq_signaling( float32, float32 );
unsigned char float32_le_quiet( float32, float32 );
unsigned char float32_lt_quiet( float32, float32 );
unsigned char float32_is_signaling_nan( float32 );

/*----------------------------------------------------------------------------
| Software IEEE double-precision conversion routines.
*----------------------------------------------------------------------------*/
signed short float64_to_int32( float64 );
signed short float64_to_int32_round_to_zero( float64 );
float32 float64_to_float32( float64 );

/*----------------------------------------------------------------------------
| Software IEEE double-precision operations.
*----------------------------------------------------------------------------*/
float64 float64_round_to_int( float64 );
float64 float64_add( float64, float64 );
float64 float64_sub( float64, float64 );
float64 float64_mul( float64, float64 );
float64 float64_div( float64, float64 );
float64 float64_rem( float64, float64 );
float64 float64_sqrt( float64 );
unsigned char float64_eq( float64, float64 );
unsigned char float64_le( float64, float64 );
unsigned char float64_lt( float64, float64 );
unsigned char float64_eq_signaling( float64, float64 );
unsigned char float64_le_quiet( float64, float64 );
unsigned char float64_lt_quiet( float64, float64 );
unsigned char float64_is_signaling_nan( float64 );

