;
; Ullrich von Bassewitz, 31.05.2002
;
; const char* __fastcall__ tgi_map_mode (unsigned char mode);
; /* Map tgi mode codes to driver names */
;

 	.export	       	_tgi_map_mode
        .import         _tgi_mode_table
        .import         return0
        .importzp       tmp1

;----------------------------------------------------------------------------
; BEWARE: The current implementation of tgi_map_mode does not work with tables
; larger that 255 bytes!

.code

_tgi_map_mode:
        sta     tmp1                    ; Save mode
        ldy     #$00

@L0:    lda     _tgi_mode_table,y
        beq     NotFound                ; Branch if mode code zero
        cmp     tmp1
        beq     Found

; Skip the name

@L1:    iny
        lda     _tgi_mode_table,y
        bne     @L1                     ; Loop until end marker found
        iny                             ; Skip end marker
        bne     @L0                     ; Branch always

; Mode not found

NotFound:
        jmp     return0

; Mode found

Found:  tya
        ldx     #>_tgi_mode_table
        sec                             ; Account for the mode byte
        adc     #<_tgi_mode_table       ; Return pointer to file name
        rts

