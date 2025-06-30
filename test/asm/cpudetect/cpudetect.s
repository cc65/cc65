
.macpack cpu

; step 1: try to assemble an instruction that's exclusive to this set
;         (when possible)

.ifp02
   lda #$ea
.endif

.ifp02x
   lax #$ea
.endif

.ifp6280
   sax
   cla
.endif

.ifpsc02
   jmp ($1234,x)
.endif

.ifpc02
   rmb0 $12
.endif

.ifpwc02
   wai
   stp
.endif

.ifpce02
   ldz #$12
.endif

.ifp4510
   taz
.endif

.ifp45GS02
   orq $1234
.endif

.ifpdtv
   sac #$00
.endif

.ifpm740
   jsr $ff12
.endif

.ifp816
   xba
.endif

.ifpsweet16
   bk
.endif


; step 2: check for bitwise compatibility of instructions sets
;         (made verbose for better reading with hexdump/hd(1))

.if (.cpu .bitand CPU_ISET_NONE)
   .byte 0,"CPU_ISET_NONE"
.endif

.if (.cpu .bitand CPU_ISET_6502)
   .byte 0,"CPU_ISET_6502"
.endif

.if (.cpu .bitand CPU_ISET_6502X)
   .byte 0,"CPU_ISET_6502X"
.endif

.if (.cpu .bitand CPU_ISET_6502DTV)
   .byte 0,"CPU_ISET_6502DTV"
.endif

.if (.cpu .bitand CPU_ISET_65SC02)
   .byte 0,"CPU_ISET_65SC02"
.endif

.if (.cpu .bitand CPU_ISET_65C02)
   .byte 0,"CPU_ISET_65C02"
.endif

.if (.cpu .bitand CPU_ISET_W65C02)
   .byte 0,"CPU_ISET_W65C02"
.endif

.if (.cpu .bitand CPU_ISET_65CE02)
   .byte 0,"CPU_ISET_65CE02"
.endif

.if (.cpu .bitand CPU_ISET_4510)
   .byte 0,"CPU_ISET_4510"
.endif

.if (.cpu .bitand CPU_ISET_45GS02)
   .byte 0,"CPU_ISET_45GS02"
.endif

.if (.cpu .bitand CPU_ISET_HUC6280)
   .byte 0,"CPU_ISET_HUC6280"
.endif

.if (.cpu .bitand CPU_ISET_M740)
   .byte 0,"CPU_ISET_M740"
.endif

.if (.cpu .bitand CPU_ISET_65816)
   .byte 0,"CPU_ISET_65816"
.endif

.if (.cpu .bitand CPU_ISET_SWEET16)
   .byte 0,"CPU_ISET_SWEET16"
.endif


.include "allinst.inc"


; step 3: switch through all supported cpus to verify the pseudo-op is there

.p02
.p02X
.psc02
.pc02
.pwc02
.pce02
.p4510
.p45GS02
.pdtv
.p6280
.pm740
.p816
.psweet16
