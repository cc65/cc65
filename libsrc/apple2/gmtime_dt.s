;
; Oliver Schmidt, 14.08.2018
; Colin Leroy-Mira, 2023 <colin@colino.net>
;
; struct tm * __fastcall__ gmtime_dt(const struct datetime *dt)
;

        .export         _gmtime_dt, tm_buf

        .include        "time.inc"
        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "mli.inc"

        ; Convert ProDOS date/time to a struct tm
        ; source date address in AX
        ; on stack:
        ; destination struct

_gmtime_dt:
        sta     ptr1
        stx     ptr1+1

        ; Get time
        ldy     #$03
        lda     (ptr1),y
        sta     tm_buf + tm::tm_hour
        dey
        lda     (ptr1),y
        sta     tm_buf + tm::tm_min

        ; Get date
        dey
        lda     (ptr1),y
        lsr
        php                     ; Save month msb
        cmp     #70             ; Year < 70?
        bcs     :+              ; No, leave alone
        adc     #100            ; Move 19xx to 20xx
:       sta     tm_buf + tm::tm_year

        dey
        lda     (ptr1),y
        tax                     ; Save day
        plp                     ; Restore month msb
        ror
        lsr
        lsr
        lsr
        lsr
        beq     erange          ; [1..12] allows for validity check
        tay
        dey                     ; Move [1..12] to [0..11]
        sty     tm_buf + tm::tm_mon
        txa                     ; Restore day
        and     #%00011111
        sta     tm_buf + tm::tm_mday

        lda     #<tm_buf        ; Return pointer to tm_buf
        ldx     #>tm_buf
        rts

        ; Load errno code and return NULL
erange: lda     #ERANGE
        sta     ___errno
        lda     #$00
        tax
        rts

        .bss

tm_buf:
        .tag    tm
