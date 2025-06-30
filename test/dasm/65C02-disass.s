.setcpu "65C02"

   brk
   ora     ($12,x)
   tsb     $12
   ora     $12
   asl     $12
   rmb0    $12
   php
   ora     #$12
   asl     a
   tsb     $3456
   ora     $3456
   asl     $3456
   bbr0    $12,*+2
   bpl     *+2
   ora     ($12),y
   ora     ($12)
   trb     $12
   ora     $12,x
   asl     $12,x
   rmb1    $12
   clc
   ora     $3456,y
   inc     a
   trb     $3456
   ora     $3456,x
   asl     $3456,x
   bbr1    $12,*+2
   jsr     $3456
   and     ($12,x)
   bit     $12
   and     $12
   rol     $12
   rmb2    $12
   plp
   and     #$12
   rol     a
   bit     $3456
   and     $3456
   rol     $3456
   bbr2    $12,*+2
   bmi     *+2
   and     ($12),y
   and     ($12)
   bit     $12,x
   and     $12,x
   rol     $12,x
   rmb3    $12
   sec
   and     $3456,y
   dec     a
   bit     $3456,x
   and     $3456,x
   rol     $3456,x
   bbr3    $12,*+2
   rti
   eor     ($12,x)
   eor     $12
   lsr     $12
   rmb4    $12
   pha
   eor     #$12
   lsr     a
   jmp     $3456
   eor     $3456
   lsr     $3456
   bbr4    $12,*+2
   bvc     *+2
   eor     ($12),y
   eor     ($12)
   eor     $12,x
   lsr     $12,x
   rmb5    $12
   cli
   eor     $3456,y
   phy
   eor     $3456,x
   lsr     $3456,x
   bbr5    $12,*+2
   rts
   adc     ($12,x)
   stz     $12
   adc     $12
   ror     $12
   rmb6    $12
   pla
   adc     #$12
   ror     a
   jmp     ($3456)
   adc     $3456
   ror     $3456
   bbr6    $12,*+2
   bvs     *+2
   adc     ($12),y
   adc     ($12)
   stz     $12,x
   adc     $12,x
   ror     $12,x
   rmb7    $12
   sei
   adc     $3456,y
   ply
   jmp     ($3456,x)
   adc     $3456,x
   ror     $3456,x
   bbr7    $12,*+2
   bra     *+2
   sta     ($12,x)
   sty     $12
   sta     $12
   stx     $12
   smb0    $12
   dey
   bit     #$12
   txa
   sty     $3456
   sta     $3456
   stx     $3456
   bbs0    $12,*+2
   bcc     *+2
   sta     ($12),y
   sta     ($12)
   sty     $12,x
   sta     $12,x
   stx     $12,y
   smb1    $12
   tya
   sta     $3456,y
   txs
   stz     $3456
   sta     $3456,x
   stz     $3456,x
   bbs1    $12,*+2
   ldy     #$12
   lda     ($12,x)
   ldx     #$12
   ldy     $12
   lda     $12
   ldx     $12
   smb2    $12
   tay
   lda     #$12
   tax
   ldy     $3456
   lda     $3456
   ldx     $3456
   bbs2    $12,*+2
   bcs     *+2
   lda     ($12),y
   lda     ($12)
   ldy     $12,x
   lda     $12,x
   ldx     $12,y
   smb3    $12
   clv
   lda     $3456,y
   tsx
   ldy     $3456,x
   lda     $3456,x
   ldx     $3456,y
   bbs3    $12,*+2
   cpy     #$12
   cmp     ($12,x)
   cpy     $12
   cmp     $12
   dec     $12
   smb4    $12
   iny
   cmp     #$12
   dex
   cpy     $3456
   cmp     $3456
   dec     $3456
   bbs4    $12,*+2
   bne     *+2
   cmp     ($12),y
   cmp     ($12)
   cmp     $12,x
   dec     $12,x
   smb5    $12
   cld
   cmp     $3456,y
   phx
   cmp     $3456,x
   dec     $3456,x
   bbs5    $12,*+2
   cpx     #$12
   sbc     ($12,x)
   cpx     $12
   sbc     $12
   inc     $12
   smb6    $12
   inx
   sbc     #$12
   nop
   cpx     $3456
   sbc     $3456
   inc     $3456
   bbs6    $12,*+2
   beq     *+2
   sbc     ($12),y
   sbc     ($12)
   sbc     $12,x
   inc     $12,x
   smb7    $12
   sed
   sbc     $3456,y
   plx
   sbc     $3456,x
   inc     $3456,x
L0:
   bbs7    $12,L0
   nop
