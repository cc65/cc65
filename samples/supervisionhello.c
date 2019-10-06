#include <supervision.h>
#include <peekpoke.h>

#define BYTES_PER_LINE 48

const unsigned char h_char[] = {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00};
const unsigned char e_char[] = {0x7E,0x60,0x60,0x78,0x60,0x60,0x7E,0x00};
const unsigned char l_char[] = {0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00};
const unsigned char o_char[] = {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00};
const unsigned char w_char[] = {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00};
const unsigned char r_char[] = {0x7C,0x66,0x66,0x7C,0x78,0x6C,0x66,0x00};
const unsigned char d_char[] = {0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00};

void clear_screen(void)
{
    unsigned short i;
    
    for(i=0;i<0x2000;++i)
    {
        POKE(SV_VIDEO+i,0);
    }
}

unsigned char reversed_map_one_to_two_lookup[16] = 
{
    0x00, 0xC0, 0x30, 0xF0, 0x0C, 0xCC, 0x3C, 0xFC,
    0x03, 0xC3, 0x33, 0xF3, 0x0F, 0xCF, 0x3F, 0xFF
};

unsigned char left_map_one_to_two(unsigned char n)
{
    return reversed_map_one_to_two_lookup[n >> 4];
}

unsigned char right_map_one_to_two(unsigned char n)
{
    return reversed_map_one_to_two_lookup[n&0x0F];
}

void display_char(const unsigned char x, const unsigned char y, const unsigned char *ch)
{
    unsigned char k;
    
    for(k=0;k<8;++k)
    { \
        SV_VIDEO[2*(y)+BYTES_PER_LINE*k+BYTES_PER_LINE*(x<<3)]    = left_map_one_to_two(ch[k]); 
        SV_VIDEO[2*(y)+BYTES_PER_LINE*k+BYTES_PER_LINE*(x<<3)+1]  = right_map_one_to_two(ch[k]); 
    } 
}

int main()
{    
    clear_screen();
    
    display_char(3,2, h_char);
    display_char(3,3, e_char);
    display_char(3,4, l_char);
    display_char(3,5, l_char);
    display_char(3,6, o_char);
    
    display_char(3,8, w_char);
    display_char(3,9, o_char);
    display_char(3,10,r_char);
    display_char(3,11,l_char);
    display_char(3,12,d_char);    

    while(1) {};
    
    return 0;
}


