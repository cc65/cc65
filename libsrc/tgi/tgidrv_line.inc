;
; Oliver Schmidt <ol.sc@web.de>
;
; Generic LINE implementation based on SETPIXEL for TGI driver inclusion
; Code previously present in a2.lo.tgi, c128-vdc.tgi and c128-vdc2.tgi
;

; ------------------------------------------------------------------------

; LINE: Draw a line from X1/Y1 to X2/Y2, where X1/Y1 = ptr1/ptr2 and
; X2/Y2 = ptr3/ptr4 using the current drawing color.
; Must set an error code: NO

        .proc   LINE

; Used for passing parameters to the driver.

X1      :=      ptr1
Y1      :=      ptr2
X2      :=      ptr3
Y2      :=      ptr4

; These ones must be on zpage.

TEMP1   :=      tmp3
TEMP2   :=      tmp4
TEMP3   :=      sreg
TEMP4   :=      sreg+1
PB      :=      ptr3
UB      :=      ptr4
ERR     :=      regsave
NX      :=      regsave+2

; ------------------------------------------------------------------------

        .bss

COUNT:  .res    2
NY:     .res    2
DX:     .res    1
DY:     .res    1
AX:     .res    1
AY:     .res    1

; ------------------------------------------------------------------------

        .code

        ; nx = abs (x2 - x1)
        sec
        lda     X2
        sbc     X1
        sta     NX
        lda     X2+1
        sbc     X1+1
        tay
        lda     NX
        jsr     abs
        sta     NX
        sty     NX+1

        ; ny = abs (y2 - y1)
        sec
        lda     Y2
        sbc     Y1
        sta     NY
        lda     Y2+1
        sbc     Y1+1
        tay
        lda     NY
        jsr     abs
        sta     NY
        sty     NY+1

        ; if (x2 >= x1)
        ldx     #X2
        lda     X1
        ldy     X1+1
        jsr     icmp
        bcc     :+

        ;    dx = 1
        lda     #$01
        bne     :++

        ; else
        ;    dx = -1
:       lda     #$FF
:       sta     DX

        ; if (y2 >= y1)
        ldx     #Y2
        lda     Y1
        ldy     Y1+1
        jsr     icmp
        bcc     :+

        ;    dy = 1
        lda     #$01
        bne     :++

        ; else
        ;    dy = -1
:       lda     #$FF
:       sta     DY

        ; err = ax = ay = 0
        lda     #$00
        sta     ERR
        sta     ERR+1
        sta     AX
        sta     AY

        ; if (nx < ny) {
        ldx     #NX
        lda     NY
        ldy     NY+1
        jsr     icmp
        bcs     :+

        ;    nx <-> ny
        lda     NX
        ldx     NY
        sta     NY
        stx     NX
        lda     NX+1
        ldx     NY+1
        sta     NY+1
        stx     NX+1

        ;    ax = dx
        lda     DX
        sta     AX

        ;    ay = dy
        lda     DY
        sta     AY

        ;    dx = dy = 0 }
        lda     #$00
        sta     DX
        sta     DY

        ; ny = - ny
:       lda     NY
        ldy     NY+1
        jsr     neg
        sta     NY
        sty     NY+1

        ; for (count = nx; count > 0; --count) {
        lda     NX
        ldx     NX+1
        sta     COUNT
        stx     COUNT+1
for:    lda     COUNT           ; count > 0
        ora     COUNT+1
        bne     :+
        rts

        ;    setpixel (X1, Y1)
:       jsr     SETPIXEL

        ;    pb = err + ny
        clc
        lda     ERR
        adc     NY
        sta     PB
        lda     ERR+1
        adc     NY+1
        sta     PB+1
        tax

        ;    ub = pb + nx
        clc
        lda     PB
        adc     NX
        sta     UB
        txa
        adc     NX+1
        sta     UB+1

        ;    x1 = x1 + dx
        ldx     #$00
        lda     DX
        bpl     :+
        dex
:       clc
        adc     X1
        sta     X1
        txa
        adc     X1+1
        sta     X1+1

        ;    y1 = y1 + ay
        ldx     #$00
        lda     AY
        bpl     :+
        dex
:       clc
        adc     Y1
        sta     Y1
        txa
        adc     Y1+1
        sta     Y1+1

        ;    if (abs (pb) < abs (ub)) {
        lda     PB
        ldy     PB+1
        jsr     abs
        sta     TEMP3
        sty     TEMP4
        lda     UB
        ldy     UB+1
        jsr     abs
        ldx     #TEMP3
        jsr     icmp
        bpl     :+

        ;       err = pb }
        lda     PB
        ldx     PB+1
        jmp     next

        ;    else { x1 = x1 + ax
:       ldx     #$00
        lda     AX
        bpl     :+
        dex
:       clc
        adc     X1
        sta     X1
        txa
        adc     X1+1
        sta     X1+1

        ;       y1 = y1 + dy
        ldx     #$00
        lda     DY
        bpl     :+
        dex
:       clc
        adc     Y1
        sta     Y1
        txa
        adc     Y1+1
        sta     Y1+1

        ;       err = ub }
        lda     UB
        ldx     UB+1
next:   sta     ERR
        stx     ERR+1

        ; } (--count)
        lda     COUNT
        sec
        sbc     #$01
        sta     COUNT
        bcc     :+
        jmp     for
:       dec     COUNT+1
        jmp     for


; Copies of some runtime routines

abs:
        ; A/Y := abs (A/Y)
        cpy     #$00
        bpl     :+
        
        ; A/Y := neg (A/Y)
neg:    clc
        eor     #$FF
        adc     #$01
        pha
        tya
        eor     #$FF
        adc     #$00
        tay
        pla
:       rts

icmp:
        ; Compare A/Y to zp,X
        sta     TEMP1           ; TEMP1/TEMP2 - arg2
        sty     TEMP2
        lda     $00,x
        pha
        lda     $01,x
        tay
        pla
        tax
        tya                     ; X/A - arg1 (a = high)

        sec
        sbc     TEMP2
        bne     :++
        cpx     TEMP1
        beq     :+
        adc     #$FF
        ora     #$01
:       rts
:       bvc     :+
        eor     #$FF
        ora     #$01
:       rts

        .endproc
