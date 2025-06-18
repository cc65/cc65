
    ; just emit to bss for now, this way the output wont get checked, which is
    ; fine - we only want to see if the assembler accepts all the opcodes it
    ; should accept for the current CPU mode
    .bss

.if (.cpu .bitand CPU_ISET_6502)
    .scope
    ; regular basic 6502 (legal) instruction set
LABEL0:
    brk             ; $00
    ora ($12,x)     ; $01
    ora $12         ; $05
    asl $12         ; $06
    php             ; $08
    ora #$12        ; $09
    asl a           ; $0a
    ora $1234       ; $0d
    asl $1234       ; $0e
    bpl LABEL0      ; $10
    ora ($12),y     ; $11
    ora $12,x       ; $15
    asl $12,x       ; $16
    clc             ; $18
    ora $1234,y     ; $19
    ora $1234,x     ; $1d
    asl $1234,x     ; $1e
    jsr $1234       ; $20
    and ($12,x)     ; $21
    bit $12         ; $24
    and $12         ; $25
    rol $12         ; $26
    plp             ; $28
    and #$12        ; $29
    rol a           ; $2a
    bit $1234       ; $2c
    and $1234       ; $2d
    rol $1234       ; $2e
    bmi LABEL       ; $30
    and ($12),y     ; $31
    and $12,x       ; $35
    rol $12,x       ; $36
    sec             ; $38
    and $1234,y     ; $39
    and $1234,x     ; $3d
    rol $1234,x     ; $3e
    rti             ; $40
    eor ($12,x)     ; $41
    eor $12         ; $45
    lsr $12         ; $46
    pha             ; $48
    eor #$12        ; $49
    lsr a           ; $4a
    jmp $1234       ; $4c
    eor $1234       ; $4d
    lsr $1234       ; $4e
    bvc LABEL       ; $50
    eor ($12),y     ; $51
    eor $12,x       ; $55
    lsr $12,x       ; $56
    cli             ; $58
    eor $1234,y     ; $59
    eor $1234,x     ; $5d
    lsr $1234,x     ; $5e
    rts             ; $60
    adc ($12,x)     ; $61
    adc $12         ; $65
    ror $12         ; $66
    pla             ; $68
    adc #$12        ; $69
    ror a           ; $6a
    jmp ($1234)     ; $6c
    adc $1234       ; $6d
    ror $1234       ; $6e
    bvs LABEL       ; $70
    adc ($12),y     ; $71
    adc $12,x       ; $75
    ror $12,x       ; $76
    sei             ; $78
    adc $1234,y     ; $79
    adc $1234,x     ; $7d
    ror $1234,x     ; $7e
LABEL:
    sta ($12,x)     ; $81
    sty $12         ; $84
    sta $12         ; $85
    stx $12         ; $86
    dey             ; $88
    txa             ; $8a
    sty $1234       ; $8c
    sta $1234       ; $8d
    stx $1234       ; $8e
    bcc LABEL       ; $90
    sta ($12),y     ; $91
    sty $12,x       ; $94
    sta $12,x       ; $95
    stx $12,y       ; $96
    tya             ; $98
    sta $1234,y     ; $99
    txs             ; $9a
    sta $1234,x     ; $9d
    ldy #$12        ; $a0
    lda ($12,x)     ; $a1
    ldx #$12        ; $a2
    ldy $12         ; $a4
    lda $12         ; $a5
    ldx $12         ; $a6
    tay             ; $a8
    lda #$12        ; $a9
    tax             ; $aa
    ldy $1234       ; $ac
    lda $1234       ; $ad
    ldx $1234       ; $ae
    bcs LABEL       ; $b0
    lda ($12),y     ; $b1
    ldy $12,x       ; $b4
    lda $12,x       ; $b5
    ldx $12,y       ; $b6
    clv             ; $b8
    lda $1234,y     ; $b9
    tsx             ; $ba
    ldy $1234,x     ; $bc
    lda $1234,x     ; $bd
    ldx $1234,y     ; $be
    cpy #$12        ; $c0
    cmp ($12,x)     ; $c1
    cpy $12         ; $c4
    cmp $12         ; $c5
    dec $12         ; $c6
    iny             ; $c8
    cmp #$12        ; $c9
    dex             ; $ca
    cpy $1234       ; $cc
    cmp $1234       ; $cd
    dec $1234       ; $ce
    bne LABEL       ; $d0
    cmp ($12),y     ; $d1
    cmp $12,x       ; $d5
    dec $12,x       ; $d6
    cld             ; $d8
    cmp $1234,y     ; $d9
    cmp $1234,x     ; $dd
    dec $1234,x     ; $de
    cpx #$12        ; $e0
    sbc ($12,x)     ; $e1
    cpx $12         ; $e4
    sbc $12         ; $e5
    inc $12         ; $e6
    inx             ; $e8
    sbc #$12        ; $e9
    nop             ; $ea
    cpx $1234       ; $ec
    sbc $1234       ; $ed
    inc $1234       ; $ee
    beq LABEL1      ; $f0
    sbc ($12),y     ; $f1
    sbc $12,x       ; $f5
    inc $12,x       ; $f6
    sed             ; $f8
    sbc $1234,y     ; $f9
    sbc $1234,x     ; $fd
    inc $1234,x     ; $fe
LABEL1:
    .endscope
.endif


.if (.cpu .bitand CPU_ISET_6502X)
    .scope
    ; all "undocumented" 6502 instructions (aka "unintended opcodes")
    jam             ; $02
    slo ($12,x)     ; $03
    nop $12         ; $04
    slo $12         ; $07
    anc #$12        ; $0b
    nop $1234       ; $0c
    slo $1234       ; $0f
    jam             ; $12
    slo ($12),y     ; $13
    nop $12,x       ; $14
    slo $12,y       ; $17
    nop             ; $1a
    slo $1234,y     ; $1b
    nop $1234,x     ; $1c
    slo $1234,x     ; $1f
    jam             ; $22
    rla ($12,x)     ; $23
    rla $12         ; $27
    anc #$12        ; $2b
    rla $1234       ; $2f
    jam             ; $32
    rla ($12),y     ; $33
    nop $12,x       ; $34
    rla $12,y       ; $37
    nop             ; $3a
    rla $1234,y     ; $3b
    nop $1234,x     ; $3c
    rla $1234,x     ; $3f
    jam             ; $42
    sre ($12,x)     ; $43
    nop $12         ; $44
    sre $12         ; $47
    alr #$12        ; $4b
    sre $1234       ; $4f
    jam             ; $52
    sre ($12),y     ; $53
    nop $12,x       ; $54
    sre $12,y       ; $57
    nop             ; $5a
    sre $1234,y     ; $5b
    nop $1234,x     ; $5c
    sre $1234,x     ; $5f
    jam             ; $62
    rra ($12,x)     ; $63
    nop $12         ; $64
    rra $12         ; $67
    arr #$12        ; $6b
    rra $1234       ; $6f
    jam             ; $72
    rra ($12),y     ; $73
    nop $12,x       ; $74
    rra $12,y       ; $77
    nop             ; $7a
    rra $1234,y     ; $7b
    nop $1234,x     ; $7c
    rra $1234,x     ; $7f
    nop #$12        ; $80
    nop #$12        ; $82
    sax ($12,x)     ; $83
    sax $12         ; $87
    nop #$12        ; $89
    ;xaa #$12        ; $8b  FIXME (implement in assembler)
    sax $1234       ; $8f
    jam             ; $92
    ;ahx ($12),y     ; $93  FIXME (implement in assembler)
    sax $12,y       ; $97
    tas $1234,y     ; $9b
    shy $1234,x     ; $9c
    shx $1234,y     ; $9e
    ;ahx $1234,y     ; $9f  FIXME (implement in assembler)
    lax ($12,x)     ; $a3
    lax $12         ; $a7
    lax #$12        ; $ab
    lax $1234       ; $af
    jam             ; $b2
    lax ($12),y     ; $b3
    lax $12,y       ; $b7
    las $1234,y     ; $bb
    lax $1234,y     ; $bf
    nop #$12        ; $c2
    dcp ($12,x)     ; $c3
    dcp $12         ; $c7
    axs #$12        ; $cb
    dcp $1234       ; $cf
    jam             ; $d2
    dcp ($12),y     ; $d3
    nop $12,x       ; $d4
    dcp $12,y       ; $d7
    nop             ; $da
    dcp $1234,y     ; $db
    nop $1234,x     ; $dc
    dcp $1234,x     ; $df
    nop #$12        ; $e2
    isc ($12,x)     ; $e3
    isc $12         ; $e7
    sbc #$12        ; $eb
    isc $1234       ; $ef
    jam             ; $f2
    isc ($12),y     ; $f3
    nop $12,x       ; $f4
    isc $12,y       ; $f7
    nop             ; $fa
    isc $1234,y     ; $fb
    nop $1234,x     ; $fc
    isc $1234,x     ; $ff
    .endscope
.endif


.if (.cpu .bitand CPU_ISET_6502DTV)
    .scope
LABEL:
    ; opcodes added over 6502 (these are JAM on 6502)
    bra LABEL       ; $12
    sac #$12        ; $32
    sir #$12        ; $42

    ; opcodes added over 6502, which work the same as the "illegal" opcodes on 6502
    nop $12         ; $04
    nop $1234       ; $0c
    nop $12,x       ; $14
    nop             ; $1a
    nop $1234,x     ; $1c
    nop $12,x       ; $34
    nop             ; $3a
    nop $1234,x     ; $3c
    nop $12         ; $44
    nop $12,x       ; $54
    nop             ; $5a
    nop $1234,x     ; $5c
    nop $12         ; $64
    nop $12,x       ; $74
    nop             ; $7a
    nop $1234,x     ; $7c
    nop #$12        ; $80
    nop #$12        ; $82
    nop #$12        ; $89
    nop #$12        ; $c2
    nop $12,x       ; $d4
    nop             ; $da
    nop $1234,x     ; $dc
    nop #$12        ; $e2
    nop $12,x       ; $f4
    nop             ; $fa
    nop $1234,x     ; $fc

    anc #$12        ; $0b
    anc #$12        ; $2b

    rla ($12,x)     ; $23
    rla $12         ; $27
    rla $1234       ; $2f
    rla ($12),y     ; $33
    rla $12,y       ; $37
    rla $1234,y     ; $3b
    rla $1234,x     ; $3f

    lax ($12,x)     ; $a3
    lax $12         ; $a7
    lax #$12        ; $ab
    lax $1234       ; $af
    lax ($12),y     ; $b3
    lax $12,y       ; $b7
    las $1234,y     ; $bb
    lax $1234,y     ; $bf

    alr #$12        ; $4b

    arr #$12        ; $6b

    rra ($12,x)     ; $63
    rra $12         ; $67
    rra $1234       ; $6f
    rra ($12),y     ; $73
    rra $12,y       ; $77
    rra $1234,y     ; $7b
    rra $1234,x     ; $7f

    shy $1234,x     ; $9c
    shx $1234,y     ; $9e

    axs #$12        ; $cb

    sbc #$12        ; $eb

    .endscope
.endif


;------------------------------------------------------------------------------
; The 65c02 is the CMOS re-design of the 6502. It has a few improvements:
;
;      65C02            65ce02
;
; $04  tsb zp
; $0c  tsb abs16
; $12  ora (zp)         (-> ora (zp), z)
; $14  trb zp
; $1a  inc
; $1c  trb abs16
; $32  and (zp)         (-> and (zp), z)
; $34  bit zp, x
; $3a  dec
; $3c  bit abs16, x
; $52  eor (zp)         (-> eor (zp), z)
; $5a  phy
; $64  stz zp           (store z, not 0)
; $72  adc (zp)         (-> adc (zp), z)
; $74  stz zp, x        (store z, not 0)
; $7a  ply
; $7c  jmp (abs16, x)
; $80  bra rel8
; $89  bit #imm8
; $92  sta (zp)         (-> sta (zp), z)
; $9c  stz abs16        (store z, not 0)
; $9e  stz abs16, x     (store z, not 0)
; $b2  lda (zp)         (-> lda (zp), z)
; $d2  cmp (zp)         (-> cmp (zp), z)
; $da  phx
; $f2  sbc (zp)         (-> sbc (zp), z)
; $fa  plx

; FIXME: currently CPU_ISET_65SC02 and CPU_65SC02 really means "65C02"

; FIXME: should really check for 65C02

.if (.cpu .bitand CPU_ISET_65SC02)
    .scope
    ; 65c02 instruction set adds some extra legal instructions to 6502
    tsb $12         ; $04
    tsb $1234       ; $0c
    ;ora ($12)       ; $12  FIXME: not working with 4510:ora (zp), z
    trb $12         ; $14
    inc a           ; $1a
    trb $1234       ; $1c
    ;and ($12)       ; $32  FIXME: not working with 4510:and (zp), z
    bit $12,x       ; $34
    dec a           ; $3a
    bit $1234,x     ; $3c
    ;eor ($12)       ; $52  FIXME: not working with 4510:eor (zp), z
    phy             ; $5a
    stz $12         ; $64
    ;adc ($12)       ; $72  FIXME: not working with 4510:adc (zp), z
    stz $12,x       ; $74
    ply             ; $7a
    jmp ($1234)     ; $7c
LABEL:
    bra LABEL       ; $80
    bit #$12        ; $89
    ;sta ($12)       ; $92  FIXME: not working with 4510:sta (zp), z
    stz $1234       ; $9c
    stz $1234,x     ; $9e
    ;lda ($12)       ; $b2  FIXME: not working with 4510:lda (zp), z
    ;cmp ($12)       ; $d2  FIXME: not working with 4510:cmp (zp), z
    phx             ; $da
    ;sbc ($12)       ; $f2  FIXME: not working with 4510:sbc (zp), z
    plx             ; $fa
    .endscope
.endif

; FIXME: hack so these opcodes get fixed anyway, while 4510 is still quirky
.if (.cpu .bitand CPU_ISET_65SC02)
.if (.not .cpu = CPU_4510)
    ora ($12)       ; $12
    and ($12)       ; $32
    eor ($12)       ; $52
    adc ($12)       ; $72
    sta ($12)       ; $92
    lda ($12)       ; $b2
    cmp ($12)       ; $d2
    sbc ($12)       ; $f2
.endif
.endif

; TODO: R65C02
; The R65C02 is a superset of the 65C02. It adds bit manipulation instructions:
;   smbB zp         set bit in zp location
;   rmbB zp         reset bit in zp location
;   bbsB zp, rel8   branch if bit is set in zp location
;   bbrB zp, rel8   branch if bit is reset in zp location

; FIXME: currently CPU_ISET_65C02 and CPU_65C02 really means "W65C02"

; FIXME: should really check for R65C02

.if (.cpu .bitand CPU_ISET_65C02)

    ; R65C02 instruction set adds some extra legal instructions to 65C02
    rmb0 $12            ; $07
    bbr0 $12, LABEL3    ; $0f
    rmb1 $12            ; $17
    bbr1 $12, LABEL3    ; $1f
    rmb2 $12            ; $27
    bbr2 $12, LABEL3    ; $2f
    rmb3 $12            ; $37
    bbr3 $12, LABEL3    ; $3f
    rmb4 $12            ; $47
    bbr4 $12, LABEL3    ; $4f
    rmb5 $12            ; $57
    bbr5 $12, LABEL3    ; $5f
    rmb6 $12            ; $67
    bbr6 $12, LABEL3    ; $6f
    rmb7 $12            ; $77
    bbr7 $12, LABEL3    ; $7f
LABEL3:
    smb0 $12            ; $87
    bbs0 $12, LABEL3    ; $8f
    smb1 $12            ; $97
    bbs1 $12, LABEL3    ; $9f
    smb2 $12            ; $a7
    bbs2 $12, LABEL3    ; $af
    smb3 $12            ; $b7
    bbs3 $12, LABEL3    ; $bf
    smb4 $12            ; $c7
    bbs4 $12, LABEL3    ; $cf
    smb5 $12            ; $d7
    bbs5 $12, LABEL3    ; $df
    smb6 $12            ; $e7
    bbs6 $12, LABEL3    ; $ef
    smb7 $12            ; $f7
    bbs7 $12, LABEL3    ; $ff

.endif


; TODO: W65C02
; The W65C02 is a superset of the R65C02. It only adds two instructions:
;
; $cb  wai             wait for interrupt
; $db  stp             wait for reset

; FIXME: currently CPU_ISET_65C02 and CPU_65C02 really means "W65C02"

; FIXME: should really check for W65C02

.if (.cpu = CPU_65C02)
    wai             ; $cb
    stp             ; $db
.endif


; TODO: 65CE02
; The 65CE02 is another superset of the R65C02. It has several improvements:
;
; $02  cle              clear stack extend disable
; $03  see              set stack extend disable
; $0b  tsy              transfer stack_ptr_high to Y
; $12  ora (zp), z
; $13  lbpl rel16
; $1b  inz              increment Z
; $22  jsr (abs16)
; $23  jsr (abs16, x)
; $2b  tys              transfer Y to stack_ptr_high
; $32  and (zp), z
; $33  lbmi rel16
; $3b  dez              decrement Z
; $42  neg              negate A
; $43  asr
; $44  asr zp
; $4b  taz              transfer A to Z
; $52  eor (zp), z
; $53  lbvc rel16
; $54  asr zp, x
; $5b  tab
; $5c  aug              "4-byte NOP reserved for future expansion"
; $62  rtn #imm8
; $63  lbsr rel16       relative jsr, "branch to subroutine"
; $64  stz zp           store Z
; $6b  tza              transfer Z to A
; $72  adc (zp), z
; $73  lbvs rel16
; $74  stz zp, x        store Z
; $7b  tba
; $82  sta (off8, s), y
; $83  lbra rel16       relative jmp
; $8b  sty abs16, x
; $92  sta (zp), z
; $93  lbcc rel16
; $9b  stx abs16, y
; $9c  stz abs16        store Z
; $9e  stz abs16, x     store Z
; $a3  ldz #imm8
; $ab  ldz abs16
; $b2  lda (zp), z
; $b3  lbcs rel16
; $bb  ldz abs16, x
; $c2  cpz #imm8
; $c3  dew zp
; $cb  asw abs16
; $d2  cmp (zp), z
; $d3  lbne rel16
; $d4  cpz zp
; $db  phz              push Z
; $dc  cpz abs16
; $e2  lda (off8, s), y
; $e3  inw zp
; $eb  row abs16
; $f2  sbc (zp), z
; $f3  lbeq rel16
; $f4  phw #imm16
; $fb  plz              pull Z
; $fc  phw abs16

; FIXME: should really check for 65CE02

.if (.cpu .bitand CPU_ISET_4510)
    .scope
    ; 65CE02 adds the following:
    cle                         ; $02
    see                         ; $03
    tsy                         ; $0b
    ora ($12), z                ; $12    (ora (zp) on 65C02)
    lbpl $1234                  ; $13
    inz                         ; $1b
    jsr ($1234)                 ; $22
    jsr ($1234,x)               ; $23
    tys                         ; $2b
    and ($12), z                ; $32    (and (zp) on 65C02)
    lbmi $1234                  ; $33
    dez                         ; $3b
    neg                         ; $42
    asr a                       ; $43
    asr $12                     ; $44
    taz                         ; $4b
    lbvc $1234                  ; $53
    asr $12,x                   ; $54
    tab                         ; $5b
    ;aug                        ; $5c FIXME: implement in assembler
    rtn #$12                    ; $62
    bsr $1234                   ; $63
    stz $12                     ; $64    (stores 0 on 65C02)
    tza                         ; $6b
    adc ($12), z                ; $72    (adc (zp) on 65C02)
    lbvs $1234                  ; $73
    stz $12, x                  ; $74    (stores 0 on 65C02)
    tba                         ; $7b
    sta ($12,s),y               ; $82
    lbra $1234                  ; $83
    sty $1234,x                 ; $8b
    lbcc $1234                  ; $93
    sta ($12), z                ; $92    (sta (zp) on 65C02)
    stx $1234,y                 ; $9b
    stz $1234                   ; $9c    (stores 0 on 65C02)
    ldz #$12                    ; $a3
    ldz $1234                   ; $ab
    lda ($12), z                ; $b2    (lda (zp) on 65C02)
    lbcs $1234                  ; $b3
    ldz $1234,x                 ; $bb
    cpz #$12                    ; $c2
    dew $12                     ; $c3
    cmp ($12), z                ; $d2    (cmp ($12) on 65C02)
    lbne $1234                  ; $d3
    cpz $12                     ; $d4
    cpz $1234                   ; $dc
    lda ($12,s),y               ; $e2
    inw $12                     ; $e3
    row $1234                   ; $eb
    sbc ($12), z                ; $f2    (sbc (zp) on 65C02)
    lbeq $1234                  ; $f3
    phw #$1234                  ; $f4
    plz                         ; $fb
    phw $1234                   ; $fc

    .endscope
.endif


; The 4502 is a superset of the 65CE02. Opcode 5c (originally a "4-byte NOP
; reserved for future expansion") has been changed to the "map" instruction,
; now using implied addressing.
;
; $5c  map
; $cb  asw abs
; $db  phz

.if (.cpu .bitand CPU_ISET_4510)
    .scope

    ; added to 65CE02
    map                         ; $5c   ("4-byte NOP reserved for future expansion" on 65CE02)
    asw $1234                   ; $cb   (wai on W65C02)
    phz                         ; $db   (stp on W65C02)

    .endscope
.endif


; TODO: MEGA65
; The m65 instruction set extends the 4502 instruction set using prefix bytes.
; Therefore, the "normal" opcode table is the same as for the 4502 cpu


; The HUC6280 is a superset of the R65C02. It adds some other instructions:

.if (.cpu .bitand CPU_ISET_HUC6280)
    .scope

    ; added to R65C02
    sxy                         ; $02
    st0 #$12                    ; $03
    st1 #$12                    ; $13
    sax                         ; $22
    st2 #$12                    ; $23
    say                         ; $42
    tma #$10                    ; $43
    bsr LABEL                   ; $44
    tam #$12                    ; $53
    csl                         ; $54
    cla                         ; $62
    tii $1234, $5678, $9abc     ; $73
LABEL:
    clx                         ; $82
    tst #$12, $34               ; $83
    clx                         ; $82
    tst #$12, $34               ; $83
    tst #$12, $3456             ; $93
    tst #$12, $34, x            ; $a3
    tst #$12, $3456, x          ; $b3
    cly                         ; $c2
    tdd $1234, $5678, $9abc     ; $c3
    tin $1234, $5678, $9abc     ; $d3
    csh                         ; $d4
    tia $1234, $5678, $9abc     ; $e3
    tai $1234, $5678, $9abc     ; $f3
    set                         ; $f4

    .endscope
.endif


.if (.cpu .bitand CPU_ISET_M740)
    .scope
    ; Mitsubishi M740 - adds new instructions to 65SC02 (but also removes some)

    jsr ($12)           ; $02
    bbs0 a, LABEL       ; $03
    bbs0 $12, LABEL     ; $07
    seb0 a              ; $0b
    seb0 $12            ; $0f
    bbc0 a, LABEL       ; $13
    bbc0 $12, LABEL     ; $17
    clb0 a              ; $1b
    clb0 $12            ; $1f
    jsr $ff12           ; $22
    bbs1 a, LABEL       ; $23
    bbs1 $12, LABEL     ; $27
    seb1 a              ; $2b
    seb1 $12            ; $2f
    bbc1 a, LABEL       ; $33
    bbc1 $12, LABEL     ; $37
    clb1 a              ; $3b
    clb1 $12            ; $3f
    stp                 ; $42
    bbs2 a, LABEL       ; $43
    com $12             ; $44
    bbs2 $12, LABEL     ; $47
    seb2 a              ; $4b
    seb2 $12            ; $4f
    bbc2 a, LABEL       ; $53
    bbc2 $12, LABEL     ; $57
    clb2 a              ; $5b
    clb2 $12            ; $5f
    bbs3 a, LABEL       ; $63
    bbs3 $12, LABEL     ; $67
    seb3 a              ; $6b
    seb3 $12            ; $6f
    bbc3 a, LABEL       ; $73
    bbc3 $12, LABEL     ; $77
    clb3 a              ; $7b
    clb3 $12            ; $7f
LABEL:
    rrf $12             ; $82
    bbs4 a, LABEL       ; $83
    bbs4 $12, LABEL     ; $87
    seb4 a              ; $8b
    seb4 $12            ; $8f
    bbc4 a, LABEL       ; $93
    bbc4 $12, LABEL     ; $97
    clb4 a              ; $9b
    clb4 $12            ; $9f
    bbs5 a, LABEL       ; $a3
    bbs5 $12, LABEL     ; $a7
    seb5 a              ; $ab
    seb5 $12            ; $af
    bbc5 a, LABEL       ; $b3
    bbc5 $12, LABEL     ; $b7
    clb5 a              ; $bb
    clb5 $12            ; $bf
    slw                 ; $c2
    bbs6 a, LABEL       ; $c3
    bbs6 $12, LABEL     ; $c7
    seb6 a              ; $cb
    seb6 $12            ; $cf
    bbc6 a, LABEL       ; $d3
    bbc6 $12, LABEL     ; $d7
    clb6 a              ; $db
    clb6 $12            ; $df
    fst                 ; $e2
    bbs7 a, LABEL       ; $e3
    bbs7 $12, LABEL     ; $e7
    seb7 a              ; $eb
    seb7 $12            ; $ef
    bbc7 a, LABEL       ; $f3
    bbc7 $12, LABEL     ; $f7
    clb7 a              ; $fb
    clb7 $12            ; $ff

    ; replaced from 65SC02
    clt                 ; $12
    set                 ; $32
    ldm $12, #$34       ; $3c
    tst $12             ; $64

    ; removed from 65SC02 (replaced by new)
    ; ora ($12)         ; $12
    ; and ($12)         ; $32
    ; bit $1234,x       ; $3c
    ; stz $12           ; $64

    ; removed from 65SC02
    ; tsb $12           ; $04
    ; tsb $1234         ; $0c
    ; trb $12           ; $14
    ; trb $1234         ; $1c
    ; bit $12,y         ; $34
    ; eor ($12)         ; $52
    ; phy               ; $5a
    ; adc ($12)         ; $72
    ; stz $12,y         ; $74
    ; ply               ; $7a
    ; jmp ($1234)       ; $7c
    ; bit #$12          ; $89
    ; sta ($12)         ; $92
    ; stz $1234         ; $9c
    ; stz $1234,x       ; $9e
    ; cmp ($12)         ; $d2
    ; phx               ; $da
    ; sbc ($12)         ; $f2
    ; plx               ; $fa

    .endscope
.endif

