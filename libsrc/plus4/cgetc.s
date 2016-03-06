;
; Ullrich von Bassewitz, 06.08.1998
;
; char cgetc (void);
;

        .export         _cgetc
        .import         cursor

        .include        "plus4.inc"

; --------------------------------------------------------------------------

.segment        "LOWCODE"       ; Accesses the ROM - must go into low mem

_cgetc: lda     KEY_COUNT       ; Get number of characters
        ora     FKEY_COUNT      ; Or with number of function key chars
        bne     L2              ; Jump if there are already chars waiting

; Switch on the cursor if needed

        ldy     CURS_X
        lda     (CRAM_PTR),y    ; Get current char
        pha                     ; And save it
        lda     CHARCOLOR
        sta     (CRAM_PTR),y

        lda     cursor
        beq     L1              ; Jump if no cursor
        tya
        clc
        adc     SCREEN_PTR
        sta     TED_CURSLO
        lda     SCREEN_PTR+1
        adc     #$00
        sbc     #$0B            ; + carry = $C00 (screen address)
        sta     TED_CURSHI

L1:     lda     KEY_COUNT
        ora     FKEY_COUNT
        beq     L1
        pla
        sta     (CRAM_PTR),y
        lda     #$ff
        sta     TED_CURSLO      ; Cursor off
        sta     TED_CURSHI

L2:     sta     ENABLE_ROM      ; Bank in the ROM
        jsr     KBDREAD         ; Read char and return in A (ROM routine)
        sta     ENABLE_RAM      ; Reenable the RAM
        ldx     #0
        rts

; --------------------------------------------------------------------------
; Make the function keys return function key codes instead of the current
; strings so the program will see and may handle them.
; Undo this change when the program ends

        .constructor    initkbd
        .destructor     donekbd

.segment        "ONCE"          ; Special init code segment may get overwritten

.proc   initkbd

        ldy     #15
@L1:    lda     fnkeys,y
        sta     FKEY_SPACE,y
        dey
        bpl     @L1
        rts

.endproc


.segment        "LOWCODE"       ; Accesses the ROM - must go into low mem

.proc   donekbd

        ldx     #$39            ; Copy the original function keys
        sta     ENABLE_ROM      ; Bank in the ROM
@L1:    lda     FKEY_ORIG,x
        sta     FKEY_SPACE,x
        dex
        bpl     @L1
        sta     ENABLE_RAM      ; Bank out the ROM
        rts

.endproc


; Function key table, readonly

.rodata
fnkeys: .byte   $01, $01, $01, $01, $01, $01, $01, $01
        .byte   133, 137, 134, 138, 135, 139, 136, 140

