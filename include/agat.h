#ifndef _AGAT_H
#define _AGAT_H

/* Check for errors */
#if !defined(__AGAT__)
#  error This module may only be used when compiling for the Agat!
#endif


/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Color defines */
#define COLOR_BLACK    0x00
#define COLOR_RED      0x01
#define COLOR_GREEN    0x02
#define COLOR_YELLOW   0x03
#define COLOR_BLUE     0x04
#define COLOR_MAGENTA  0x05
#define COLOR_CYAN     0x06
#define COLOR_WHITE    0x07

/* Characters codes */
#define CH_CTRL_C      0x03
#define CH_ENTER       0x0D
#define CH_ESC         0x1B
#define CH_CURS_LEFT   0x08
#define CH_CURS_RIGHT  0x15
#define CH_CURS_UP     0x19
#define CH_CURS_DOWN   0x1A
#define CH_ESC         0x1B
#define CH_HLINE       0x1B
#define CH_VLINE       0x5C
#define CH_ULCORNER    0x10
#define CH_URCORNER    0x12
#define CH_LLCORNER    0x1D
#define CH_LRCORNER    0x1F

/* Masks for joy_read */
#define JOY_UP_MASK    0x10
#define JOY_DOWN_MASK  0x20
#define JOY_LEFT_MASK  0x04
#define JOY_RIGHT_MASK 0x08
#define JOY_BTN_1_MASK 0x40
#define JOY_BTN_2_MASK 0x80

/* Return codes for get_ostype */
#define AGAT_UNKNOWN   0x00
#define AGAT_7         0x10  /* Agat 7 */
#define AGAT_9         0x20  /* Agat 9 */


/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/

unsigned char get_ostype (void);
/* Get the machine type. Returns one of the AGAT_xxx codes. */

void rebootafterexit (void);
/* Reboot machine after program termination has completed. */

/* The following #defines will cause the matching functions calls in conio.h
** to be overlaid by macros with the same names, saving the function call
** overhead.
*/
#define _bgcolor(color)         COLOR_BLACK
#define _bordercolor(color)     COLOR_BLACK

/* End of agat.h */


#endif
