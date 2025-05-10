;
; Oliver Schmidt, 07.09.2009
;
; signed char __fastcall__ videomode (unsigned mode);
;
        .export         _videomode

        .import         ostype, has_80cols_card, returnFFFF

        .ifndef __APPLE2ENH__
        .constructor    init_80cols, 8
        .endif

        .include        "apple2.inc"
        .include        "mli.inc"


VIDEOMODE_40x24 = $15
VIDEOMODE_80x24 = $00


        .ifndef __APPLE2ENH__

        .segment        "ONCE"

; has_80cols_card >= $80 if a 80 column card is present
.proc init_80cols
        lda     ostype
        cmp     #$30          ; Apple //e
        ror                   ; Carry to high bit
        sta     has_80cols_card
        rts
.endproc

        .endif

        .segment        "LOWCODE"

_videomode:
        .ifndef __APPLE2ENH__
        bit     has_80cols_card
        bmi     set_mode

        ; No 80 column card, return error if requested mode is 80cols
        cmp     #VIDEOMODE_40x24
        beq     out
        jmp     returnFFFF
set_mode:
        .endif

        ; Get and save current videomode flag
        bit     RD80VID
        php

        ; Initializing the 80 column firmware needs the ROM switched
        ; in, otherwise it would copy the F8 ROM to the LC (@ $CEF4)
        bit     $C082

        ; Call 80 column firmware with ctrl-char code
        jsr     $C300

        ; Switch in LC bank 2 for R/O
        bit     $C080

        ; Switch in alternate charset again
        sta     SETALTCHAR

        ; Return ctrl-char code for setting previous
        ; videomode using the saved videomode flag
        lda     #VIDEOMODE_40x24
        plp
        bpl     out
        lda     #VIDEOMODE_80x24
out:    rts                     ; X was preserved all the way
