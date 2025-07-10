;
; Oliver Schmidt, 14.08.2018
; Colin Leroy-Mira, 2023 <colin@colino.net>
;
; time_t __fastcall__ mktime_dt(const struct datetime *dt)
;

        .import         steaxspidx, pushax, incsp2, _gmtime_dt
        .import         tm_buf
        .export         _mktime_dt

        .include        "time.inc"
        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "mli.inc"

        ; Convert ProDOS date/time to UNIX timestamp
        ; source date address in AX

_mktime_dt:
        ; Convert to internal tm
        jsr     _gmtime_dt
        cpx     #$00
        bne     :+
        cmp     #$00
        beq     err

        ; Make time_t
:       lda     #<tm_buf
        ldx     #>tm_buf
        jmp     _mktime

err:    lda     #$00
        tax
        sta     sreg
        sta     sreg+1
        rts
