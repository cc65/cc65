;
; Ullrich von Bassewitz, 16.11.2002
;
; Read the disk error channel
;

        .export         readdiskerror, getdiskerror
        .import         fnunit
        .importzp       tmp1

        .include        "cbm.inc"

;--------------------------------------------------------------------------
; readdiskerror: Read a disk error from an already open command channel.
; Returns an error code in A, which may either be the code read from the
; command channel, or another error when accessing the command channel failed.

.proc   readdiskerror

; Read the command channel. We won't check the status after the channel is
; open, because this seems to be unnecessary in most cases.

        ldx     #15
        jsr     CHKIN           ; Read from LFN 15
        bcs     done            ; Bail out with error code in A

        jsr     BASIN
        and     #$0F            ; Make digit value from PETSCII
        sta     tmp1
        asl     a               ; * 2
        asl     a               ; * 4, carry clear
        adc     tmp1            ; * 5
        asl     a               ; * 10
        sta     tmp1

        jsr     BASIN
        and     #$0F            ; Make digit value from PETSCII
        clc
        adc     tmp1

; Errors below 20 are not real errors. Fix that

        cmp     #20+1
        bcs     done
        lda     #0

done:   rts

.endproc


;--------------------------------------------------------------------------
; getdiskerror: Open the command channel to a disk unit with id in X. Read
; the error and close it again. Returns an error code in A, which may either
; be the code read from the command channel, or another error when accessing
; the command channel failed.

.proc   getdiskerror

        cpx     #8              ; Disk unit?
        bcc     nodisk

        lda     #15             ; Command channel
        tay                     ; Secondary address
        jsr     SETLFS

        lda     #$00
        jsr     SETNAM          ; No name supplied to OPEN

        jsr     OPEN
        bcs     err

        jsr     readdiskerror
        pha                     ; Save error code
        lda     #15
        jsr     CLOSE
        pla

err:    rts
        
nodisk: lda     #$00
        rts

.endproc



