;
; Wayne Parham (wayne@parhamdata.com)
;
; int loadt (int id);
; int dumpt (int id, int start_addr, int end_addr);
;

.include        "sym1.inc"

.import         popax

.export         _loadt, _dumpt

.segment        "CODE"

.proc _loadt:   near

        sta     P1L              ; Tape record ID to P1L
        ldx     #$00
        stx     P1H
        ldy     #$80
        jsr     LOADT            ; Read data from tape
        bcs     error
        lda     #$00
        ldx     #$00             ; Return 0000 if successful
        jmp     done
error:  ldx     #>$0000
        lda     #$FF             ; or 00FF if not
done:   rts

.endproc

.proc _dumpt:  near

        sta     P3L              ; End address
        stx     P3H
        jsr     popax
        sta     P2L              ; Start address
        stx     P2H
        jsr     popax
        sta     P1L              ; Tape Record ID
        ldx     #$00
        stx     P1H
        ldy     #$80
        jsr     DUMPT            ; Write data to tape
        bcs     error
        lda     #$00
        ldx     #$00             ; Return 0000 if successful
        jmp     done
error:  ldx     #>$0000
        lda     #$FF             ; or 00FF if not
done:   rts

.endproc

