;
;GEOS macros
;reassembled for 6502TASM/MMS by Maciej 'YTM/Elysium' Witkowiak
;4-2-99

;28-6-99 - ca65 port
;macro 'sub' renamed to 'ssub' due to 65816 mnemonics

;-------------------------------------------------------------------------

.macro  LoadB dest, value
        lda #value
        sta dest
.endmacro

;-------------------------------------------------------------------------

.macro  LoadW dest, value
        LoadB dest+1, >(value)
        LoadB dest+0, <(value)
.endmacro

;-------------------------------------------------------------------------

.macro  MoveB source, dest
        lda source
        sta dest
.endmacro

;-------------------------------------------------------------------------

.macro  MoveW source, dest
        MoveB source+1, dest+1
        MoveB source+0, dest+0
.endmacro

;-------------------------------------------------------------------------

.macro  add source
        clc
        adc source
.endmacro

;-------------------------------------------------------------------------

.macro  AddB source, dest
        lda source
        add dest
        sta dest
.endmacro

;-------------------------------------------------------------------------

.macro  AddW source, dest
        AddB source+0, dest+0
        lda source+1
        adc dest+1
        sta dest+1
.endmacro

;-------------------------------------------------------------------------

.macro  AddVB value, dest
        lda dest
        clc
        adc #(value)
        sta dest
        .endmacro

;-------------------------------------------------------------------------

.macro  AddVW value, dest
.local Skip
        AddVB <(value), dest+0
        .if (>(value))=0
            bcc Skip
            inc dest+1
        .else
            lda #>(value)
            adc dest+1
            sta dest+1
        .endif
Skip:
.endmacro

;-------------------------------------------------------------------------

.macro  ssub source
        sec
        sbc source
.endmacro

;-------------------------------------------------------------------------

.macro  SubB source, dest
        lda dest
        ssub source
        sta dest
.endmacro

;-------------------------------------------------------------------------

.macro  SubW source, dest
        SubB source+0, dest+0
        lda dest+1
        sbc source+1
        sta dest+1
.endmacro

;-------------------------------------------------------------------------

.macro  SubVW value, dest
        sec
        lda dest+0
        sbc #<(value)
        sta dest+0
        lda dest+1
        sbc #>(value)
        sta dest+1
.endmacro

;-------------------------------------------------------------------------

.macro  CmpB source, dest
        lda source
        cmp dest
.endmacro

;-------------------------------------------------------------------------

.macro  CmpBI source, immed
        lda source
        cmp #(immed)
.endmacro

;-------------------------------------------------------------------------

.macro  CmpW source, dest
.local Skip     
        CmpB source+1, dest+1
        bne Skip
        CmpB source+0, dest+0
Skip:
.endmacro

;-------------------------------------------------------------------------

.macro  CmpWI source, immed
.local Skip
        CmpBI source+1, >(immed)
        bne Skip
        CmpBI source+0, <(immed)
Skip:
.endmacro

;-------------------------------------------------------------------------

.macro  PushB source
        lda source
        pha
.endmacro

;-------------------------------------------------------------------------

.macro  PushW source
        PushB source+1
        PushB source+0
.endmacro

;-------------------------------------------------------------------------

.macro  PopB dest
        pla
        sta dest
.endmacro

;-------------------------------------------------------------------------

.macro  PopW dest
        PopB dest+0
        PopB dest+1
.endmacro

;-------------------------------------------------------------------------

.macro  smb bitNumber, dest
        pha
        lda #(1 << bitNumber)
        ora dest
        sta dest
        pla
.endmacro

;-------------------------------------------------------------------------

.macro  smbf bitNumber, dest
        lda #(1 << bitNumber)
        ora dest
        sta dest
.endmacro

;-------------------------------------------------------------------------

.macro  rmb bitNumber, dest
        pha
        lda #(1 << bitNumber) ^ $ff 
        and dest
        sta dest
        pla
.endmacro

;-------------------------------------------------------------------------

.macro  rmbf bitNumber, dest
        lda #(1 << bitNumber) ^ $ff
        and dest
        sta dest
.endmacro

;-------------------------------------------------------------------------

.macro  bbs bitNumber, source, addr
.local Skip
        php
        pha
        lda source
        and #(1 << bitNumber)
        beq Skip
        pla
        plp
        bra addr
Skip:   pla
        plp
.endmacro

;-------------------------------------------------------------------------

.macro  bbsf bitNumber, source, addr
        .if     bitNumber=7
                bit source
                bmi addr
        .else
                .if bitNumber=6
                    bit source
                    bvs addr
                .else
                    lda source
                    and #(1 << bitNumber)
                    bne addr
                .endif
        .endif
.endmacro

;-------------------------------------------------------------------------

.macro  bbr bitNumber, source, addr
.local Skip
        php
        pha
        lda source
        and #(1 << bitNumber)
        bne Skip
        pla
        plp
        bra addr
Skip:   pla
        plp
.endmacro

;-------------------------------------------------------------------------

.macro  bbrf bitNumber, source, addr
        .if bitNumber=7
            bit source
            bpl addr
        .else
                .if bitNumber=6
                    bit source
                    bvc addr
                .else
                    lda source
                    and #(1 << bitNumber)
                    beq addr
                .endif
        .endif
.endmacro

;-------------------------------------------------------------------------

;03.03.99 by Maciej Witkowiak

;-------------------------------------------------------------------------

.macro  addv value
        clc
        adc #(value)
.endmacro

;-------------------------------------------------------------------------

.macro  subv value
        sec
        sbc #(value)
.endmacro

;-------------------------------------------------------------------------

.macro  bnex addr
        txa
        bne addr
.endmacro

;-------------------------------------------------------------------------

.macro  beqx addr
        txa
        beq addr
.endmacro

;-------------------------------------------------------------------------
