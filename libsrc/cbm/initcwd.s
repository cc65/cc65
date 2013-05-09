;
; Stefan Haubenthal, 2005-06-08
;
; CBM _curunit to _cwd
;

        .export         initcwd, devicestr
        .import         curunit, __cwd
        .import         pusha0, tosudiva0
        .importzp       sreg, ptr1, ptr2

        .macpack        generic

initcwd:
        lda     #<__cwd
        ldx     #>__cwd
        sta     ptr2
        stx     ptr2+1
        lda     curunit
        ; Fall through

;------------------------------------------------------------------------------
; Convert unit number in A into string representation pointed to by ptr2.

devicestr:
        jsr     pusha0
        lda     #10
        jsr     tosudiva0
        ldy     #0
        lda     sreg
        beq     @L0             ; >=10
        add     #'0'
        sta     (ptr2),y
        iny
@L0:    lda     ptr1            ; rem
        add     #'0'
        sta     (ptr2),y
        iny
        lda     #0
        sta     (ptr2),y
        rts
