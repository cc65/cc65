;
; Ullrich von Bassewitz, 12.11.2002
;
; time_t _systime (void);
; /* Similar to time(), but:
; **   - Is not ISO C
; **   - Does not take the additional pointer
; **   - Does not set errno when returning -1
; */
;

        .export         __systime

        .importzp       sreg

.code

.proc   __systime

        lda     #$FF
        tax
        sta     sreg
        sta     sreg+1
        rts                     ; Return -1

.endproc


