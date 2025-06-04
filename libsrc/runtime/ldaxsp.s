;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Load ax from offset in stack
;

        .export         ldax0sp, ldaxysp
        .importzp       spc

; Beware: The optimizer knows about the value in Y after return!

ldax0sp:
        ldy     #1
ldaxysp:
        lda     (spc),y          ; get high byte
        tax                     ; and save it
        dey                     ; point to lo byte
        lda     (spc),y          ; load low byte
        rts

