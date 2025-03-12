;
; Oliver Schmidt, 07.09.2009
;
; unsigned __fastcall__ videomode (unsigned mode);
;
        .ifdef  __APPLE2ENH__

        .export         _videomode

        .include        "apple2.inc"

        .segment        "LOWCODE"

_videomode:
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
        lda     #$15            ; Ctrl-char code for 40 cols
        plp
        bpl     :+
        lda     #$00            ; Ctrl-char code for 80 cols
:       rts                     ; X was preserved all the way

        .endif                  ; __APPLE2ENH__
