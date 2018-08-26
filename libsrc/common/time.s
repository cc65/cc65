;
; Ullrich von Bassewitz, 12.11.2002
;
; time_t __fastcall__ time (time_t* timep);
;

        .export         _time

        .import         decsp1, ldeaxi
        .importzp       ptr1, sreg, tmp1, tmp2

        .include        "time.inc"


.code

.proc   _time

        pha
        txa
        pha                     ; Save timep

; Get the time (machine dependent)

        jsr     decsp1
        lda     #<time
        ldx     #>time
        jsr     _clock_gettime
        sta     tmp2
        lda     #<time
        ldx     #>time
        .assert timespec::tv_sec = 0, error
        jsr     ldeaxi
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

; If the result is != 0, return -1

@L1:    lda     tmp2
        beq     @L2

        tax
        sta     sreg
        sta     sreg+1
        rts

; Reload the low byte of the result and return

@L2:    lda     tmp1
        rts

.endproc

; ------------------------------------------------------------------------
; Data

.bss
time:   .tag    timespec
