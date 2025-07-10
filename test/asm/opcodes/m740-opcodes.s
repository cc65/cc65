; da65 V2.19 - Git 89651fd8b
; Created:    2025-06-16 20:10:42
; Input file: testfile
; Page:       1


        .setcpu "m740"

L000C           := $000C
L040C           := $040C
LFF0C           := $FF0C
        brk
        .byte   $0C
        .byte   $04
        ora     (L000C,x)
        .byte   $04
        jsr     (L000C)

        .byte   $04
        bbs0    a, L8018
        .byte   $04
        .byte   $04
        .byte   $0C
        .byte   $04
        ora     L000C
        .byte   $04
        asl     L000C
        .byte   $04
        bbs0    L000C, L801C
L8018:  php
        .byte   $0C
        .byte   $04
        .byte   $09
L801C:  .byte   $0C
        .byte   $04
        asl     a
        .byte   $0C
        .byte   $04
        seb0    a
        .byte   $0C
        .byte   $04
        .byte   $0C
        .byte   $0C
        .byte   $04
        ora     L040C
        asl     L040C
        seb0    L000C
        .byte   $04
        bpl     L803E
        .byte   $04
        ora     (L000C),y
        .byte   $04
        clt
        .byte   $0C
        .byte   $04
        bbc0    a, L8048
        .byte   $04
        .byte   $14
        .byte   $0C
L803E:  .byte   $04
        ora     L000C,x
        .byte   $04
        asl     L000C,x
        .byte   $04
        bbc0    L000C, L804C
L8048:  clc
        .byte   $0C
        .byte   $04
        .byte   $19
L804C:  .byte   $0C
        .byte   $04
        inc     a
        .byte   $0C
        .byte   $04
        clb0    a
        .byte   $0C
        .byte   $04
        .byte   $1C
        .byte   $0C
        .byte   $04
        ora     L040C,x
        asl     L040C,x
        clb0    L000C
        .byte   $04
        jsr     L040C
        and     (L000C,x)
        .byte   $04
        jsr     LFF0C
        .byte   $04
        bbs1    a, L8078
        .byte   $04
        bit     L000C
        .byte   $04
        and     L000C
        .byte   $04
        rol     L000C
        .byte   $04
        bbs1    L000C, L807C
L8078:  plp
        .byte   $0C
        .byte   $04
        .byte   $29
L807C:  .byte   $0C
        .byte   $04
        rol     a
        .byte   $0C
        .byte   $04
        seb1    a
        .byte   $0C
        .byte   $04
        bit     L040C
        and     L040C
        rol     L040C
        seb1    L000C
        .byte   $04
        bmi     L809E
        .byte   $04
        and     (L000C),y
        .byte   $04
        set
        .byte   $0C
        .byte   $04
        bbc1    a, L80A8
        .byte   $04
        .byte   $34
        .byte   $0C
L809E:  .byte   $04
        and     L000C,x
        .byte   $04
        rol     L000C,x
        .byte   $04
        bbc1    L000C, L80AC
L80A8:  sec
        .byte   $0C
        .byte   $04
        .byte   $39
L80AC:  .byte   $0C
        .byte   $04
        dec     a
        .byte   $0C
        .byte   $04
        clb1    a
        .byte   $0C
        .byte   $04
        ldm     L000C, #$04
        and     L040C,x
        rol     L040C,x
        clb1    L000C
        .byte   $04
        rti

        .byte   $0C
        .byte   $04
        eor     (L000C,x)
        .byte   $04
        stp
        .byte   $0C
        .byte   $04
        bbs2    a, L80D8
        .byte   $04
        com     L000C
        .byte   $04
        eor     L000C
        .byte   $04
        lsr     L000C
        .byte   $04
        bbs2    L000C, L80DC
L80D8:  pha
        .byte   $0C
        .byte   $04
        .byte   $49
L80DC:  .byte   $0C
        .byte   $04
        lsr     a
        .byte   $0C
        .byte   $04
        seb2    a
        .byte   $0C
        .byte   $04
        jmp     L040C

        eor     L040C
        lsr     L040C
        seb2    L000C
        .byte   $04
        bvc     L80FE
        .byte   $04
        eor     (L000C),y
        .byte   $04
        .byte   $52
        .byte   $0C
        .byte   $04
        bbc2    a, L8108
        .byte   $04
        .byte   $54
        .byte   $0C
L80FE:  .byte   $04
        eor     L000C,x
        .byte   $04
        lsr     L000C,x
        .byte   $04
        bbc2    L000C, L810C
L8108:  cli
        .byte   $0C
        .byte   $04
        .byte   $59
L810C:  .byte   $0C
        .byte   $04
        .byte   $5A
        .byte   $0C
        .byte   $04
        clb2    a
        .byte   $0C
        .byte   $04
        .byte   $5C
        .byte   $0C
        .byte   $04
        eor     L040C,x
        lsr     L040C,x
        clb2    L000C
        .byte   $04
        rts

        .byte   $0C
        .byte   $04
        adc     (L000C,x)
        .byte   $04
        .byte   $62
        .byte   $0C
        .byte   $04
        bbs3    a, L8138
        .byte   $04
        tst     L000C
        .byte   $04
        adc     L000C
        .byte   $04
        ror     L000C
        .byte   $04
        bbs3    L000C, L813C
L8138:  pla
        .byte   $0C
        .byte   $04
        .byte   $69
L813C:  .byte   $0C
        .byte   $04
        ror     a
        .byte   $0C
        .byte   $04
        seb3    a
        .byte   $0C
        .byte   $04
        jmp     (L040C)

        adc     L040C
        ror     L040C
        seb3    L000C
        .byte   $04
        bvs     L815E
        .byte   $04
        adc     (L000C),y
        .byte   $04
        .byte   $72
        .byte   $0C
        .byte   $04
        bbc3    a, L8168
        .byte   $04
        .byte   $74
        .byte   $0C
L815E:  .byte   $04
        adc     L000C,x
        .byte   $04
        ror     L000C,x
        .byte   $04
        bbc3    L000C, L816C
L8168:  sei
        .byte   $0C
        .byte   $04
        .byte   $79
L816C:  .byte   $0C
        .byte   $04
        .byte   $7A
        .byte   $0C
        .byte   $04
        clb3    a
        .byte   $0C
        .byte   $04
        .byte   $7C
        .byte   $0C
        .byte   $04
        adc     L040C,x
        ror     L040C,x
        clb3    L000C
        .byte   $04
        bra     L818E
        .byte   $04
        sta     (L000C,x)
        .byte   $04
        rrf     L000C
        .byte   $04
        bbs4    a, L8198
        .byte   $04
        sty     L000C
L818E:  .byte   $04
        sta     L000C
        .byte   $04
        stx     L000C
        .byte   $04
        bbs4    L000C, L819C
L8198:  dey
        .byte   $0C
        .byte   $04
        .byte   $89
L819C:  .byte   $0C
        .byte   $04
        txa
        .byte   $0C
        .byte   $04
        seb4    a
        .byte   $0C
        .byte   $04
        sty     L040C
        sta     L040C
        stx     L040C
        seb4    L000C
        .byte   $04
        bcc     L81BE
        .byte   $04
        sta     (L000C),y
        .byte   $04
        .byte   $92
        .byte   $0C
        .byte   $04
        bbc4    a, L81C8
        .byte   $04
        sty     L000C,x
L81BE:  .byte   $04
        sta     L000C,x
        .byte   $04
        stx     L000C,y
        .byte   $04
        bbc4    L000C, L81CC
L81C8:  tya
        .byte   $0C
        .byte   $04
        .byte   $99
L81CC:  .byte   $0C
        .byte   $04
        txs
        .byte   $0C
        .byte   $04
        clb4    a
        .byte   $0C
        .byte   $04
        .byte   $9C
        .byte   $0C
        .byte   $04
        sta     L040C,x
        .byte   $9E
        .byte   $0C
        .byte   $04
        clb4    L000C
        .byte   $04
        ldy     #$0C
        .byte   $04
        lda     (L000C,x)
        .byte   $04
        ldx     #$0C
        .byte   $04
        bbs5    a, L81F8
        .byte   $04
        ldy     L000C
        .byte   $04
        lda     L000C
        .byte   $04
        ldx     L000C
        .byte   $04
        bbs5    L000C, L81FC
L81F8:  tay
        .byte   $0C
        .byte   $04
        .byte   $A9
L81FC:  .byte   $0C
        .byte   $04
        tax
        .byte   $0C
        .byte   $04
        seb5    a
        .byte   $0C
        .byte   $04
        ldy     L040C
        lda     L040C
        ldx     L040C
        seb5    L000C
        .byte   $04
        bcs     L821E
        .byte   $04
        lda     (L000C),y
        .byte   $04
        jmp     (L000C)

        .byte   $04
        bbc5    a, L8228
        .byte   $04
        ldy     L000C,x
L821E:  .byte   $04
        lda     L000C,x
        .byte   $04
        ldx     L000C,y
        .byte   $04
        bbc5    L000C, L822C
L8228:  clv
        .byte   $0C
        .byte   $04
        .byte   $B9
L822C:  .byte   $0C
        .byte   $04
        tsx
        .byte   $0C
        .byte   $04
        clb5    a
        .byte   $0C
        .byte   $04
        ldy     L040C,x
        lda     L040C,x
        ldx     L040C,y
        clb5    L000C
        .byte   $04
        cpy     #$0C
        .byte   $04
        cmp     (L000C,x)
        .byte   $04
        slw
        .byte   $0C
        .byte   $04
        bbs6    a, L8258
        .byte   $04
        cpy     L000C
        .byte   $04
        cmp     L000C
        .byte   $04
        dec     L000C
        .byte   $04
        bbs6    L000C, L825C
L8258:  iny
        .byte   $0C
        .byte   $04
        .byte   $C9
L825C:  .byte   $0C
        .byte   $04
        dex
        .byte   $0C
        .byte   $04
        seb6    a
        .byte   $0C
        .byte   $04
        cpy     L040C
        cmp     L040C
        dec     L040C
        seb6    L000C
        .byte   $04
        bne     L827E
        .byte   $04
        cmp     (L000C),y
        .byte   $04
        .byte   $D2
        .byte   $0C
        .byte   $04
        bbc6    a, L8288
        .byte   $04
        .byte   $D4
        .byte   $0C
L827E:  .byte   $04
        cmp     L000C,x
        .byte   $04
        dec     L000C,x
        .byte   $04
        bbc6    L000C, L828C
L8288:  cld
        .byte   $0C
        .byte   $04
        .byte   $D9
L828C:  .byte   $0C
        .byte   $04
        .byte   $DA
        .byte   $0C
        .byte   $04
        clb6    a
        .byte   $0C
        .byte   $04
        .byte   $DC
        .byte   $0C
        .byte   $04
        cmp     L040C,x
        dec     L040C,x
        clb6    L000C
        .byte   $04
        cpx     #$0C
        .byte   $04
        sbc     (L000C,x)
        .byte   $04
        fst
        .byte   $0C
        .byte   $04
        bbs7    a, L82B8
        .byte   $04
        cpx     L000C
        .byte   $04
        sbc     L000C
        .byte   $04
        inc     L000C
        .byte   $04
        bbs7    L000C, L82BC
L82B8:  inx
        .byte   $0C
        .byte   $04
        .byte   $E9
L82BC:  .byte   $0C
        .byte   $04
        nop
        .byte   $0C
        .byte   $04
        seb7    a
        .byte   $0C
        .byte   $04
        cpx     L040C
        sbc     L040C
        inc     L040C
        seb7    L000C
        .byte   $04
        beq     L82DE
        .byte   $04
        sbc     (L000C),y
        .byte   $04
        .byte   $F2
        .byte   $0C
        .byte   $04
        bbc7    a, L82E8
        .byte   $04
        .byte   $F4
        .byte   $0C
L82DE:  .byte   $04
        sbc     L000C,x
        .byte   $04
        inc     L000C,x
        .byte   $04
        bbc7    L000C, L82EC
L82E8:  sed
        .byte   $0C
        .byte   $04
        .byte   $F9
L82EC:  .byte   $0C
        .byte   $04
        .byte   $FA
        .byte   $0C
        .byte   $04
        clb7    a
        .byte   $0C
        .byte   $04
        .byte   $FC
        .byte   $0C
        .byte   $04
        sbc     L040C,x
        inc     L040C,x
        clb7    L000C
        .byte   $04
