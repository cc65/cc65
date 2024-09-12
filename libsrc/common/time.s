;
; Ullrich von Bassewitz, 12.11.2002
;
; time_t __fastcall__ time (time_t* timep);
;

        .export         _time

        .import         pusha, ldeaxi
        .importzp       ptr1, sreg, tmp1, tmp2

        .include        "time.inc"


.code

.proc   _time

        pha
        txa
        pha                     ; Save timep

; Get the time (machine dependent)

        .assert timespec::tv_sec = 0, error
        lda     #CLOCK_REALTIME
        jsr     pusha
        lda     #<time
        ldx     #>time
        jsr     _clock_gettime

; _clock_gettime returns 0 on success and -1 on error. Check that.

        inx                     ; Did _clock_gettime return -1?
        bne     @L2             ; Jump if not

; We had an error so invalidate time. A contains $FF.

        ldy     #3
@L1:    sta     time,y
        dey
        bpl     @L1

; Restore timep and check if it is NULL

@L2:    pla
        sta     ptr1+1
        pla
        sta     ptr1            ; Restore timep
        ora     ptr1+1          ; timep == 0?
        beq     @L4

; timep is not NULL, store the result there

        ldy     #3
@L3:    lda     time,y
        sta     (ptr1),y
        dey
        bpl     @L3

; Load the final result.

@L4:    lda     time+3
        sta     sreg+1
        lda     time+2
        sta     sreg
        ldx     time+1
        lda     time
        rts

.endproc

; ------------------------------------------------------------------------
; Data

.bss
time:   .tag    timespec
