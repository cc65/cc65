;
; Oliver Schmidt, 07.09.2009
;
; unsigned __fastcall__ videomode (unsigned mode);
;
        .ifdef  __APPLE2ENH__

        .export         _videomode
        .import         COUT

        .include        "apple2.inc"

        .segment        "LOWCODE"

_videomode:
        ; Get and save current videomode flag
        bit     RD80VID
        php
        
        ; If we are in 80 column mode then the 80 column firmware is
        ; known to be active so we can just print the ctrl-char code
        ; (even if this only means staying in the current videomode)
        bpl     :+
        jsr     COUT
        bra     done

        ; If we are in 40 column mode and want to set 40 column mode
        ; then we explicitly do nothing as we neither know about the
        ; current state of the 80 column firmware nor want to fix it
:       cmp     #$11            ; Ctrl-char code for 40 cols
        beq     done
        
        ; If we are in 40 column mode and want to set 80 column mode
        ; then we first presume the 80 column firmware being already
        ; active and print the ctrl-char code (this causes a garbage
        ; char to be printed on the screen if isn't already active)
        jsr     COUT
        
        ; If we successfully switched to 80 column mode then the 80
        ; column firmware was in fact already active and we're done
        bit     RD80VID
        bmi     done
        
        ; The 80 column firmware isn't already active so we need to
        ; initialize it - causing the screen to be cleared and thus
        ; the garbage char printed above to be erased (but for some
        ; reason the cursor horizontal position not to be zeroed)
        stz     CH

        ; Initializing the 80 column firmware needs the ROM switched
        ; in, otherwise it would copy the F8 ROM to the LC (@ $CEF4)
        bit     $C082

        ; Initialize 80 column firmware
        jsr     $C300           ; PR#3

        ; Switch in LC bank 2 for R/O
        bit     $C080
                
        ; Return ctrl-char code for setting previous
        ; videomode using the saved videomode flag
done:   lda     #$11            ; Ctrl-char code for 40 cols
        plp
        bpl     :+
        lda     #$12            ; Ctrl-char code for 80 cols
:       rts                     ; X was preserved all the way

        .endif                  ; __APPLE2ENH__
