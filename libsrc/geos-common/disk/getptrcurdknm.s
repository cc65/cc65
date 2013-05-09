;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; void GetPtrCurDkNm (char *curName);
;  (fills curName[17] with current disk's name)

            .importzp ptr4, ptr3
            .import __oserror
            .export _GetPtrCurDkNm

            .include "jumptab.inc"
            .include "geossym.inc"
        
_GetPtrCurDkNm: 
        sta ptr3
        stx ptr3+1
        ldx #ptr4
        jsr GetPtrCurDkNm
        ldy #0
        txa
        bne fin
namelp: lda (ptr4),y
.ifdef __GEOS_CBM__
        cmp #$a0
.else
        cmp #0
.endif
        beq fin
        sta (ptr3),y
        iny
        cpy #16
        bne namelp
fin:    lda #0
        sta (ptr3),y
        stx __oserror
        rts
