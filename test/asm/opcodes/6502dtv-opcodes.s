.setcpu "6502DTV"

   brk
   ora     ($12,x)
   .byte   $02
   .byte   $03
   nop     $12
   ora     $12
   asl     $12
   .byte   $07
   php
   ora     #$12
   asl     a
   anc     #$12
   nop     $3456
   ora     $3456
   asl     $3456
   .byte   $0f
   bpl     *+122
   ora     ($12),y
   bra     *+122
   .byte   $13
   nop     $12,x
   ora     $12,x
   asl     $12,x
   .byte   $17
   clc
   ora     $3456,y
   .byte   $1a         ; nop
   .byte   $1b
   nop     $3456,x
   ora     $3456,x
   asl     $3456,x
   .byte   $1f
   jsr     $3456
   and     ($12,x)
   .byte   $22
   rla     ($12,x)
   bit     $12
   and     $12
   rol     $12
   rla     $12
   plp
   and     #$12
   rol     a
   .byte   $2b,$12     ; anc     #$12
   bit     $3456
   and     $3456
   rol     $3456
   rla     $3456
   bmi     *+122
   and     ($12),y
   sac     #$12
   rla     ($12),y
   .byte   $34,$12     ; nop     $12,x
   and     $12,x
   rol     $12,x
   rla     $12,x
   sec
   and     $3456,y
   .byte   $3a         ; nop
   rla     $3456,y
   .byte   $3c,$56,$34 ; nop     $3456,x
   and     $3456,x
   rol     $3456,x
   rla     $3456,x
   rti
   eor     ($12,x)
   sir     #$12
   .byte   $43
   .byte   $44,$12     ; nop     $12
   eor     $12
   lsr     $12
   .byte   $47
   pha
   eor     #$12
   lsr     a
   alr     #$12
   jmp     $3456
   eor     $3456
   lsr     $3456
   .byte   $4f
   bvc     *+122
   eor     ($12),y
   .byte   $52
   .byte   $53
   .byte   $54,$12     ; nop     $12,x
   eor     $12,x
   lsr     $12,x
   .byte   $57
   cli
   eor     $3456,y
   .byte   $5a         ; nop
   .byte   $5b
   .byte   $5c,$56,$34 ; nop     $3456,x
   eor     $3456,x
   lsr     $3456,x
   .byte   $5f
   rts
   adc     ($12,x)
   .byte   $62
   rra     ($12,x)
   .byte   $64,$12     ; nop     $12
   adc     $12
   ror     $12
   rra     $12
   pla
   adc     #$12
   ror     a
   arr     #$12
   jmp     ($3456)
   adc     $3456
   ror     $3456
   rra     $3456
   bvs     *+122
   adc     ($12),y
   .byte   $72
   rra     ($12),y
   .byte   $74,$12     ; nop     $12,x
   adc     $12,x
   ror     $12,x
   rra     $12,x
   sei
   adc     $3456,y
   .byte   $7a         ; nop
   rra     $3456,y
   .byte   $7c,$56,$34 ; nop     $3456,x
   adc     $3456,x
   ror     $3456,x
   rra     $3456,x
   nop     #$12
   sta     ($12,x)
   .byte   $82,$12     ; nop     #$12
   .byte   $83
   sty     $12
   sta     $12
   stx     $12
   .byte   $87
   dey
   .byte   $89,$12     ; nop     #$12
   txa
   .byte   $8b
   sty     $3456
   sta     $3456
   stx     $3456
   .byte   $8f
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
   .byte   $9b
   shy     $3456,x
   sta     $3456,x
   shx     $3456,y
   .byte   $9f
   ldy     #$12
   lda     ($12,x)
   ldx     #$12
   lax     ($12,x)
   ldy     $12
   lda     $12
   ldx     $12
   lax     $12
   tay
   lda     #$12
   tax
   lax     #$12
   ldy     $3456
   lda     $3456
   ldx     $3456
   lax     $3456
   bcs     *+122
   lda     ($12),y
   .byte   $b2
   lax     ($12),y
   ldy     $12,x
   lda     $12,x
   ldx     $12,y
   lax     $12,y
   clv
   lda     $3456,y
   tsx
   las     $3456,y
   ldy     $3456,x
   lda     $3456,x
   ldx     $3456,y
   lax     $3456,y
   cpy     #$12
   cmp     ($12,x)
   .byte   $c2,$12     ; nop     #$12
   .byte   $c3
   cpy     $12
   cmp     $12
   dec     $12
   .byte   $c7
   iny
   cmp     #$12
   dex
   axs     #$12
   cpy     $3456
   cmp     $3456
   dec     $3456
   .byte   $cf
   bne     *+122
   cmp     ($12),y
   .byte   $d2
   .byte   $d3
   .byte   $d4,$12     ; nop     $12,x
   cmp     $12,x
   dec     $12,x
   .byte   $d7
   cld
   cmp     $3456,y
   .byte   $da         ; nop
   .byte   $db
   .byte   $dc,$56,$34 ; nop     $3456,x
   cmp     $3456,x
   dec     $3456,x
   .byte   $df
   cpx     #$12
   sbc     ($12,x)
   .byte   $e2,$12     ; nop     #$12
   .byte   $e3
   cpx     $12
   sbc     $12
   inc     $12
   .byte   $e7
   inx
   sbc     #$12
   nop
   .byte   $eb,$12     ; sbc     #$12
   cpx     $3456
   sbc     $3456
   inc     $3456
   .byte   $ef
   beq     *+122
   sbc     ($12),y
   .byte   $f2
   .byte   $f3
   .byte   $f4,$12     ; nop     $12,x
   sbc     $12,x
   inc     $12,x
   .byte   $f7
   sed
   sbc     $3456,y
   .byte   $fa         ; nop
   .byte   $fb
   .byte   $fc,$56,$34 ; nop     $3456,x
   sbc     $3456,x
   inc     $3456,x
   .byte   $ff
