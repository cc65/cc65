;
; Wayne Parham (wayne@parhamdata.com)
;
; int __fastcall__ loadt (unsigned char id);
; int __fastcall__ dumpt (unsigned char id, void* start_addr, void* end_addr);
;

.include        "sym1.inc"

.import         popa, popax, return0, return1

.export         _loadt, _dumpt

.segment        "CODE"

.proc _loadt:   near

        sta     P1L              ; Tape record ID to P1L
        ldx     #$00
        stx     P1H
        ldy     #$80
        jsr     LOADT            ; Read data from tape
        bcs     error
        jmp     return0          ; Return 0 if sucessful
error:  jmp     return1          ; or 1 if not

.endproc

.proc _dumpt:  near

        sta     P3L              ; End address
        stx     P3H
        jsr     popax
        sta     P2L              ; Start address
        stx     P2H
        jsr     popa
        sta     P1L              ; Tape Record ID
        ldx     #$00
        stx     P1H
        ldy     #$80
        jsr     DUMPT            ; Write data to tape
        bcs     error
        jmp     return0          ; Return 0 if sucessful
error:  jmp     return1          ; or 1 if not

.endproc

