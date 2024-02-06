;
; Macros to disable and enable the ROM on Atari XL systems.
;
; Christian Groessler, chris@groessler.org, 19-Sep-2013
;
;
; Defines which modify the operation of the macros:
;
; CHARGEN_RELOC: If defined, CHBAS and CHBASE are updated when
;                enabling or disabling the ROM.
;                If the ROM is enabled, $E0 is written to CHBAS
;                and CHBASE.
;                If the ROM is disabled, the upper byte of
;                __CHARGEN_START__ is written to CHBAS and CHBASE.
; USEWSYNC:      If defined, the code waits for horizontal retrace
;                before switching the ROM and updating CHBAS and
;                CHBASE. This define only has effect if CHARGEN_RELOC
;                is also defined.


.ifdef __ATARIXL__

.ifdef CHARGEN_RELOC

.macro  set_chbase val
        lda     #val
        sta     CHBAS
        sta     CHBASE
.endmacro

.else   ; above CHARGEN_RELOC, below not

.macro  set_chbase val
.endmacro

.endif  ; .ifdef CHARGEN_RELOC


.if .defined(USEWSYNC) .and .defined(CHARGEN_RELOC)

.macro  wsync
        sta     WSYNC
.endmacro

.else   ; above USEWSYNC, below not

.macro  wsync
.endmacro

.endif


; "disable ROM" macros

.macro  disable_rom
        lda     PORTB
        and     #$fe
        wsync
        sta     PORTB
        set_chbase >__CHARGEN_START__
.endmacro

.macro  disable_rom_quick
        lda     PORTB
        and     #$fe
        sta     PORTB
        set_chbase >__CHARGEN_START__
.endmacro

.macro  disable_rom_val val
        lda     val
        wsync
        sta     PORTB
        set_chbase >__CHARGEN_START__
.endmacro

; "enable ROM" macros

.macro  enable_rom
        lda     PORTB
        ora     #1
        wsync
        sta     PORTB
        set_chbase $E0
.endmacro

.macro  enable_rom_quick
        lda     PORTB
        ora     #1
        sta     PORTB
        set_chbase $E0
.endmacro

.else   ; above __ATARIXL__, below not

.macro  disable_rom
.endmacro
.macro  enable_rom
.endmacro

.endif
