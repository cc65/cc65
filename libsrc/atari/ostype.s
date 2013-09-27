;
; Christian Groessler, July 2004
; from Freddy Offenga's rominfo.c
;
; unsigned char get_ostype(void)
;
; x x x x x x x x   -   8 bit flag
; | | | | | | | |
; | | | | | +-+-+-- main OS rev.
; | | +-+-+-------- minor OS rev.
; +-+-------------- unused
;
; main OS rev.:
;       000 - unknown
;       001 - 400/800 ROM
;       010 - 1200XL ROM
;       011 - XL/XE ROM
;       1xx - unassigned
; minor OS rev.: (depending on main OS rev.);
;       400/800:
;               000 - unknown
;               001 - Rev. A PAL
;               010 - Rev. B PAL
;               011 - Rev. A NTSC
;               100 - Rev. B NTSC
;               101 - unassigned (up to 111)
;       1200XL:
;               000 - unknown
;               001 - Rev. 10
;               010 - Rev. 11
;               011 - unassigned (up to 111)
;       XL/XE:
;               000 - unknown
;               001 - Rev. 1
;               010 - Rev. 2
;               011 - Rev. 3
;               100 - Rev. 4
;               101 - unassigned (up to 111)
;

        .export         _get_ostype

        .include "atari.inc"
        .include "romswitch.inc"

.ifdef __ATARIXL__

        .import __CHARGEN_START__
        .segment "LOWCODE"

.macro  disable_rom_save_a
        pha
        disable_rom
        pla
.endmacro

.else   ; above atarixl, below atari

.macro  disable_rom_save_a
.endmacro

.endif  ; .ifdef __ATARIXL__


; unknown ROM

_unknown:
        lda     #0
        tax
        disable_rom_save_a
        rts


_get_ostype:

        enable_rom
        lda     $fcd8
        cmp     #$a2
        beq     _400800
        lda     $fff1
        cmp     #1
        beq     _1200xl
        lda     $fff7
        cmp     #1
        bcc     _unknown
        cmp     #5
        bcs     _unknown

;XL/XE ROM

        sec
        asl     a
        asl     a
        asl     a
        and     #%00111000
        ora     #%11
_fin:   ldx     #0
        disable_rom_save_a
        rts

; 1200XL ROM

_1200xl:
        lda     $fff7           ; revision #
        cmp     #10
        beq     _1200_10
        cmp     #11
        beq     _1200_11
        lda     #0              ; for unknown
        beq     _1200_fin

_1200_10:
        lda     #%00001000
        bne     _1200_fin

_1200_11:
        lda     #%00010000

_1200_fin:
        ora     #%010
        bne     _fin

; 400/800 ROM

_400800:
        lda     $fff8
        ldx     $fff9
        cmp     #$dd
        bne     _400800_1
        cpx     #$57
        bne     _400800_unknown

; 400/800 NTSC Rev. A

        lda     #%00011001
        bne     _fin

; 400/800 unknown

_400800_unknown:
        lda     #%00000001
        bne     _fin

_400800_1:
        cmp     #$d6
        bne     _400800_2
        cpx     #$57
        bne     _400800_unknown

; 400/800 PAL Rev. A

        lda     #%00001001
        bne     _fin

_400800_2:
        cmp     #$f3
        bne     _400800_3
        cpx     #$e6
        bne     _400800_unknown

; 400/800 NTSC Rev. B

        lda     #%00100001
        bne     _fin

_400800_3:

        cmp     #$22
        bne     _400800_unknown
        cpx     #$58
        bne     _400800_unknown

; 400/800 PAL Rev. B

        lda     #%00010001
        bne     _fin
