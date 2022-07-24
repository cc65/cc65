/*****************************************************************************/
/*                                                                           */
/* Watara Supervision sample C program                                       */
/*                                                                           */
/* Fabrizio Caruso (fabrizio_caruso@hotmail.com), 2019                       */
/* Greg King       (greg.king5@verizon.net), 2021                            */
/*                                                                           */
/*****************************************************************************/

#include <supervision.h>
#include <string.h>

/* Number of words per screen line (Remark: Last 4 words aren't displayed) */
#define WORDS_PER_LINE (160/8+4)

struct sv_vram {
    unsigned int v[160/8][8][WORDS_PER_LINE];
};
#define SV_VRAM ((*(struct sv_vram *)0x4000).v)

/* Character definitions in 8x8 format */
/* That format gives us a screen of 20 columns and 20 rows */
static const unsigned char h_char[] = {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00};
static const unsigned char e_char[] = {0x7E,0x60,0x60,0x78,0x60,0x60,0x7E,0x00};
static const unsigned char l_char[] = {0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00};
static const unsigned char o_char[] = {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00};
static const unsigned char w_char[] = {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00};
static const unsigned char r_char[] = {0x7C,0x66,0x66,0x7C,0x78,0x6C,0x66,0x00};
static const unsigned char d_char[] = {0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00};

static void clear_screen(void)
{
    memset(SV_VIDEO, 0, 0x2000);
}

/* Necessary conversion to have 2 bits per pixel with darkest hue */
/* Remark: The Supervision uses 2 bits per pixel, and bits are mapped into pixels in reversed order */
static unsigned int __fastcall__ double_reversed_bits(unsigned char)
{
    __asm__("stz ptr2");
    __asm__("stz ptr2+1");
    __asm__("ldy #$08");
L1: __asm__("lsr a");
    __asm__("php");
    __asm__("rol ptr2");
    __asm__("rol ptr2+1");
    __asm__("plp");
    __asm__("rol ptr2");
    __asm__("rol ptr2+1");
    __asm__("dey");
    __asm__("bne %g", L1);
    __asm__("lda ptr2");
    __asm__("ldx ptr2+1");
    return __AX__;
}

static void display_char(const unsigned char x, const unsigned char y, const unsigned char *ch)
{
    unsigned char k;

    for(k=0;k<8;++k)
    {
        SV_VRAM[y][k][x] = double_reversed_bits(ch[k]);
    }
}

static void init_lcd(void)
{
    SV_LCD.width = 160;
    SV_LCD.height = 160;
}

static void hello(unsigned char x, unsigned char y)
{
    display_char(x+ 0,y,h_char);
    display_char(x+ 1,y,e_char);
    display_char(x+ 2,y,l_char);
    display_char(x+ 3,y,l_char);
    display_char(x+ 4,y,o_char);

    display_char(x+ 6,y,w_char);
    display_char(x+ 7,y,o_char);
    display_char(x+ 8,y,r_char);
    display_char(x+ 9,y,l_char);
    display_char(x+10,y,d_char);
}

void main(void)
{
    init_lcd();
    clear_screen();

    hello(2,3);
    hello(7,16);
}
