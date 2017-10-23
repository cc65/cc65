.setcpu "4510"

ZP = $12
ABS = $2345

start:
   brk
   ora (ZP,x)
   cle
   see
   tsb ZP
   ora ZP
   asl ZP
   rmb0 ZP
   php
   ora #$01
   asl
   tsy
   tsb ABS
   ora ABS
   asl ABS
   bbr0 ZP,label1

label1:
   bpl label2
   ora (ZP),y
   ora (ZP),z
   lbpl start ; bpl start
   trb ZP
   ora ZP,x
   asl ZP,x
   rmb1 ZP
   clc
   ora ABS,y
   inc
   inz
   trb ABS
   ora ABS,x
   asl ABS,x
   bbr1 ZP,label2

label2:
   jsr ABS
   and (ZP,x)
   jsr ($2345)
   jsr ($2456,x)
   bit ZP
   and ZP
   rol ZP
   rmb2 ZP
   plp
   and #$01
   rol
   tys
   bit ABS
   and ABS
   rol ABS
   bbr2 ZP,label3

label3:
   bmi label4
   and (ZP),y
   and (ZP),z
   lbmi start ; bmi start
   bit ZP,x
   and ZP,x
   rol ZP,x
   rmb3 ZP
   sec
   and ABS,y
   dec
   dez
   bit ABS,x
   and ABS,x
   rol ABS,x
   bbr3 ZP,label4

label4:
   rti
   eor (ZP,x)
   neg
   asr
   asr ZP
   eor ZP
   lsr ZP
   rmb4 ZP
   pha
   eor #$01
   lsr
   taz
   jmp ABS
   eor ABS
   lsr ABS
   bbr4 ZP,label5

label5:
   bvc label6
   eor (ZP),y
   eor (ZP),z
   lbvc start ; bvc start
   asr ZP,x
   eor ZP,x
   lsr ZP,x
   rmb5 ZP
   cli
   eor ABS,y
   phy
   tab
   map
   eor ABS,x
   lsr ABS,x
   bbr5 ZP,label6

label6:
   rts
   adc (ZP,x)
   rtn #$09
   bsr start
   stz ZP
   adc ZP
   ror ZP
   rmb6 ZP
   pla
   adc #$01
   ror
   tza
   jmp ($2345)
   adc ABS
   ror ABS
   bbr6 ZP,label7

label7:
   bvs label8
   adc (ZP),y
   adc (ZP),z
   lbvs start ; bvs start
   stz ZP,x
   adc ZP,x
   ror ZP,x
   rmb7 ZP
   sei
   adc ABS,y
   ply
   tba
   jmp ($2456,x)
   adc ABS,x
   ror ABS,x
   bbr7 ZP,label8

label8:
   bra label9
   sta (ZP,x)
   sta ($0f,sp),y
   lbra start ; bra start
   sty ZP
   sta ZP
   stx ZP
   smb0 ZP
   dey
   bit #$01
   txa
   sty ABS,x
   sty ABS
   sta ABS
   stx ABS
   bbs0 ZP,label9

label9:
   bcc labelA
   sta (ZP),y
   sta (ZP),z
   lbcc start ; bcc start
   sty ZP,x
   sta ZP,x
   stx ZP,y
   smb1 ZP
   tya
   sta ABS,y
   txs
   stx ABS,y
   stz ABS
   sta ABS,x
   stz ABS,x
   bbs1 ZP,labelA

labelA:
   ldy #$01
   lda (ZP,x)
   ldx #$01
   ldz #$01
   ldy ZP
   lda ZP
   ldx ZP
   smb2 ZP
   tay
   lda #$01
   tax
   ldz ABS
   ldy ABS
   lda ABS
   ldx ABS
   bbs2 ZP,labelB

labelB:
   bcs labelC
   lda (ZP),y
   lda (ZP),z
   lbcs start ; bcs start
   ldy ZP,x
   lda ZP,x
   ldx ZP,y
   smb3 ZP
   clv
   lda ABS,y
   tsx
   ldz ABS,x
   ldy ABS,x
   lda ABS,x
   ldx ABS,y
   bbs3 ZP,labelC

labelC:
   cpy #$01
   cmp (ZP,x)
   cpz #$01
   dew ZP
   cpy ZP
   cmp ZP
   dec ZP
   smb4 ZP
   iny
   cmp #$01
   dex
   asw ABS
   cpy ABS
   cmp ABS
   dec ABS
   bbs4 ZP,labelD

labelD:
   bne labelE
   cmp (ZP),y
   cmp (ZP),z
   lbne start ; bne start
   cpz ZP
   cmp ZP,x
   dec ZP,x
   smb5 ZP
   cld
   cmp ABS,y
   phx
   phz
   cpz ABS
   cmp ABS,x
   dec ABS,x
   bbs5 ZP,labelE

labelE:
   cpx #$01
   sbc (ZP,x)
   lda ($0f,sp),y
   inw ZP
   cpx ZP
   sbc ZP
   inc ZP
   smb6 ZP
   inx
   sbc #$01
   eom
   nop
   row ABS
   cpx ABS
   sbc ABS
   inc ABS
   bbs6 ZP,labelF

labelF:
   beq labelG
   sbc (ZP),y
   sbc (ZP),z
   lbeq start ; beq start
   phw #$089a
   sbc ZP,x
   inc ZP,x
   smb7 ZP
   sed
   sbc ABS,y
   plx
   plz
   phd ABS
   phw ABS
   sbc ABS,x
   inc ABS,x
   bbs7 ZP,labelG

labelG:
   brk

