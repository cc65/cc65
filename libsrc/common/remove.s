;
; Ullrich von Bassewitz, 16.11.2002
;
; int __fastcall__ remove (const char* name);
;

        .export         _remove

        .import         __sysremove
        .import         __osmaperrno
        .import         __errno


;--------------------------------------------------------------------------
; remove


.proc   _remove

; Go ahead and call the machine dependent function

        jsr     __sysremove

; Check for errors

        cmp     #$00
        bne     ok
        jsr     __osmaperrno
        sta     __errno
        stx     __errno+1
        lda     #$FF
        tax
        rts

; Error free

ok:     lda     #$00
        tax
        rts

.endproc




