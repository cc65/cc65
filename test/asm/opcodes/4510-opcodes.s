.setcpu "4510"

   brk
   ora ($05,x)
   cle
   see
   tsb $02
   ora $02
   asl $02
   rmb0 $02
   php
   ora #$01
   asl
   tsy
   tsb $1234
   ora $1234
   asl $1234
   bbr0 $02,*+$34

   bpl *+$32
   ora ($06),y
   ora ($07),z
   lbpl *+$3133 ; bpl *+$3133
   trb $02
   ora $03,x
   asl $03,x
   rmb1 $02
   clc
   ora $1456,y
   inc
   inz
   trb $1234
   ora $1345,x
   asl $1345,x
   bbr1 $02,*+$34

   jsr $1234
   and ($05,x)
   jsr ($2345)
   jsr ($2456,x)
   bit $02
   and $02
   rol $02
   rmb2 $02
   plp
   and #$01
   rol
   tys
   bit $1234
   and $1234
   rol $1234
   bbr2 $02,*+$34

   bmi *+$32
   and ($06),y
   and ($07),z
   lbmi *+$3133 ; bmi *+$3133
   bit $03,x
   and $03,x
   rol $03,x
   rmb3 $02
   sec
   and $1456,y
   dec
   dez
   bit $1345,x
   and $1345,x
   rol $1345,x
   bbr3 $02,*+$34

   rti
   eor ($05,x)
   neg
   asr
   asr $02
   eor $02
   lsr $02
   rmb4 $02
   pha
   eor #$01
   lsr
   taz
   jmp $1234
   eor $1234
   lsr $1234
   bbr4 $02,*+$34

   bvc *+$32
   eor ($06),y
   eor ($07),z
   lbvc *+$3133 ; bvc *+$3133
   asr $03,x
   eor $03,x
   lsr $03,x
   rmb5 $02
   cli
   eor $1456,y
   phy
   tab
   map
   eor $1345,x
   lsr $1345,x
   bbr5 $02,*+$34

   rts
   adc ($05,x)
   rtn #$09
   bsr *+$3133
   stz $02
   adc $02
   ror $02
   rmb6 $02
   pla
   adc #$01
   ror
   tza
   jmp ($2345)
   adc $1234
   ror $1234
   bbr6 $02,*+$34

   bvs *+$32
   adc ($06),y
   adc ($07),z
   lbvs *+$3133 ; bvs *+$3133
   stz $03,x
   adc $03,x
   ror $03,x
   rmb7 $02
   sei
   adc $1456,y
   ply
   tba
   jmp ($2456,x)
   adc $1345,x
   ror $1345,x
   bbr7 $02,*+$34

   bra *+$32
   sta ($05,x)
   sta ($0f,s),y
   sta ($0f,sp),y
   lbra *+$3133 ; bra *+$3133
   sty $02
   sta $02
   stx $02
   smb0 $02
   dey
   bit #$01
   txa
   sty $1345,x
   sty $1234
   sta $1234
   stx $1234
   bbs0 $02,*+$34

   bcc *+$32
   sta ($06),y
   sta ($07),z
   lbcc *+$3133 ; bcc *+$3133
   sty $03,x
   sta $03,x
   stx $04,y
   smb1 $02
   tya
   sta $1456,y
   txs
   stx $1456,y
   stz $1234
   sta $1345,x
   stz $1345,x
   bbs1 $02,*+$34

   ldy #$01
   lda ($05,x)
   ldx #$01
   ldz #$01
   ldy $02
   lda $02
   ldx $02
   smb2 $02
   tay
   lda #$01
   tax
   ldz $1234
   ldy $1234
   lda $1234
   ldx $1234
   bbs2 $02,*+$34

   bcs *+$32
   lda ($06),y
   lda ($07),z
   lbcs *+$3133 ; bcs *+$3133
   ldy $03,x
   lda $03,x
   ldx $04,y
   smb3 $02
   clv
   lda $1456,y
   tsx
   ldz $1345,x
   ldy $1345,x
   lda $1345,x
   ldx $1456,y
   bbs3 $02,*+$34

   cpy #$01
   cmp ($05,x)
   cpz #$01
   dew $02
   cpy $02
   cmp $02
   dec $02
   smb4 $02
   iny
   cmp #$01
   dex
   asw $1234
   cpy $1234
   cmp $1234
   dec $1234
   bbs4 $02,*+$34

   bne *+$32
   cmp ($06),y
   cmp ($07),z
   lbne *+$3133 ; bne *+$3133
   cpz $02
   cmp $03,x
   dec $03,x
   smb5 $02
   cld
   cmp $1456,y
   phx
   phz
   cpz $1234
   cmp $1345,x
   dec $1345,x
   bbs5 $02,*+$34

   cpx #$01
   sbc ($05,x)
   lda ($0f,s),y
   lda ($0f,sp),y
   inw $02
   cpx $02
   sbc $02
   inc $02
   smb6 $02
   inx
   sbc #$01
   eom
   nop
   row $1234
   cpx $1234
   sbc $1234
   inc $1234
   bbs6 $02,*+$34

   beq *+$32
   sbc ($06),y
   sbc ($07),z
   lbeq *+$3133 ; beq *+$3133
   phd #$089a
   phw #$089a
   sbc $03,x
   inc $03,x
   smb7 $02
   sed
   sbc $1456,y
   plx
   plz
   phd $1234
   phw $1234
   sbc $1345,x
   inc $1345,x
   bbs7 $02,*+$34
