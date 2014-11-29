#define CH_CROSS	0x10

#define CH_RTEE     0x17
#define CH_LTEE     0x0f

#define CH_ULCORNER	0x10
#define CH_URCORNER	0x10
#define CH_LLCORNER	0x10
#define CH_LRCORNER	0x10


#define TV_NTSC 	0
#define TV_PAL  	1
#define get_tv()	0

#define CLOCKS_PER_SEC		50		// ???
#define CLK_TCK				50		// ?!?

//#ifndef CH_ENTER
#define CH_ENTER	'\n'
//#endif

#define CH_STOP 	0x08

#define CH_F1   	0x14
#define CH_F3   	0x15
#define CH_F5   	0x16
#define CH_F7   	0x17

#define CH_CURS_UP		0x01
#define CH_CURS_DOWN	0x02

#ifndef CH_CURS_LEFT
#define CH_CURS_LEFT	0x03
#endif

#ifndef CH_CURS_RIGHT
#define CH_CURS_RIGHT	0x04
#endif

#define CH_ESC		8

#define CH_DEL		20

/* Color defines */
#define COLOR_BLACK  	       	0x00
#define COLOR_WHITE  	       	0x01
#define COLOR_RED    	       	0x02
#define COLOR_CYAN      		0x03
#define COLOR_VIOLET 	       	0x04
#define COLOR_GREEN  	       	0x05
#define COLOR_BLUE   	       	0x06
#define COLOR_YELLOW 	       	0x07
#define COLOR_ORANGE 	       	0x08
#define COLOR_BROWN  	       	0x09
#define COLOR_LIGHTRED       	0x0A
#define COLOR_GRAY1  	       	0x0B
#define COLOR_GRAY2  	       	0x0C
#define COLOR_LIGHTGREEN     	0x0D
#define COLOR_LIGHTBLUE      	0x0E
#define COLOR_GRAY3  	       	0x0F

#define JOY_FIRE_B  5
#define JOY_START   6
#define JOY_SELECT  7

/*
void __fastcall__ waitvblank(void);

unsigned char __fastcall__ cpeekcharxy(unsigned char x,unsigned char y);
unsigned char __fastcall__ cpeekchar(void);
unsigned char __fastcall__ cpeekcolxy(unsigned char x,unsigned char y);
unsigned char __fastcall__ cpeekcol(void);
*/
