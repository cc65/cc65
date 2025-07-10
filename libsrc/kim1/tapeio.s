;
; int __fastcall__ loadt (unsigned char id);
; int __fastcall__ dumpt (unsigned char id, void* start_addr, void* end_addr);
;

.include        "kim1.inc"

.import         popa, popax, return0, return1

.export         _loadt, _dumpt

.segment        "CODE"

.proc _loadt:   near

        sta     ID               ; Tape record ID to P1L
        jsr     LOADT            ; Read data from tape
        bcs     error
        jmp     return0          ; Return 0 if successful
error:  jmp     return1          ; or 1 if not

.endproc

.proc _dumpt:  near

        sta     EAL              ; End address
        stx     EAH
        jsr     popax
        sta     SAL              ; Start address
        stx     SAH
        jsr     popa
        sta     ID               ; Tape Record ID
        ldx     #$00
        jsr     DUMPT            ; Write data to tape
        bcs     error
        jmp     return0          ; Return 0 if successful
error:  jmp     return1          ; or 1 if not

.endproc
