.setcpu "6502"

   brk
   ora     ($12,x)
   .byte   $02
   .byte   $03
   .byte   $04
   ora     $12
   asl     $12
   .byte   $07
   php
   ora     #$12
   asl     a
   .byte   $0B
   .byte   $0C
   ora     $3456
   asl     $3456
   .byte   $0F
   bpl     *+122
   ora     ($12),y
   .byte   $12
   .byte   $13
   .byte   $14
   ora     $12,x
   asl     $12,x
   .byte   $17
   clc
   ora     $3456,y
   .byte   $1A
   .byte   $1B
   .byte   $1C
   ora     $3456,x
   asl     $3456,x
   .byte   $1F
   jsr     $3456
   and     ($12,x)
   .byte   $22
   .byte   $23
   bit     $12
   and     $12
   rol     $12
   .byte   $27
   plp
   and     #$12
   rol     a
   .byte   $2B
   bit     $3456
   and     $3456
   rol     $3456
   .byte   $2F
   bmi     *+122
   and     ($12),y
   .byte   $32
   .byte   $33
   .byte   $34
   and     $12,x
   rol     $12,x
   .byte   $37
   sec
   and     $3456,y
   .byte   $3A
   .byte   $3B
   .byte   $3C
   and     $3456,x
   rol     $3456,x
   .byte   $3F
   rti
   eor     ($12,x)
   .byte   $42
   .byte   $43
   .byte   $44
   eor     $12
   lsr     $12
   .byte   $47
   pha
   eor     #$12
   lsr     a
   .byte   $4B
   jmp     $3456
   eor     $3456
   lsr     $3456
   .byte   $4F
   bvc     *+122
   eor     ($12),y
   .byte   $52
   .byte   $53
   .byte   $54
   eor     $12,x
   lsr     $12,x
   .byte   $57
   cli
   eor     $3456,y
   .byte   $5A
   .byte   $5B
   .byte   $5C
   eor     $3456,x
   lsr     $3456,x
   .byte   $5F
   rts
   adc     ($12,x)
   .byte   $62
   .byte   $63
   .byte   $64
   adc     $12
   ror     $12
   .byte   $67
   pla
   adc     #$12
   ror     a
   .byte   $6B
   jmp     ($3456)
   adc     $3456
   ror     $3456
   .byte   $6F
   bvs     *+122
   adc     ($12),y
   .byte   $72
   .byte   $73
   .byte   $74
   adc     $12,x
   ror     $12,x
   .byte   $77
   sei
   adc     $3456,y
   .byte   $7A
   .byte   $7B
   .byte   $7C
   adc     $3456,x
   ror     $3456,x
   .byte   $7F
   .byte   $80
   sta     ($12,x)
   .byte   $82
   .byte   $83
   sty     $12
   sta     $12
   stx     $12
   .byte   $87
   dey
   .byte   $89
   txa
   .byte   $8B
   sty     $3456
   sta     $3456
   stx     $3456
   .byte   $8F
   bcc     *+122
   sta     ($12),y
   .byte   $92
   .byte   $93
   sty     $12,x
   sta     $12,x
   stx     $12,y
   .byte   $97
   tya
   sta     $3456,y
   txs
   .byte   $9B
   .byte   $9C
   sta     $3456,x
   .byte   $9E
   .byte   $9F
   ldy     #$12
   lda     ($12,x)
   ldx     #$12
   .byte   $A3
   ldy     $12
   lda     $12
   ldx     $12
   .byte   $A7
   tay
   lda     #$12
   tax
   .byte   $AB
   ldy     $3456
   lda     $3456
   ldx     $3456
   .byte   $AF
   bcs     *+122
   lda     ($12),y
   .byte   $B2
   .byte   $B3
   ldy     $12,x
   lda     $12,x
   ldx     $12,y
   .byte   $B7
   clv
   lda     $3456,y
   tsx
   .byte   $BB
   ldy     $3456,x
   lda     $3456,x
   ldx     $3456,y
   .byte   $BF
   cpy     #$12
   cmp     ($12,x)
   .byte   $C2
   .byte   $C3
   cpy     $12
   cmp     $12
   dec     $12
   .byte   $C7
   iny
   cmp     #$12
   dex
   .byte   $CB
   cpy     $3456
   cmp     $3456
   dec     $3456
   .byte   $CF
   bne     *+122
   cmp     ($12),y
   .byte   $D2
   .byte   $D3
   .byte   $D4
   cmp     $12,x
   dec     $12,x
   .byte   $D7
   cld
   cmp     $3456,y
   .byte   $DA
   .byte   $DB
   .byte   $DC
   cmp     $3456,x
   dec     $3456,x
   .byte   $DF
   cpx     #$12
   sbc     ($12,x)
   .byte   $E2
   .byte   $E3
   cpx     $12
   sbc     $12
   inc     $12
   .byte   $E7
   inx
   sbc     #$12
   .byte   $EB
   cpx     $3456
   sbc     $3456
   inc     $3456
   .byte   $EF
   beq     *+122
   sbc     ($12),y
   .byte   $F2
   .byte   $F3
   .byte   $F4
   sbc     $12,x
   inc     $12,x
   .byte   $F7
   sed
   sbc     $3456,y
   .byte   $FA
   .byte   $FB
   .byte   $FC
   sbc     $3456,x
   inc     $3456,x
   .byte   $FF
