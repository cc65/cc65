#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>

#define SCREENRAM ((unsigned char*)0x400)

/* move cursor back one char with 
** recognition of row change
*/   
void chBack(void){
    signed char x, y;
    
    x = wherex();
    y = wherey();
    
    x-=1;
    if (x < 0) {
        x=39;
        y-=1;
    }
    gotoxy(x,y);
}



/* move cursor forth one char with 
** recognition of row change
*/
void chForth(void){
    signed char x, y;
    
    x = wherex();
    y = wherey();
    
    x+=1;
    if (x >= 40) {
        x=0;
        y+=1;
    }
    gotoxy(x,y);
}



/* simple hack to get unmodified byte from screen memory 
** at current cursor position 
*/
unsigned char peekChWithoutTranslation(void){
    return SCREENRAM[wherex()+40*wherey()];
}



/* test which outputs every possible char, reads it back 
** from screen memory, outputs again at the next char position
** and compares the two screen memory bytes on identity 
*/
int testCPeekC(char ch){

    /* Test the given charcode on special characters NEWLINE,LINEFEED and
    ** TAB
    */
    if ( ('\n' !=ch) && ('\r' !=ch) && ('\t' != ch) ){ 
        /* go on if not special char according to prev comment */
        unsigned char ch2_a, ch2_b;
        
        /* output char to the screen */
        cputc(ch);
        
        /* move cursorpos to previous output */
        chBack();
        /* get back written char including the translation scrcode->asc */
        ch2_a = cpeekc();
        /* get back written char without translation */
        ch2_b = peekChWithoutTranslation();
        
        /* move cursor to following writing position */
        chForth();
        /* output again the char which was read back by cpeekc */
        cputc(ch2_a);
        
        /* move cursorpos to previous output */
        chBack();
        
        /* get back second written char without translation and compare it to
        ** the previously written char without translation 
        */
        if ( peekChWithoutTranslation() == ch2_b ){
            /* test succesful
             * move cursor to following writing position 
             */
            chForth();
            return EXIT_SUCCESS;
        }
        /* test NOT succesful
        ** output diagnostic and return FAILURE
        */
        cprintf("\n\rError on char: 0x%x. was 0x%x instead", ch, ch2_a);
    
        return EXIT_FAILURE;
    }
    cputc('.');
    cputc('.');
    return EXIT_SUCCESS;
}



/* The main code inits the screen for the tests and sets revers flag.
** Then it calls testCPeekC for every char within 0..255.
** On failure the failed char+1000 is returned to the system 
*/
int main()
{
    int i = 0;

    clrscr();
    revers(0);
    for (;i<256;++i){
        if (EXIT_FAILURE == testCPeekC(i)) return i+1000;
    }
    return 0;
}   


