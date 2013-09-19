;
; Macros to disable and enable the ROM on Atari XL systems.
;
; Christian Groessler, chris@groessler.org, 19-Sep-2013
;


.ifdef __ATARIXL__

USEWSYNC=	1

.if USEWSYNC
.macro  wsync
        sta     WSYNC
.endmacro
.else
.macro  wsync
.endmacro
.endif


.macro  disable_rom
        lda     PORTB
        and     #$fe
        wsync
        sta     PORTB
        lda     #>__CHARGEN_START__
        sta     CHBAS
        sta     CHBASE
.endmacro
.macro  disable_rom_quick
        lda     PORTB
        and     #$fe
        sta     PORTB
        lda     #>__CHARGEN_START__
        sta     CHBAS
        sta     CHBASE
.endmacro
.macro  disable_rom_val val
        lda     val
        wsync
        sta     PORTB
        lda     #>__CHARGEN_START__
        sta     CHBAS
        sta     CHBASE
.endmacro

.macro  enable_rom
        lda     PORTB
        ora     #1
        wsync
        sta     PORTB
        lda     #$E0
        sta     CHBAS
        sta     CHBASE
.endmacro
.macro  enable_rom_quick
        lda     PORTB
        ora     #1
        sta     PORTB
        lda     #$E0
        sta     CHBAS
        sta     CHBASE
.endmacro

.else   ; above __ATARIXL__, below not

.macro  disable_rom
.endmacro
.macro  enable_rom
.endmacro

.endif
