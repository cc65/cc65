; Booth multiply, 8x8 and 16x16 flavours
; Rob Burbidge, prandeamus@btinternet.com 
;
        .include "zeropage.inc"        
        .export  _booth8x8, _booth16x16
        .import  popa,popax

; C wrapper for signed integer 8x8
; int16_t __fastcall__ booth8x8(int8_t x, int8_t y);
.proc   _booth8x8

        sta bFactor2    ; Second parameter in A
        jsr popa        ; First parameter on C stack
        sta bFactor1

        jsr booth8x8

        lda wProduct    ; Return value in AX
        ldx wProduct+1  
        
        rts
.endproc

; C wrapper for signed integer 16x16
; int32_t __fastcall__ booth16x16(int16_t, int16_t y);
.proc   _booth16x16
        
        sta wFactor2    ; Second parameter in AX
        stx wFactor2+1
        
        jsr popax       ; First parameter on C stack
        
        sta wFactor1
        stx wFactor1+1
        
        jsr booth16x16  
        
        ; 32-bit product -> sreg/X/A
        
        lda lProduct
        ldx lProduct+1
        ldy lProduct+2
        sty sreg
        ldy lProduct+3
        sty sreg+1
        
        rts
.endproc 

; ZP aliases for booth8x8

        bFactor1 = tmp1 ; First factor - bits are rotated
        bFactor2 = ptr1 ; Second factor - sign extended into second byte
        wProduct = ptr2 ; 16 bit product

; ZP aliases for booth16x16

        wFactor1 = ptr1 ; First factor - bits are rotated
        wFactor2 = ptr2 ; Second factor - sign extended into ptr3
        lProduct = tmp1 ; 32-bit product tmp1,tmp2,tmp3,tmp4
        
; 8-bit multiply bFactor1*bFactor2->wProduct using Booth's algorithm
; Compared to a regular multiplication, this will reduce the number of
; 16-bit additions when there is a run of consecutive 1's or 0's in
; the A parameter. On exit, AX contains the result. Y and status are trashed.

.proc   booth8x8       
        
        ldy #0          ; Y=0 until further notice

        ; Zero product (16-bit)
        sty wProduct
        sty wProduct+1
                
        ; Sign-extend bFactor2
        lda bFactor2 
        ora #$7F        ; Now A is $FF or $7F
        bmi skipSgn     ; Skip if negative ($FF)
        tya             ; If zero/positive, set 0
skipSgn:sta bFactor2+1  ; Store high byte bFactor2
        
        iny             ; Y is now 1 throughout the loop 
        ldx #8          ; Bits to add
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
        
plus:   ; wProduct += bFactor2 (16 bit)
        clc
        lda wProduct
        adc bFactor2
        sta wProduct
        lda wProduct+1
        adc bFactor2+1
        sta wProduct+1
        
        jmp loopNxt
        
minus:  ; wProduct -= bFactor2 (16 bit)
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
        
        ; Next loop
        dex                 ; preserves C
        bne loopTop
        
        rts
.endproc

; lProduct = wFactor1*wFactor2 using Booth's algorithm
.proc   booth16x16

        ldy  #0     ; Y is 0 until further notice
        
        ; Zero product (32-bit)
        
        sty lProduct
        sty lProduct+1
        sty lProduct+2
        sty lProduct+3 

        ; Sign extend wFactor2
        lda wFactor2+1  ; Upper byte of 2
        ora #$7F        ; Now A is $FF or $7F
        bmi skipSgn     ; Skip if negative ($FF)
        tya             ; If zero/positive, set 0
skipSgn:sta wFactor2+2  ; Store high byte wFactor2+2
        sta wFactor2+3  ; Store high byte wFactor2+3

        iny             ; Y is now 1 throughout the loop 
        ldx #16         ; Bits to add
        clc             ; Additional bit in stream, initialised to 0
loopTop:
        tya             ; Y=1, equivalent to lda #1
        and wFactor1    ; Lowest bit of wFactor1. Sets S and Z, preserve C

        ; Logic for 00, 01, 10, 11 
        ; Remember Z=1 means the low bit is zero)
        bcs cSet
        beq loopNxt     ; C=0, Z=1
        bne minus       ; C=0, Z=0
cSet:   bne loopNxt     ; C=1, Z=0
        ;beq plus       ; C=1, Z=1 (Fall through, not a branch)
        
plus:   ; lProduct += wFactor2 (32 bit)
        clc
        lda lProduct
        adc wFactor2
        sta lProduct
        lda lProduct+1
        adc wFactor2+1
        sta lProduct+1
        lda lProduct+2
        adc wFactor2+2
        sta lProduct+2
        lda lProduct+3
        adc wFactor2+3
        sta lProduct+3

        jmp loopNxt
        
minus:  ; lProduct -= wFactor2 (32 bit)
        sec
        lda lProduct
        sbc wFactor2
        sta lProduct
        lda lProduct+1
        sbc wFactor2+1
        sta lProduct+1
        lda lProduct+2
        sbc wFactor2+2
        sta lProduct+2
        lda lProduct+3
        sbc wFactor2+3
        sta lProduct+3
        
        ; jmp loopNxt   ; Fall through

loopNxt:
        ; wFactor2 *= 2 (32-bit)
        asl wFactor2
        rol wFactor2+1
        rol wFactor2+2
        rol wFactor2+3        
        
        ; wFactor1 shift right, low bit into C
        lsr wFactor1+1
        ror wFactor1        ; Bit 0->C
        
        ; Next loop
        dex                 ; preserves C
        bne loopTop

        rts
.endproc
