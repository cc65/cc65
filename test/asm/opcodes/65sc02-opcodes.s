.setcpu "65SC02"

   brk
   ora     ($12,x)
   .byte   $02
   .byte   $03
   tsb     $12
   ora     $12
   asl     $12
   .byte   $07
   php
   ora     #$12
   asl     a
   .byte   $0B
   tsb     $3456
   ora     $3456
   asl     $3456
   .byte   $0F
   bpl     *+122
   ora     ($12),y
   ora     ($12)
   .byte   $13
   trb     $12
   ora     $12,x
   asl     $12,x
   .byte   $17
   clc
   ora     $3456,y
   inc     a
   .byte   $1B
   trb     $3456
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
   and     ($12)
   .byte   $33
   bit     $12,x
   and     $12,x
   rol     $12,x
   .byte   $37
   sec
   and     $3456,y
   dec     a
   .byte   $3B
   bit     $3456,x
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
   eor     ($12)
   .byte   $53
   .byte   $54
   eor     $12,x
   lsr     $12,x
   .byte   $57
   cli
   eor     $3456,y
   phy
   .byte   $5B
   .byte   $5C
   eor     $3456,x
   lsr     $3456,x
   .byte   $5F
   rts
   adc     ($12,x)
   .byte   $62
   .byte   $63
   stz     $12
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
   adc     ($12)
   .byte   $73
   stz     $12,x
   adc     $12,x
   ror     $12,x
   .byte   $77
   sei
   adc     $3456,y
   ply
   .byte   $7B
   jmp     ($3456,x)
   adc     $3456,x
   ror     $3456,x
   .byte   $7F
   bra     *+122
   sta     ($12,x)
   .byte   $82
   .byte   $83
   sty     $12
   sta     $12
   stx     $12
   .byte   $87
   dey
   bit     #$12
   txa
   .byte   $8B
   sty     $3456
   sta     $3456
   stx     $3456
   .byte   $8F
   bcc     *+122
   sta     ($12),y
   sta     ($12)
   .byte   $93
   sty     $12,x
   sta     $12,x
   stx     $12,y
   .byte   $97
   tya
   sta     $3456,y
   txs
   .byte   $9B
   stz     $3456
   sta     $3456,x
   stz     $3456,x
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
   lda     ($12)
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
   cmp     ($12)
   .byte   $D3
   .byte   $D4
   cmp     $12,x
   dec     $12,x
   .byte   $D7
   cld
   cmp     $3456,y
   phx
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
   nop
   .byte   $EB
   cpx     $3456
   sbc     $3456
   inc     $3456
   .byte   $EF
   beq     *+122
   sbc     ($12),y
   sbc     ($12)
   .byte   $F3
   .byte   $F4
   sbc     $12,x
   inc     $12,x
   .byte   $F7
   sed
   sbc     $3456,y
   plx
   .byte   $FB
   .byte   $FC
   sbc     $3456,x
   inc     $3456,x
   .byte   $FF
