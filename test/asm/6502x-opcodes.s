.setcpu "6502X"

   brk
   ora     ($12,x)
   jam
   slo     ($12,x)
   nop     $12
   ora     $12
   asl     $12
   slo     $12
   php
   ora     #$12
   asl     a
   anc     #$12
   nop     $3456
   ora     $3456
   asl     $3456
   slo     $3456
   bpl     *+122
   ora     ($12),y
   .byte   $12         ; jam
   slo     ($12),y
   nop     $12,x
   ora     $12,x
   asl     $12,x
   slo     $12,x
   clc
   ora     $3456,y
   .byte   $1a         ; nop
   slo     $3456,y
   nop     $3456,x
   ora     $3456,x
   asl     $3456,x
   slo     $3456,x
   jsr     $3456
   and     ($12,x)
   .byte   $22         ; jam
   rla     ($12,x)
   bit     $12
   and     $12
   rol     $12
   rla     $12
   plp
   and     #$12
   rol     a
   .byte   $2b         ; anc     #$12
   bit     $3456
   and     $3456
   rol     $3456
   rla     $3456
   bmi     *+122
   and     ($12),y
   .byte   $32         ; jam
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
   .byte   $42         ; jam
   sre     ($12,x)
   .byte   $44,$12     ; nop     $12
   eor     $12
   lsr     $12
   sre     $12
   pha
   eor     #$12
   lsr     a
   alr     #$12
   jmp     $3456
   eor     $3456
   lsr     $3456
   sre     $3456
   bvc     *+122
   eor     ($12),y
   .byte   $52         ; jam
   sre     ($12),y
   .byte   $54,$12     ; nop     $12,x
   eor     $12,x
   lsr     $12,x
   sre     $12,x
   cli
   eor     $3456,y
   .byte   $5a         ; nop
   sre     $3456,y
   nop     $3456,x
   eor     $3456,x
   lsr     $3456,x
   sre     $3456,x
   rts
   adc     ($12,x)
   .byte   $62         ; jam
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
   .byte   $72         ; jam
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
   sax     ($12,x)
   sty     $12
   sta     $12
   stx     $12
   sax     $12
   dey
   .byte   $89,$12     ; nop     #$12
   txa
   .byte   $8b,$12     ; xaa     #$12
   sty     $3456
   sta     $3456
   stx     $3456
   sax     $3456
   bcc     *+122
   sta     ($12),y
   .byte   $92     ; jam
   .byte   $93,$12 ; ahx     ($12),y
   sty     $12,x
   sta     $12,x
   stx     $12,y
   sax     $12,y
   tya
   sta     $3456,y
   txs
   tas     $3456,y
   shy     $3456,x
   sta     $3456,x
   shx     $3456,y
   .byte   $9f,$56,$34 ;  ahx     $3456,y
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
   .byte   $b2     ; jam
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
   dcp     ($12,x)
   cpy     $12
   cmp     $12
   dec     $12
   dcp     $12
   iny
   cmp     #$12
   dex
   axs     #$12
   cpy     $3456
   cmp     $3456
   dec     $3456
   dcp     $3456
   bne     *+122
   cmp     ($12),y
   .byte   $d2         ; jam
   dcp     ($12),y
   .byte   $d4,$12     ; nop     $12,x
   cmp     $12,x
   dec     $12,x
   dcp     $12,x
   cld
   cmp     $3456,y
   .byte   $da         ; nop
   dcp     $3456,y
   .byte   $dc,$56,$34 ; nop     $3456,x
   cmp     $3456,x
   dec     $3456,x
   dcp     $3456,x
   cpx     #$12
   sbc     ($12,x)
   .byte   $e2,$12     ; nop     #$12
   isc     ($12,x)
   cpx     $12
   sbc     $12
   inc     $12
   isc     $12
   inx
   sbc     #$12
   nop
   .byte   $eb         ; nop
   cpx     $3456
   sbc     $3456
   inc     $3456
   isc     $3456
   beq     *+122
   sbc     ($12),y
   .byte   $f2         ; jam
   isc     ($12),y
   .byte   $f4,$12     ; nop     $12,x
   sbc     $12,x
   inc     $12,x
   isc     $12,x
   sed
   sbc     $3456,y
   .byte   $fa         ; nop
   isc     $3456,y
   .byte   $fc,$56,$34 ; nop     $3456,x
   sbc     $3456,x
   inc     $3456,x
   isc     $3456,x
