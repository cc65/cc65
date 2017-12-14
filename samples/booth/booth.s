;
; Rob Burbidge, prandeamus@btinternet.com 
; December 2017
;
        .include "zeropage.inc"        
        .export  _booth8x8, _booth16x16
        .import  popa,popax

; C wrapper for signed integer 8x8
; int16_t __fastcall__ booth8x8(int8_t x, int8_t y);
; No major attempt has been made to optimise this
; except to optimise the jsr/rts at the end
.proc   _booth8x8
        tax             ; "y" parameter->X
        jsr popa        ; "x" parameter->A
        jmp booth8x8    ; jsr booth8x8; rts
.endproc

; C wrapper for signed integer 16x16
; int32_t __fastcall__ booth16x16(int16_t, int16_t y);
.proc   _booth16x16
        
        sta wFactor1    ; Second parameter already loaded in AX
        stx wFactor1+1
        jsr popax       ; Load first parameter into AX
        sta wFactor2
        stx wFactor2+1
        
        jsr booth16x16  
        
        ; A/X/sreg = product
        lda lProduct+3
        sta sreg+1
        lda lProduct+2
        sta sreg
        ldx lProduct+1
        lda lProduct
        
        rts
.endproc 

; ZP aliases for booth8x8

        bFactor1 = tmp1 ; First factor - bits are rotated
        bFactor2 = ptr1 ; Second factor - sign extended into second byte
        wProduct = ptr2 ; 16 bit product

; ZP aliases for booth16x16

        wFactor1 = ptr1 ; First factor - bits are rotated
        wFactor2 = ptr2 ; Second factor - sign extended into ptr3
        lProduct = ptr4 ; 32-bit product tmp1,tmp2,tmp3,tmp4
        
; 8-bit multiply A*X->AX using Booth's algorithm
; Compared to a regular multiplication, this will reduce the number of
; 16-bit additions when there is a run of consecutive 1's or 0's in
; the A parameter. On exit, AX contains the result. Y and status are trashed.

.proc   booth8x8
        
        sta bFactor1    ; Save A to bFactor1
        
        ldy #0          ; Y=0

        ; Zero product (16-bit)
        sty wProduct
        sty wProduct+1
                
        ; Save X to bFactor2 and sign-extend to bFactor2+1
        stx bFactor2    ; low byte bFactor2
        txa             ; 
        ora #$7F        ; Now A is $FF or $7F
        bmi skipSgn     ; Skip if negative ($FF)
        tya             ; If zero/positive, set 0
skipSgn:sta bFactor2+1  ; Store high byte bFactor2
        
        iny             ; Y is now 1
        ldx #8          ; Loop count: 8 bits
        clc             ; Additional bit in stream, initialised to 0
loopTop:
        tya             ; Y=1, equivalent to lda #1
        and bFactor1    ; Lowest bit of bFactor1. Sets S and Z, preserve C

        ; Logic for 00, 01, 10, 11 
        ; Remember Z=1 means the low bit is zero)
        bcs cSet
        beq loopNxt     ; C=0, Z=1
        bne minus       ; C=0, Z=0
cSet:   bne loopNxt     ; C=1, Z=0
        ;beq plus       ; C=1, Z=1 (Fall through, not a branch)
        
plus:   ; product += bFactor2 (16 bit)
        clc
        lda wProduct
        adc bFactor2
        sta wProduct
        lda wProduct+1
        adc bFactor2+1
        sta wProduct+1
        
        jmp loopNxt
        
minus:  ; product -= bFactor2 (16 bit)
        sec
        lda wProduct
        sbc bFactor2
        sta wProduct
        lda wProduct+1
        sbc bFactor2+1
        sta wProduct+1
        
        ; jmp loopNxt   ; Fall through

loopNxt:
        ; bFactor2 *= 2 (16-bit)
        asl bFactor2
        rol bFactor2+1
        ; bFactor1 shift right, low bit into C
        lsr bFactor1        ; Bit 0->C
        dex                 ; preserves C
        bne loopTop

exit:   ; product -> AX
        lda wProduct
        ldx wProduct+1
        ; Return to caller: AX is the product
        rts
.endproc

; lProduct = wFactor1*wFactor2 using Booth's algorithm
.proc   booth16x16

        ; DUMMY STUB initial version - just add the two numbers
        clc
        lda wFactor1
        adc wFactor2
        sta lProduct
        lda wFactor1+1
        adc wFactor2+1
        sta lProduct+1
        lda #0
        adc #0
        sta lProduct+2
        lda #0
        sta lProduct+3
        
        rts
.endproc
