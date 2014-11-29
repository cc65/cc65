
;
; HuC6280 additional opcodes (use with --cpu 65C02)
;
; WARNING: THIS IS __NOT__ COMPLETE !!!
;

;; lda abs
.macro ldaio arg1
	.byte $ad
	.word arg1
.endmacro
;; sta abs
.macro staio arg1
	.byte $8d
	.word arg1
.endmacro
.macro stzio arg1
	.byte $9c
	.word arg1
.endmacro

.macro cla
	.byte $62
.endmacro
.macro clx
	.byte $82
.endmacro

;; lda (zp)
.macro ldaind arg1
	.byte $b2
	.byte arg1
.endmacro

.macro cly
	.byte $c2
.endmacro

.macro st0 arg1
	.if (.match (.left (1, arg1), #))
        ; called with immidiate operand
		.byte $03
		.byte (.right (.tcount (arg1)-1, arg1))
    .else
		.error "illegal address mode"
    .endif
.endmacro
.macro st1 arg1
	.if (.match (.left (1, arg1), #))
        ; called with immidiate operand
		.byte $13
		.byte (.right (.tcount (arg1)-1, arg1))
    .else
		.error "illegal address mode"
    .endif
.endmacro
.macro st2 arg1
	.if (.match (.left (1, arg1), #))
        ; called with immidiate operand
		.byte $23
		.byte (.right (.tcount (arg1)-1, arg1))
    .else
		.error "illegal address mode"
    .endif
.endmacro

; tam #$xx
.macro tam arg1
	.if (.match (.left (1, arg1), #))
        ; called with immidiate operand
		.byte $53
		.byte 1<<(.right (.tcount (arg1)-1, arg1))
    .else
		.error "illegal address mode"
    .endif
.endmacro

; tii x,y,z
.macro tii arg1,arg2,arg3
	.byte $73
	.word arg1,arg2,arg3
.endmacro

.macro csh
	.byte $d4
.endmacro
.macro set
	.byte $f4
.endmacro
.macro _rmb0 arg1
	.byte $07
	.byte arg1
.endmacro
.macro _rmb1 arg1
	.byte $17
	.byte arg1
.endmacro
.macro _rmb2 arg1
	.byte $27
	.byte arg1
.endmacro
.macro _rmb3 arg1
	.byte $37
	.byte arg1
.endmacro
.macro _rmb4 arg1
	.byte $47
	.byte arg1
.endmacro
.macro _rmb5 arg1
	.byte $57
	.byte arg1
.endmacro
.macro _rmb6 arg1
	.byte $67
	.byte arg1
.endmacro
.macro _rmb7 arg1
	.byte $77
	.byte arg1
.endmacro

.macro _smb0 arg1
	.byte $87
	.byte arg1
.endmacro
.macro _smb1 arg1
	.byte $97
	.byte arg1
.endmacro
.macro _smb2 arg1
	.byte $a7
	.byte arg1
.endmacro
.macro _smb3 arg1
	.byte $b7
	.byte arg1
.endmacro
.macro _smb4 arg1
	.byte $c7
	.byte arg1
.endmacro
.macro _smb5 arg1
	.byte $d7
	.byte arg1
.endmacro
.macro _smb6 arg1
	.byte $e7
	.byte arg1
.endmacro
.macro _smb7 arg1
	.byte $f7
	.byte arg1
.endmacro

.macro _bbr0 arg1,arg2
	.byte $0f ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbs0 arg1,arg2
	.byte $8f ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbr1 arg1,arg2
	.byte $1f ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbs1 arg1,arg2
	.byte $9f ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbr2 arg1,arg2
	.byte $2f ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbs2 arg1,arg2
	.byte $af ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbr3 arg1,arg2
	.byte $3f ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbs3 arg1,arg2
	.byte $bf ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbr4 arg1,arg2
	.byte $4f ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbs4 arg1,arg2
	.byte $cf ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbr5 arg1,arg2
	.byte $5f ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbs5 arg1,arg2
	.byte $df ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbr6 arg1,arg2
	.byte $6f ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbs6 arg1,arg2
	.byte $ef ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbr7 arg1,arg2
	.byte $7f ;;,arg1
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro
.macro _bbs7 arg1,arg2
	.byte $ff
	.byte arg1
	.byte <((arg2)-(*+1))
.endmacro


