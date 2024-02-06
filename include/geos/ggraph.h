/*
  GEOS graphic (non icon/menu/sprite) functions

  by Maciej 'YTM/Elysium' Witkowiak
*/

#ifndef _GGRAPH_H
#define _GGRAPH_H

#include <geos/gstruct.h>

void __fastcall__ SetPattern(char newpattern);

void __fastcall__ HorizontalLine(char pattern, char y, unsigned xstart, unsigned xend);
void __fastcall__ InvertLine(char y, unsigned xstart, unsigned xend);
void __fastcall__ RecoverLine(char y, unsigned xstart, unsigned xend);
void __fastcall__ VerticalLine(char pattern, char ystart, char yend, unsigned x);

void __fastcall__ InitDrawWindow(struct window *myRectangle);
void Rectangle(void);
void __fastcall__ FrameRectangle(char pattern);
void InvertRectangle(void);
void ImprintRectangle(void);
void RecoverRectangle(void);

void __fastcall__ DrawLine(char mode, struct window *topBotCoords);

void __fastcall__ DrawPoint(char mode, struct pixel *myPixel);
char __fastcall__ TestPoint(struct pixel *myPixel);

void __fastcall__ PutChar(char character, char y, unsigned x);
void __fastcall__ PutString(char *myString, char y, unsigned x);
void __fastcall__ PutDecimal(char style, unsigned value, char y, unsigned x);

char __fastcall__ GetCharWidth(char character);
void __fastcall__ LoadCharSet(struct fontdesc *myFont);
void UseSystemFont(void);

void __fastcall__ BitmapUp(struct iconpic *myIcon);
void __fastcall__ BitmapClip(char skipl, char skipr, unsigned skiptop,
                             struct iconpic *myIcon);
void __fastcall__ BitOtherClip(void *proc1, void *proc2, char skipl,
                               char skipr, unsigned skiptop,
                               struct iconpic *myIcon);

void __fastcall__ GraphicsString(const void *myGfxString);

#ifdef __GEOS_CBM__
void SetNewMode(void);
#endif

/* VIC colour constants */
#define BLACK           0
#define WHITE           1
#define RED             2
#define CYAN            3
#define PURPLE          4
#define GREEN           5
#define BLUE            6
#define YELLOW          7
#define ORANGE          8
#define BROWN           9
#define LTRED           10
#define DKGREY          11
#define GREY            12
#define MEDGREY         12
#define LTGREEN         13
#define LTBLUE          14
#define LTGREY          15
/* VIC memory banks - lowest 2 bits of cia2base+0 */
#define GRBANK0         3
#define GRBANK1         2
#define GRBANK2         1
#define GRBANK3         0
/* VIC screen sizes */
#define VIC_X_POS_OFF   24
#define VIC_Y_POS_OFF   50
#ifdef __GEOS_CBM__
#define SC_BYTE_WIDTH   40
#define SC_PIX_HEIGHT   200
#define SC_PIX_WIDTH    320
#define SC_SIZE         8000
#else
#define SC_BYTE_WIDTH   70
#define SC_PIX_HEIGHT   192
#define SC_PIX_WIDTH    560
#define SC_SIZE         13440
#endif
/* VDC screen constants */
#define SCREENBYTEWIDTH         80
#define SCREENPIXELWIDTH        640
/* control characters for use as numbers, not characters */
#define BACKSPACE       8
#define FORWARDSPACE    9
#define TAB             9
#define LF              10
#define HOME            11
#define PAGE_BREAK      12
#define UPLINE          12
#define CR              13
#define ULINEON         14
#define ULINEOFF        15
#define ESC_GRAPHICS    16
#define ESC_RULER       17
#define REV_ON          18
#define REV_OFF         19
#define GOTOX           20
#define GOTOY           21
#define GOTOXY          22
#define NEWCARDSET      23
#define BOLDON          24
#define ITALICON        25
#define OUTLINEON       26
#define PLAINTEXT       27
/* control characters for use in
   strings: eg: str[10]=CBOLDON "Hello"; */
#define CCR             "\015"
#define CULINEON        "\016"
#define CULINEOFF       "\017"
#define CREV_ON         "\022"
#define CREV_OFF        "\023"
#define CBOLDON         "\030"
#define CITALICON       "\031"
#define COUTLINEON      "\032"
#define CPLAINTEXT      "\033"

/*values of currentMode */
/* bitNumbers */
#define UNDERLINE_BIT   7
#define BOLD_BIT        6
#define REVERSE_BIT     5
#define ITALIC_BIT      4
#define OUTLINE_BIT     3
#define SUPERSCRIPT_BIT 2
#define SUBSCRIPT_BIT   1
/* bitMasks */
#define SET_UNDERLINE   0x80
#define SET_BOLD        0x40
#define SET_REVERSE     0x20
#define SET_ITALIC      0x10
#define SET_OUTLINE     0x08
#define SET_SUPERSCRIPT 0x04
#define SET_SUBSCRIPT   0x02
#define SET_PLAINTEXT   0
/* values of dispBufferOn */
#define ST_WRGS_FORE    0x20
#define ST_WR_BACK      0x40
#define ST_WR_FORE      0x80
/* PutDecimal parameters */
/* leading zeros? */
#define SET_NOSURPRESS  0
#define SET_SURPRESS    0x40
/* justification */
#define SET_RIGHTJUST   0
#define SET_LEFTJUST    0x80
/* C128 x-extension flags */
#define ADD1_W          0x2000
#define DOUBLE_B        0x80
#define DOUBLE_W        0x8000
/* DrawLine/DrawPoint mode values */
#define DRAW_ERASE      0x00
#define DRAW_DRAW       0x40
#define DRAW_COPY       0x80

typedef void graphicStr;

#define MOVEPENTO(x,y) (char)1, (unsigned)(x), (char)(y)
#define LINETO(x,y) (char)2, (unsigned)(x), (char)(y)
#define RECTANGLETO(x,y) (char)3, (unsigned)(x), (char)(y)
#define NEWPATTERN(p) (char)5, (char)(p)
#define FRAME_RECTO(x,y) (char)7, (unsigned)(x), (char)(y)
#define PEN_X_DELTA(x) (char)8, (unsigned)(x)
#define PEN_Y_DELTA(y) (char)9, (char)(y)
#define PEN_XY_DELTA(x,y) (char)10, (unsigned)(x), (char)(y)
#define GSTR_END (char)NULL
/* ESC_PUTSTRING can't be implemented - it needs text, not pointer to it
   #define ESC_PUTSTRING(x,y,text) (char)6, (unsigned)(x), (char)(y), (text), (char)NULL
*/

#endif
