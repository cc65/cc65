; Opcodes with implied A addressing
; Implied Q addressing on 45GS02

; Literals, alignments and padding for friendly hex dumps
.align $40

.setcpu "6502"
.literal "6502"
.align $10

    asl
    asl     a
    lsr
    lsr     a
    rol
    rol     a
    ror
    ror     a

.align $40

.setcpu "65C02"
.literal "65C02"
.align $10

    asl
    asl     a
    lsr
    lsr     a
    rol
    rol     a
    ror
    ror     a
    dec
    dec     a
    dea
    inc
    inc     a
    ina

.align $40

.setcpu "65816"
.literal "65816"
.align $10
.a16
.i16

    asl
    asl     a
    lsr
    lsr     a
    rol
    rol     a
    ror
    ror     a
    dec
    dec     a
    dea
    inc
    inc     a
    ina

.align $40

.setcpu "4510"
.literal "4510"
.align $10

    asl
    asl     a
    asr
    asr     a
    lsr
    lsr     a
    rol
    rol     a
    ror
    ror     a
    dec
    dec     a
    dea
    inc
    inc     a
    ina

.align $40

.setcpu "45GS02"
.literal "45GS02"
.align $10

    asl
    asl     a
    asr
    asr     a
    lsr
    lsr     a
    rol
    rol     a
    ror
    ror     a
    dec
    dec     a
    dea
    inc
    inc     a
    ina

    aslq
    aslq    q
    asrq
    asrq    q
    lsrq
    lsrq    q
    rolq
    rolq    q
    rorq
    rorq    q
    deq
    deq     q
    inq
    inq     q
