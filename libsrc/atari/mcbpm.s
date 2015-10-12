;
; P/M mouse callbacks for the Ataris
;
; Christian Groessler, 11.04.2014
;
; All functions in this module should be interrupt safe, because they may
; be called from an interrupt handler
;

        .include        "atari.inc"
        .importzp       sp
        .export         _mouse_pm_callbacks
        .constructor    pm_init, 27
        .destructor     pm_down

; get mouse shape data
        .import   mouse_pm_bits
        .importzp mouse_pm_height
        .importzp mouse_pm_hotspot_x
        .importzp mouse_pm_hotspot_y


; P/M definitions. The MOUSE_PM_NUM value can be changed to adjust the
; number of the P/M used for the mouse. All others depend on this value.
; Valid P/M numbers are 0 to 4. When 4 is used, the missiles are used
; as a player.
.ifdef USE_PAGE6
MOUSE_PM_NUM    = 2                             ; P/M used for the mouse
                                                ; This cannot be changed since only player #2 uses the memory at $600.
.else
MOUSE_PM_NUM    = 4                             ; P/M used for the mouse
                                                ; Using player #4 (missiles) wastes the least amount of memory on the
                                                ; atari target, since top of memory is typically at $xC20, and the
                                                ; missiles use the space at $xB00-$xBFF.
                                                ; On the atarixl target this configuration (not using page 6) is not
                                                ; really satisfying since the top of memory typically lies beneath
                                                ; the ROM and there is flickering visible while the ROM is banked in.
.endif
MOUSE_PM_BASE   = pm_base                       ; ZP location pointing to the hw area used by the selected P/M

.if MOUSE_PM_NUM = 4
MOUSE_PM_RAW    = 0                             ; MOUSE_PM_RAW is the hardware P/M number for MOUSE_PM_NUM
.macro  set_mouse_x
        ; assume CF = 0
        sta     HPOSM3
        adc     #2
        sta     HPOSM2
        adc     #2
        sta     HPOSM1
        adc     #2
        sta     HPOSM0
.endmacro
.else
MOUSE_PM_RAW    = MOUSE_PM_NUM + 1
.macro  set_mouse_x
        sta     HPOSP0 + MOUSE_PM_NUM
.endmacro
.endif

; ------------------------------------------------------------------------

        .rodata

        ; Callback structure
_mouse_pm_callbacks:
        .addr   hide
        .addr   show
        .addr   prep
        .addr   draw
        .addr   movex
        .addr   movey

; ------------------------------------------------------------------------

        .bss

omy:    .res    1                       ; old Mouse Y position
colhlp: .res    1                       ; helper variable to set P/M color

; ------------------------------------------------------------------------

        .segment "EXTZP" : zeropage

pm_base:.res    2

; ------------------------------------------------------------------------

        .code

; Hide the mouse cursor.
hide:   lda     #0
        sta     GRACTL
        rts

; Show the mouse cursor.
show:
.if MOUSE_PM_NUM < 4
        lda     #2
.else
        lda     #1
.endif
        sta     GRACTL
        jmp     update_colors

prep:
draw:
        rts

; Move the mouse cursor x position to the value in A/X.
movex:  cpx     #1
        ror     a
        clc
        adc     #48
        sbc     #(mouse_pm_hotspot_x - 1) & $FF
        set_mouse_x
        jmp     update_colors

; Move the mouse cursor y position to the value in A/X.
movey:  clc
        adc     #32
        sbc     #(mouse_pm_hotspot_y - 1) & $FF
        pha
        lda     omy
        jsr     clr_pm                  ; remove player at old position
        jsr     update_colors
        pla
        sta     omy
        ;jmp    set_pm                  ; put player to new position
        ; fall thru

; Set P/M data from 'mouse_pm_bits'
set_pm: tay
        ldx     #0
set_l:  lda     mouse_pm_bits,x
        sta     (MOUSE_PM_BASE),y
        inx
        iny
        beq     set_end
        cpx     #mouse_pm_height
        bcc     set_l
set_end:rts

; Clear (zero) P/M data
clr_pm: ldx     #mouse_pm_height
        tay
        lda     #0
clr_l:  sta     (MOUSE_PM_BASE),y
        iny
        beq     clr_end
        dex
        bne     clr_l
clr_end:rts


pm_down = hide


; this assumes a GRAPHICS 0 screen
update_colors:
        lda     COLOR2                  ; get background color
        and     #$F0
        sta     colhlp
        lda     COLOR1
        and     #$0F
        ora     colhlp

.if MOUSE_PM_NUM = 4
        sta     PCOLR0
        sta     PCOLR1
        sta     PCOLR2
        sta     PCOLR3
        lda     #0
        sta     SIZEM
.else
        sta     PCOLR0 + MOUSE_PM_NUM
        lda     #0
        sta     SIZEP0 + MOUSE_PM_NUM
.endif
        rts

; ------------------------------------------------------------------------

        .segment "INIT"

pm_init:
        lda     #0

.ifdef USE_PAGE6

        sta     MOUSE_PM_BASE
        ldx     #6                      ; page 6
        stx     MOUSE_PM_BASE+1

.else

; use top of memory and lower sp accordingly
        sta     sp
        sta     MOUSE_PM_BASE
        lda     sp+1
        and     #7                      ; offset within 2K
        cmp     #3 + MOUSE_PM_RAW + 1   ; can we use it?
        bcc     @decr                   ; no

        lda     sp+1
        and     #$F8
@set:   adc     #3 + MOUSE_PM_RAW - 1   ; CF is set, so adding MOUSE_PM_RAW + 3
        sta     MOUSE_PM_BASE+1
        sta     sp+1
        bne     @cont                   ; jump always

@decr:  lda     sp+1
        and     #$F8
        sbc     #8 - 1                  ; CF is clear, subtracts 8
        bcs     @set                    ; jump always

@cont:  lda     #0

.endif

        tay
@iniloo:sta     (MOUSE_PM_BASE),y
        iny
        bne     @iniloo

.ifndef USE_PAGE6
        lda     MOUSE_PM_BASE+1
        and     #$F8
.endif
        sta     PMBASE

        lda     #62
        sta     SDMCTL

        lda     #1
        sta     GPRIOR

        jmp     update_colors
