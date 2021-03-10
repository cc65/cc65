/*****************************************************************************/
/*                                                                           */
/*                  _atari5200os.h                                           */
/*                                                                           */
/*            Internal include file, do not use directly                     */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/

#ifndef __ATARI5200OS_H
#define __ATARI5200OS_H


struct __os {

    /*Page zero*/
    unsigned char pokmsk;            // = $00       System mask for POKEY IRQ enable 
    unsigned char rtclok[2];         // = $01,$02   Real time clock
    unsigned char critic;            // = $03       Critical section flag 
    unsigned char atract;            // = $04       Attract mode counter
    
    union {
        struct {
            unsigned char sdlstl;    // = $05       Save display list LO
            unsigned char sdlsth;    // = $06       Save display list HI
        };
        void*   sdlst;               // = $05,$06   Display list shadow
    };  
    
    unsigned char sdmctl;            // = $07       DMACTL shadow
    unsigned char pcolr0;            // = $08       PM color 0
    unsigned char pcolr1;            // = $09       PM color 1
    unsigned char pcolr2;            // = $0A       PM color 2
    unsigned char pcolr3;            // = $0B       PM color 3
    unsigned char color0;            // = $0C       PF color 0
    unsigned char color1;            // = $0D       PF color 1
    unsigned char color2;            // = $0E       PF color 2
    unsigned char color3;            // = $0F       PF color 3
    unsigned char color4;            // = $10       PF color 4
    unsigned char _free_1[0xEF];     // = $11-$FF   User space
    
    /*Stack*/
    unsigned char stack[0x100];      // = $100-$1FF Stack
    
    /*Page 2 OS variables*/
    void (*vinter)(void);            // = $200      Immediate IRQ vector
    void (*vvblki)(void);            // = $202      Immediate VBI vector
    void (*vvblkd)(void);            // = $204      Deferred VBI vector
    void (*vdslst)(void);            // = $206      DLI vector
    void (*vkeybd)(void);            // = $208      Keyboard IRQ vector
    void (*vkeypd)(void);            // = $20A      Keyboard continuation vector
    void (*vbrkky)(void);            // = $20C      Break key interrupt vector
    void (*vbreak)(void);            // = $20E      BRK instruction interrupt vector
    void (*vserin)(void);            // = $210      Serial input ready vector
    void (*vseror)(void);            // = $212      Serial output data needed vector
    void (*vseroc)(void);            // = $214      Serial output completed vector
    void (*vtimr1)(void);            // = $216      POKEY timer 1 IRQ vector
    void (*vtimr2)(void);            // = $218      POKEY timer 2 IRQ vector
    void (*vtimr4)(void);            // = $21A      POKEY timer 4 IRQ vector
    
};

#endif
