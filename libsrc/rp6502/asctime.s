;
; char* __fastcall__ asctime (const struct tm* timep);
;
; STRFTIME renders %a and %b in the configured locale, so the English names
; that C requires come from tables here.
;

        .export         _asctime

        .import         _strftime
        .import         pusha0, pushax

        .importzp       ptr1, tmp1

        .include        "time.inc"

BUF_SIZE = 26                   ; "Sun Sep 16 01:03:52 1973\n" and the terminator

_asctime:
        sta     ptr1
        stx     ptr1+1
        ldx     #0
        ldy     #tm::tm_wday
        lda     (ptr1),y
        jsr     name
        ldy     #tm::tm_mon
        lda     (ptr1),y
        clc
        adc     #7              ; the months follow the days
        jsr     name
        lda     #<(buf+8)
        ldx     #>(buf+8)
        jsr     pushax
        lda     #BUF_SIZE-8
        jsr     pusha0
        lda     #<fmt
        ldx     #>fmt
        jsr     pushax
        lda     ptr1
        ldx     ptr1+1
        jsr     _strftime
        tay                     ; 0 when the tail did not fit
        beq     @done
        lda     #<buf
        ldx     #>buf
@done:  rts

; Copy name A and a space to buf at X
name:   sta     tmp1
        asl     a
        adc     tmp1
        tay
@char:  lda     names,y
        sta     buf,x
        iny
        inx
        txa
        and     #3
        cmp     #3
        bne     @char
        lda     #' '
        sta     buf,x
        inx
        rts

        .rodata

names:  .byte   "SunMonTueWedThuFriSat"
        .byte   "JanFebMarAprMayJunJulAugSepOctNovDec"
fmt:    .byte   "%e %H:%M:%S %Y", $0A, $00

        .bss

buf:    .res    BUF_SIZE
