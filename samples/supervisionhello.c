#include <supervision.h>
#include <peekpoke.h>

#define SV_OFFSET 0x0A
#define SV_INIT_POSITION (SV_VIDEO+0x0A*48+0x0A)

unsigned char h_char[] = {0x66,0x66,0x66,0x7e,0x66,0x66,0x66,0x00};
unsigned char e_char[] = {0x7e,0x60,0x60,0x78,0x60,0x60,0x7e,0x00};
unsigned char l_char[] = {0x60,0x60,0x60,0x60,0x60,0x60,0x7e,0x00};
unsigned char o_char[] = {0x3c,0x66,0x66,0x66,0x66,0x66,0x3c,0x00};
unsigned char w_char[] = {0x63,0x63,0x63,0x6b,0x7f,0x77,0x63,0x00};
unsigned char r_char[] = {0x7c,0x66,0x66,0x7c,0x78,0x6c,0x66,0x00};
unsigned char d_char[] = {0x78,0x6c,0x66,0x66,0x66,0x6c,0x78,0x00};


void clear_screen(void)
{
    unsigned short i;
    
    for(i=0;i<0x2000;++i)
    {
        POKE(SV_VIDEO+i,0);
    }
}

unsigned char bit_reverse_lookup[16] = 
{
    0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE,
    0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF 
};

unsigned char bit_reverse(unsigned char n) 
{
    return (bit_reverse_lookup[n&0b1111] << 4) | bit_reverse_lookup[n>>4];
}

void init_screen(void)
{
    SV_LCD.height = 0xA0;
    SV_LCD.width = 0xA0;
    SV_BANK = 0xC9;     
}

int main()
{
    unsigned char i;
    
    init_screen();
    
    clear_screen();
    
    for(i=0;i<8;++i)
    {
        POKE(SV_INIT_POSITION+1 +i*48,bit_reverse(h_char[i]));
        POKE(SV_INIT_POSITION+2 +i*48,bit_reverse(e_char[i]));
        POKE(SV_INIT_POSITION+3 +i*48,bit_reverse(l_char[i]));
        POKE(SV_INIT_POSITION+4 +i*48,bit_reverse(l_char[i]));
        POKE(SV_INIT_POSITION+5 +i*48,bit_reverse(o_char[i]));
        
        POKE(SV_INIT_POSITION+7 +i*48,bit_reverse(w_char[i]));
        POKE(SV_INIT_POSITION+8 +i*48,bit_reverse(o_char[i]));
        POKE(SV_INIT_POSITION+9 +i*48,bit_reverse(r_char[i]));
        POKE(SV_INIT_POSITION+10+i*48,bit_reverse(l_char[i]));
        POKE(SV_INIT_POSITION+11+i*48,bit_reverse(d_char[i]));        
    }

    while(1) {};
    
    return 0;
}


