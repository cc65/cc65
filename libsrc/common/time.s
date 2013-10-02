;
; Ullrich von Bassewitz, 12.11.2002
;
; time_t __fastcall__ time (time_t* timep);
;

        .export         _time

        .import         __systime
        .importzp       ptr1, sreg, tmp1

        .include        "errno.inc"


.code

.proc   _time

        pha
        txa
        pha                     ; Save timep

        jsr     __systime       ; Get the time (machine dependent)

        sta     tmp1            ; Save low byte of result

; Restore timep and check if it is NULL

        pla
        sta     ptr1+1
        pla
        sta     ptr1            ; Restore timep
        ora     ptr1+1          ; timep == 0?
        beq     @L1

; timep is not NULL, store the result there

        ldy     #3
        lda     sreg+1
        sta     (ptr1),y
        dey
        lda     sreg
        sta     (ptr1),y
        dey
        txa
        sta     (ptr1),y
        dey
        lda     tmp1
        sta     (ptr1),y

; If the result is less than zero, set ERRNO

@L1:    ldy     sreg+1
        bpl     @L2
                  
        lda     #ENOSYS         ; Function not implemented
        jsr     __seterrno      ; Set __errno

; Reload the low byte of the result and return

@L2:    lda     tmp1
        rts

.endproc


