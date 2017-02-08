;*
;* Creativision Joystick Function
;* 
;* unsigned char __fastcall__ joystate(unsigned char joy);
;*
;* JOY_1 -> Return Left Joystick direction
;* JOY_2 -> Return Right Joystick direction
;* JOY_3 -> Return Left Joystick buttons
;* JOY_4 -> Return Right Joystick buttons
;*
;* Will only work if interrupts are enabled.

        .export         _joystate
        .include        "creativision.inc"
        
_joystate:
        cmp             #1      ; Left Direction
        bne             l1
        
        lda             $11
        beq             l5
        and             #$f
        lsr             a
        tax
        inx
        txa
        rts
        
l1:     cmp             #2      ; Right Direction
        bne             l2
        
        lda             $13
        beq             l5
        and             #$f
        lsr             a
        tax
        inx
        txa
        rts
        
l2:     cmp             #3      ; Left Buttons
        bne             l3
        
        lda             $16
        beq             l5
        and             #$f
        rts
        
l3:     cmp             #4
        bne             l4
        
        lda             $17
        beq             l5
        and             #$f
        rts
        
l4:     lda             #0
l5:     rts
