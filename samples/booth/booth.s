;
; Rob Burbidge, prandeamus@btinternet.com 
; December 2017
;
        .include "zeropage.inc"        
        .export  _booth8x8
        .import  popa   

; C Wrapper for signed integer 8x8
; int16_t __fastcall__ booth8x8(int8_t x, int8_t y);
; No major attempt has been made to optimise this
; except to optimise the jsr/rts at the end
.proc   _booth8x8
        tax             ; "y" parameter->X
        jsr popa        ; "x" parameter->A
        jmp booth8x8    ; jsr booth8x8; rts
.endproc

; 8-bit multiply A*X->AX using Booth's algorithm
; Compared to a regular multiplication, this will reduce the number of
; 16-bit additions when there is a run of consecutive 1's or 0's in
; the A parameter. On exit, AX contains the result. Y and status are trashed.

.proc   booth8x8
        ; ZP aliases for this routine
        factorA = tmp1  ; First factor - bits are rotated
        factorX = ptr1  ; Second factor - sign extended
        product = ptr2  ; 16 bit product
        
        sta factorA     ; Save factor A
        
        ldy #0          ; Y=0

        ; Zero product (16-bit)
        sty product     ; product = 0
        sty product+1
                
        ; Save X to factorX and sign-extend to factorX+1
        stx factorX     ; low byte factorX
        txa             ; 
        ora #$7F        ; Now A is $FF or $7F
        bmi skipSgn     ; Skip if negative ($FF)
        tya             ; If zero/positive, set 0
skipSgn:sta factorX+1   ; Store high byte factorX
        
        iny             ; Y is now 1
        ldx #8          ; Loop count: 8 bits
        clc             ; Additional bit in stream, initialised to 0
loopTop:
        tya             ; Y=1, equivalent to lda #1
        and factorA     ; Lowest bit of factorA. Sets S and Z, preserve C

        ; Logic for 00, 01, 10, 11 
        ; Remember Z=1 means the low bit is zero)
        bcs cSet
        beq loopNxt     ; C=0, Z=1
        bne minus       ; C=0, Z=0
cSet:   bne loopNxt     ; C=1, Z=0
        ;beq plus       ; C=1, Z=1 (Fall through, not a branch)
        
plus:   ; product += factorX (16 bit)
        clc
        lda product
        adc factorX
        sta product
        lda product+1
        adc factorX+1
        sta product+1
        
        jmp loopNxt
        
minus:  ; product -= factorX (16 bit)
        sec
        lda product
        sbc factorX
        sta product
        lda product+1
        sbc factorX+1
        sta product+1
        
        ; jmp loopNxt   ; Fall through

loopNxt:
        ; factorX *= 2 (16-bit)
        asl factorX
        rol factorX+1
        ; factorA shift right, low bit into C
        lsr factorA         ; Bit 0->C
        dex                 ; preserves C
        bne loopTop

exit:   ; product -> AX
        lda product
        ldx product+1
        ; Return to caller: AX is the product
        rts
.endproc
