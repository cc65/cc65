.setcpu "6502DTV"

   brk
   ora     ($12,x)
   nop     $12
   ora     $12
   asl     $12
   php
   ora     #$12
   asl     a
   anc     #$12
   nop     $3456
   ora     $3456
   asl     $3456
   bpl     *+2
   ora     ($12),y
   bra     *+2
   nop     $12,x
   ora     $12,x
   asl     $12,x
   clc
   ora     $3456,y
   nop     $3456,x
   ora     $3456,x
   asl     $3456,x
   jsr     $3456
   and     ($12,x)
   rla     ($12,x)
   bit     $12
   and     $12
   rol     $12
   rla     $12
   plp
   and     #$12
   rol     a
   bit     $3456
   and     $3456
   rol     $3456
   rla     $3456
   bmi     *+2
   and     ($12),y
   sac     #$12
   rla     ($12),y
   and     $12,x
   rol     $12,x
   rla     $12,x
   sec
   and     $3456,y
   rla     $3456,y
   and     $3456,x
   rol     $3456,x
   rla     $3456,x
   rti
   eor     ($12,x)
   sir     #$12
   eor     $12
   lsr     $12
   pha
   eor     #$12
   lsr     a
   alr     #$12
   jmp     $3456
   eor     $3456
   lsr     $3456
   bvc     *+2
   eor     ($12),y
   eor     $12,x
   lsr     $12,x
   cli
   eor     $3456,x
   lsr     $3456,x
   rts
   adc     ($12,x)
   rra     ($12,x)
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
   bvs     *+2
   adc     ($12),y
   rra     ($12),y
   adc     $12,x
   ror     $12,x
   rra     $12,x
   sei
   adc     $3456,y
   rra     $3456,y
   adc     $3456,x
   ror     $3456,x
   rra     $3456,x
   nop     #$12
   sta     ($12,x)
   sty     $12
   sta     $12
   stx     $12
   dey
   txa
   sty     $3456
   sta     $3456
   stx     $3456
   bcc     *+2
   sta     ($12),y
   sty     $12,x
   sta     $12,x
   stx     $12,y
   tya
   sta     $3456,y
   txs
   shy     $3456,x
   sta     $3456,x
   shx     $3456,y
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
   bcs     *+2
   lda     ($12),y
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
   cpy     $12
   cmp     $12
   dec     $12
   iny
   cmp     #$12
   dex
   axs     #$12
   cpy     $3456
   cmp     $3456
   dec     $3456
   bne     *+2
   cmp     ($12),y
   cmp     $12,x
   dec     $12,x
   cld
   cmp     $3456,y
   cmp     $3456,x
   dec     $3456,x
   cpx     #$12
   sbc     ($12,x)
   cpx     $12
   sbc     $12
   inc     $12
   inx
   sbc     #$12
   nop
   cpx     $3456
   sbc     $3456
   inc     $3456
   beq     *+2
   sbc     ($12),y
   sbc     $12,x
   inc     $12,x
   sed
   sbc     $3456,y
   sbc     $3456,x
   inc     $3456,x
