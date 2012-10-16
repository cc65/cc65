;
; Oliver Schmidt, 2012-09-10
;
; char* __fastcall__ getdevicedir (unsigned char device, char* buf, size_t size);
;

        .export         _getdevicedir
        .import         opencmdchannel, closecmdchannel
        .import         writefndiskcmd, readdiskerror
        .import         isdisk, fnunit, fncmd, devicestr
        .import         popa, popax
        .importzp       ptr2, ptr3

        .include        "errno.inc"

;------------------------------------------------------------------------------
; _getdevicedir

.proc   _getdevicedir

; Save size

        sta     ptr3
        stx     ptr3+1

; Save buf

        jsr     popax
        sta     ptr2
        stx     ptr2+1

; Save device

        jsr     popa
        sta     fnunit

; Check for disk device

        tax
        jsr     isdisk
        bcs     erange

; Open channel

        jsr     opencmdchannel
        bne     oserr

; Write command

        lda     #'i'            ; Init command
        sta     fncmd
        jsr     writefndiskcmd
        bne     close

; Read error

        ldx     fnunit
        jsr     readdiskerror

; Close channel

close:  pha
        ldx     fnunit
        jsr     closecmdchannel
        pla
        bne     oserr

; Check for sufficient buf size

        lda     ptr3+1
        bne     okay            ; Buf >= 256
        lda     ptr3
        cmp     #3
        bcs     okay            ; Buf >= 3

erange: lda     #<ERANGE
        jsr     __directerrno
        bne     fail            ; Branch always

oserr:  jsr     __mappederrno

fail:   lda     #0              ; Return NULL
        tax
        rts

; Copy device string representation into buf

okay:   lda     fnunit
        jsr     devicestr       ; Returns 0 in A
        sta     __oserror       ; Clear _oserror

; Success, return buf

        lda     ptr2
        ldx     ptr2+1
        rts

.endproc
