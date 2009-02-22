;
; Ullrich von Bassewitz, 2009-02-22
;
; unsigned char __fastcall__ _sysrename (const char *oldpath, const char *newpath);
;

        .export         __sysrename

        .import         fnparse, fnadd, fnparsename
        .import         opencmdchannel, closecmdchannel, writefndiskcmd
        .import         popax

        .import         fncmd, fnunit


;--------------------------------------------------------------------------
; __sysrename:

.proc   __sysrename

        jsr     fnparse         ; Parse first filename, pops newpath
        bne     done

        lda     #'='
        jsr     fnadd

        jsr     popax
        jsr     fnparsename     ; Parse second filename
        bne     done

        ldx     fnunit
        jsr     opencmdchannel
        bne     done

        lda     #'r'            ; Rename command
        sta     fncmd
        jsr     writefndiskcmd

        pha
        ldx     fnunit
        jsr     closecmdchannel
        pla

done:   rts

.endproc


