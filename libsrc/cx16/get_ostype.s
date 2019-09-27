;
; 2019-09-09, Greg King
;
; signed char get_ostype(void)
; /* Return a "build version". */
;
; Positive number -- release build
; Negative number -- prerelease build
; -1 -- custom build
;

        .export         _get_ostype

.proc   _get_ostype
        ldx     #>$0000
        lda     $ff80
        bpl     :+
        dex                     ; negative
:       rts
.endproc
