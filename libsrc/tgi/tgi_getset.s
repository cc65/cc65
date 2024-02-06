;
; 2002-06-22, Ullrich von Bassewitz
; 2018-07-27, Greg King
;
; Helper function for getpixel/setpixel. Load X/Y from arguments;
; and, check if the co-ordinates are valid. Return carry clear if so.
;

        .include        "tgi-kernel.inc"

        .import         popax
        .importzp       ptr1, ptr2


.proc   tgi_getset

        jsr     tgi_popxy       ; Pop X/Y into ptr1/ptr2

; Are the co-ordinates out of range? First, check if any coord is negative.

        txa                     ; (.X = ptr2+1 from tgi_popxy)
        ora     ptr1+1
        sec                     ; Return carry set if number is negative
        bmi     @L9             ; Bail out if negative

; Check if X is larger than the maximum x coord. If so, bail out.

        lda     ptr1
        cmp     _tgi_xres
        txa
        sbc     _tgi_xres+1
        bcs     @L9

; Check if Y is larger than the maximum y coord.

        lda     ptr2
        cmp     _tgi_yres
        lda     ptr2+1
        sbc     _tgi_yres+1
@L9:    rts

.endproc
