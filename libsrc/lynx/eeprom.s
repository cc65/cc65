;****************
; CC65 Lynx Library
;
; Originally by Bastian Schick
; http://www.geocities.com/SiliconValley/Byte/4242/lynx/
;
; Ported to cc65 (http://www.cc65.org) by
; Shawn Jefferson, June 2004
;
; Several changes,
; Ullrich von Bassewitz, 1004-10-14
;
;
;****************
;* EEPROM-routs
;* for 93C46 (1024bit => 64 16-bit words)
;*
;* created : 11.05.95
;* last modified :
;*
;* 16.02.96      leaner (thanks to Harry)
;* 12.03.96      test for busy after write and erase (well, Harry ;)) )
;* 22.08.97      ported to ra65 for use with cc65
;* 02.12.97      added xref for the new ra65
;*
;*
;* (c) 1995..97 Bastian Schick
;* CS    = A7 (18)
;* CLK   = A1 (11)
;* DI/DO = AUDIN (32)
;*
;* And now how to contact the EEPROM :
;*
;* CARD
;* PORT               ----\/----      93C46(SMD too)
;* (18)  A7   --------| CS     |- +5V
;* (11)  A1   --------| CLK    |- NC
;*                +---| DI     |- NC
;* (32) AUDIN ----+---| DO     |- GND
;*                    ----------
;*
;****************


        .export         _lynx_eeprom_read
        .export         _lynx_eeprom_write
        .export         _lynx_eeprom_erase
        .import         popa
        .importzp       ptr1

        .include        "lynx.inc"


; ------------------------------------------------------------------------
; EEPROM command list

EE_C_WRITE      =    $40
EE_C_READ       =    $80
EE_C_ERASE      =    $C0
EE_C_EWEN       =    $30
EE_C_EWDS       =    $00


; ------------------------------------------------------------------------
; unsigned __fastcall__ lynx_eeprom_read (unsigned char cell);
; /* Read a 16 bit word from the given address */
;

_lynx_eeprom_read:
        and     #$3f
        ora     #EE_C_READ
        jsr     EE_Send9Bit

        lda     #$a
        sta     IODIR            ; set AUDIN to Input

        clc
        stz     ptr1
        stz     ptr1+1          ; Clear result
        ldy     #16-1           ; Initialize bit counter
@L1:
; CLK = 1
        stz     RCART0
        stz     RCART0
; CLK = 0
        stz     RCART0
        stz     RCART0

        lda     IODAT
        and     #$10             ; mask bit
        adc     #$f0             ; C=1 if A=$10
        rol     ptr1
        rol     ptr1+1           ; shifts 0 to Carry
        dey
        bpl     @L1

        ldx     #$1a
        stx     IODIR            ; set AUDIN for output
;EE_SET_CS_LOW

        ldx     #3
        stx     SYSCTL1
        dex
        stx     SYSCTL1

        lda     ptr1
        ldy     ptr1+1          ; Load result

        rts


; ------------------------------------------------------------------------
; unsigned __fastcall__ lynx_eeprom_erase (unsigned char cell);
; /* Clear the word at the given address */
;

_lynx_eeprom_erase:
        pha                     ; Save argument
        lda     #EE_C_EWEN      ; EWEN
        jsr     EE_Send9Bit
        pla                     ; Restore cell
        and     #$3f
        ora     #EE_C_ERASE     ; clear cell A
        jsr     EE_Send9Bit
        bra     EE_wait


; ------------------------------------------------------------------------
; unsigned __fastcall__ lynx_eeprom_write (unsigned char cell, unsigned val);
; /* Write the word at the given address */
;

_lynx_eeprom_write:
        sta     ptr1
        stx     ptr1+1          ; Save val into ptr1
        lda     #EE_C_EWEN      ; EWEN
        jsr     EE_Send9Bit
        jsr     popa            ; Get cell
        and     #$3f            ; Make valid range 0..63
        ora     #EE_C_WRITE     ; WRITE
        jsr     EE_Send9Bit
        jsr     EE_Send16Bit    ; Send value in ptr1

EE_wait:
; EE_SET_CS_HIGH

        ldx     #63
EEloop:
        stz     RCART0
        stz     RCART0
        dex
        bpl     EEloop

        lda     #$0A
        sta     IODIR           ; AUDIN to input
        lda     #$10
EE_wait1:
        bit     IODAT           ; 'til ready :D0-read is /D0-written
        beq     EE_wait1
        lda     #$1a            ; AUDIN to output
        sta     IODIR

        lda     #EE_C_EWDS      ; EWDS

;       bra     EE_Send9Bit     ; fall into


; ------------------------------------------------------------------------
; Send 8 bit value in A to eeprom

EE_Send9Bit:
; EE_SET_CS_LOW
        ldx     #3
        stx     SYSCTL1
        dex
        stx     SYSCTL1
; EE_SET_CS_HIGH

        ldx     #63
EEloop2:
        stz     RCART0
        stz     RCART0
        dex
        bpl     EEloop2

        ldy     #8
        sec                     ; start bit
        ror     A
        ror     A
        ror     A
        ror     A               ; bit 8 at pos. 4
EEloop3:
        tax
        and     #$10
        ora     #$b
        sta     IODAT
; CLK = 1
        stz     RCART0
        stz     RCART0
; CLK = 0
        stz     RCART0
        stz     RCART0
        txa
        rol     A
        dey
        bpl     EEloop3

        lda     #$b             ; fnr neue EEPROMs
        sta     IODAT

        rts

; ------------------------------------------------------------------------
; Send 16 bit value in ptr1 to eeprom

EE_Send16Bit:
        lda     ptr1+1

        ror     A
        ror     ptr1
        ror     A
        ror     ptr1
        ror     A
        ror     ptr1

        ldy     #15
EEloop4:
        tax
        and     #$10
        ora     #$b
        sta     IODAT
; CLK = 1
        stz     RCART0
        stz     RCART0
; CLK = 0
        stz     RCART0
        stz     RCART0
        txa
        rol     ptr1
        rol     A
        dey
        bpl     EEloop4

; EE_SET_CS_LOW
        ldx     #3
        stx     SYSCTL1
        dex
        stx     SYSCTL1

        lda     #$b             ; fnr neue EEPROMs
        sta     IODAT

        rts

                      
