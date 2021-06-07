;
; Wayne Parham (wayne@parhamdata.com)
;
; int fdisp (void);
; void set_D0 (char segments);
; int get_D0 (void);
; void set_D1 (char segments);
; int get_D1 (void);
; void set_D2 (char segments);
; int get_D2 (void);
; void set_D3 (char segments);
; int get_D3 (void);
; void set_D4 (char segments);
; int get_D4 (void);
; void set_D5 (char segments);
; int get_D5 (void);
; void set_D6 (char segments);
; int get_D6 (void);
;

.include        "sym1.inc"

.export         _fdisp,  _set_D0, _get_D0
.export         _set_D1, _get_D1, _set_D2, _get_D2
.export         _set_D3, _get_D3, _set_D4, _get_D4
.export         _set_D5, _get_D5, _set_D6, _get_D6

.segment        "CODE"

.proc _fdisp:   near

        jsr     SCAND            ;  Flash Display
        rts

.endproc

.proc _set_D0:  near

        sta     DISBUF0          ;  Write Digit 0
        rts

.endproc

.proc _get_D0:  near

        lda     DISBUF0          ;  Read Digit 0
        ldx     #$00             ;
        rts                      ;  Return DISBUF0

.endproc

.proc _set_D1:  near

        sta     DISBUF1          ;  Write Digit 1
        rts

.endproc

.proc _get_D1:  near

        lda     DISBUF1          ;  Read Digit 1
        ldx     #$00             ;
        rts

.endproc

.proc _set_D2:  near

        sta     DISBUF2          ;  Write Digit 2
        rts

.endproc

.proc _get_D2:  near

        lda     DISBUF2          ;  Read Digit 2
        ldx     #$00             ;
        rts                      ;  Return DISBUF2

.endproc

.proc _set_D3:  near

        sta     DISBUF3          ;  Write Digit 3
        rts

.endproc

.proc _get_D3:  near

        lda     DISBUF3          ;  Read Digit 3
        ldx     #$00             ;
        rts                      ;  Return DISBUF3

.endproc

.proc _set_D4:  near

        sta     DISBUF4          ;  Write Digit 4
        rts

.endproc

.proc _get_D4:  near

        lda     DISBUF4          ;  Read Digit 4
        ldx     #$00             ;
        rts                      ;  Return DISBUF4

.endproc

.proc _set_D5:  near

        sta     DISBUF5          ;  Write Digit 5
        rts

.endproc

.proc _get_D5:  near

        lda     DISBUF5          ;  Read Digit 5
        ldx     #$00             ;
        rts                      ;  Return DISBUF5

.endproc

.proc _set_D6:  near

        sta     DISBUF6          ;  Write byte to the right of display
        rts

.endproc

.proc _get_D6:  near

        lda     DISBUF6          ;  Read byte to the right of display
        ldx     #$00             ;
        rts                      ;  Return DISBUF6

.endproc

