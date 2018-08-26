;
; Ullrich von Bassewitz, 2009-02-22
;
; unsigned char __fastcall__ _sysrename (const char *oldpath, const char *newpath);
;

        .export         __sysrename

        .import         fnparse, fnadd, fnparsename
        .import         opencmdchannel, closecmdchannel
        .import         writefndiskcmd, readdiskerror
        .import         popptr1

        .import         fncmd, fnunit
        .importzp       ptr1


;--------------------------------------------------------------------------
; __sysrename:

.proc   __sysrename

        jsr     fnparse         ; Parse first filename, pops newpath
        bne     done

        lda     #'='
        jsr     fnadd

        jsr     popptr1
        ; ldy     #0              Y=0 guaranteed by popptr1
        jsr     fnparsename     ; Parse second filename
        bne     done

        ldx     fnunit
        jsr     opencmdchannel
        bne     done

        lda     #'r'            ; Rename command
        sta     fncmd
        jsr     writefndiskcmd

;       ldx     fnunit
;       jsr     readdiskerror

        pha
        ldx     fnunit
        jsr     closecmdchannel
        pla

done:   rts

.endproc


