.setcpu "65C02"
; copy of 65c02, comments note changes to the m740 according to
; http://documentation.renesas.com/doc/products/mpumcu/rej09b0322_740sm.pdf

   brk
   ora     ($12,x)
   .byte   $02,$00,$00 ; jsr zp,ind
   .byte   $03,$00,$00 ; bbs 0,a
   tsb     $12         ; .byte $04
   ora     $12
   asl     $12
   rmb0    $12         ; bbs 0,zp
   php
   ora     #$12
   asl     a
   .byte   $0B,$00,$00 ; seb 0,a
   tsb     $3456       ; .byte $0c
   ora     $3456
   asl     $3456
   bbr0    $12,*+122   ; seb 0,zp
   bpl     *+122
   ora     ($12),y
   ora     ($12)       ; clt
   .byte   $13,$00,$00 ; bbc 0,a
   trb     $12         ; .byte $14
   ora     $12,x
   asl     $12,x
   rmb1    $12         ; bbc 0,zp
   clc
   ora     $3456,y
   inc     a
   .byte   $1B,$00,$00 ; clb 0,a
   trb     $3456       ; .byte $1c
   ora     $3456,x
   asl     $3456,x
   bbr1    $12,*+122   ; clb 0,zp
   jsr     $3456
   and     ($12,x)
   .byte   $22,$00,$00 ; jsr sp
   .byte   $23,$00,$00 ; bbs 1,a
   bit     $12
   and     $12
   rol     $12
   rmb2    $12         ; bbs 1,zp
   plp
   and     #$12
   rol     a
   .byte   $2B,$00,$00 ; seb 1,a
   bit     $3456
   and     $3456
   rol     $3456
   bbr2    $12,*+122   ; seb 1,zp
   bmi     *+122
   and     ($12),y
   and     ($12)       ; set
   .byte   $33,$00,$00 ; bbc 1,a
   bit     $12,x       ; .byte $34
   and     $12,x
   rol     $12,x
   rmb3    $12         ; bbc 1,zp
   sec
   and     $3456,y
   dec     a
   .byte   $3B,$00,$00 ; clb 1,a
   bit     $3456,x     ; ldm zp
   and     $3456,x
   rol     $3456,x
   bbr3    $12,*+122   ; clb 1,zp
   rti
   eor     ($12,x)
   .byte   $42,$00,$00 ; stp
   .byte   $43,$00,$00 ; bbs 2,a
   .byte   $44,$00,$00 ; com zp
   eor     $12
   lsr     $12
   rmb4    $12         ; bbs 2,zp
   pha
   eor     #$12
   lsr     a
   .byte   $4B,$00,$00 ; seb 2,a
   jmp     $3456
   eor     $3456
   lsr     $3456
   bbr4    $12,*+122   ; seb 2,zp
   bvc     *+122
   eor     ($12),y
   eor     ($12)       ; .byte $52
   .byte   $53,$00,$00 ; bbc 2,a
   .byte   $54,$00,$00
   eor     $12,x
   lsr     $12,x
   rmb5    $12         ; bbc 2,zp
   cli
   eor     $3456,y
   phy
   .byte   $5B,$00,$00 ; clb 2,a
   .byte   $5C,$00,$00
   eor     $3456,x
   lsr     $3456,x
   bbr5    $12,*+122   ; clb 2,zp
   rts
   adc     ($12,x)
   .byte   $62,$00,$00 ; mul zp,x
   .byte   $63,$00,$00 ; bbs 3,a
   stz     $12         ; tst zp
   adc     $12
   ror     $12
   rmb6    $12         ; bbs 3,zp
   pla
   adc     #$12
   ror     a
   .byte   $6B,$00,$00 ; seb 3,a
   jmp     ($3456)
   adc     $3456
   ror     $3456
   bbr6    $12,*+122   ; seb 3,zp
   bvs     *+122
   adc     ($12),y
   adc     ($12)       ; .byte $72
   .byte   $73,$00,$00 ; bbc 3,a
   stz     $12,x       ; .byte $74
   adc     $12,x
   ror     $12,x
   rmb7    $12         ; bbc 3,zp
   sei
   adc     $3456,y
   ply
   .byte   $7B,$00,$00 ; clb 3,a
   jmp     ($3456,x)   ; .byte $7c
   adc     $3456,x
   ror     $3456,x
   bbr7    $12,*+122   ; clb 3,zp
   bra     *+122
   sta     ($12,x)
   .byte   $82,$00,$00 ; rrf zp
   .byte   $83,$00,$00 ; bbs 4,a
   sty     $12
   sta     $12
   stx     $12
   smb0    $12         ; bbs 4,zp
   dey
   bit     #$12
   txa
   .byte   $8B,$00,$00 ; seb 4,a
   sty     $3456
   sta     $3456
   stx     $3456
   bbs0    $12,*+122   ; seb 4,zp
   bcc     *+122
   sta     ($12),y
   sta     ($12)       ; .byte $92
   .byte   $93,$00,$00 ; bbc 4,a
   sty     $12,x
   sta     $12,x
   stx     $12,y
   smb1    $12         ; bbc 4,zp
   tya
   sta     $3456,y
   txs
   .byte   $9B,$00,$00 ; clb 4,a
   stz     $3456       ; .byte $9c
   sta     $3456,x
   stz     $3456,x     ; .byte $9e
   bbs1    $12,*+122   ; clb 4,zp
   ldy     #$12
   lda     ($12,x)
   ldx     #$12
   .byte   $A3,$00,$00 ; bbs 5,a
   ldy     $12
   lda     $12
   ldx     $12
   smb2    $12         ; bbs 5,zp
   tay
   lda     #$12
   tax
   .byte   $AB,$00,$00 ; seb 5,a
   ldy     $3456
   lda     $3456
   ldx     $3456
   bbs2    $12,*+122   ; seb 5,zp
   bcs     *+122
   lda     ($12),y
   lda     ($12)       ; .byte $b2
   .byte   $B3,$00,$00 ; bbc 5,a
   ldy     $12,x
   lda     $12,x
   ldx     $12,y
   smb3    $12         ; bbc 5,zp
   clv
   lda     $3456,y
   tsx
   .byte   $BB,$00,$00 ; clb 5,a
   ldy     $3456,x
   lda     $3456,x
   ldx     $3456,y
   bbs3    $12,*+122   ; clb 5,zp
   cpy     #$12
   cmp     ($12,x)
   .byte   $C2,$00,$00 ; wit
   .byte   $C3,$00,$00 ; bbs 6,a
   cpy     $12
   cmp     $12
   dec     $12
   smb4    $12         ; bbs 6,zp
   iny
   cmp     #$12
   dex
   .byte   $CB,$00,$00 ; seb 6,a
   cpy     $3456
   cmp     $3456
   dec     $3456
   bbs4    $12,*+122   ; seb 6,zp
   bne     *+122
   cmp     ($12),y
   cmp     ($12)       ; .byte $d2
   .byte   $D3,$00,$00 ; bbc 6,a
   .byte   $D4,$00,$00
   cmp     $12,x
   dec     $12,x
   smb5    $12         ; bbc 6,zp
   cld
   cmp     $3456,y
   phx
   .byte   $DB,$00,$00 ; clb 6,a
   .byte   $DC,$00,$00
   cmp     $3456,x
   dec     $3456,x
   bbs5    $12,*+122   ; clb 6,zp
   cpx     #$12
   sbc     ($12,x)
   .byte   $E2,$00,$00 ; div zp,x
   .byte   $E3,$00,$00 ; bbs 7,a
   cpx     $12
   sbc     $12
   inc     $12
   smb6    $12         ; bbs 7,zp
   inx
   sbc     #$12
   nop
   .byte   $EB,$00,$00 ; seb 7,a
   cpx     $3456
   sbc     $3456
   inc     $3456
   bbs6    $12,*+122   ; seb 7,zp
   beq     *+122
   sbc     ($12),y
   sbc     ($12)       ; .byte $f2
   .byte   $F3,$00,$00 ; bbc 7,a
   .byte   $F4,$00,$00
   sbc     $12,x
   inc     $12,x
   smb7    $12         ; bbc 7,zp
   sed
   sbc     $3456,y
   plx
   .byte   $FB,$00,$00 ; clb 7,a
   .byte   $FC,$00,$00
   sbc     $3456,x
   inc     $3456,x
   bbs7    $12,*+122   ; clb 7,zp
