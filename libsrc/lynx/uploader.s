        .include        "lynx.inc"
        .include        "extzp.inc"
        .interruptor    _UpLoaderIRQ
        .export         __UPLOADER__: absolute = 1

load_len=_FileDestAddr
load_ptr=_FileFileLen
load_ptr2=_FileCurrBlock

.segment "UPCODE"

ComLynxReadAndExec:
        ldy     #4
loop0:
        jsr     read_byte
        sta     load_len-1,y
        dey
        bne     loop0       ; get destination and length
        tax                 ; lowbyte of length

        lda     load_ptr
        sta     load_ptr2
        lda     load_ptr+1
        sta     load_ptr2+1

loop1:
        inx
        bne     cont1
        inc     load_len+1
        bne     cont1
        jmp     (load_ptr)

cont1:
        jsr     read_byte
        sta     (load_ptr2),y
        sta     PALETTE         ; feedback ;-)
        iny
        bne     loop1
        inc     load_ptr2+1
        bra     loop1

read_byte:
        bit     SERCTL
        bvc     read_byte
        lda     SERDAT
        rts

_UpLoaderIRQ:
        lda     INTSET
        and     #$10
        bne     @L0
        clc
        rts
@L0:
        lda     SERDAT          ; wait for the start sequence
        bit     flag            ; already seen $81 ?
        bpl     again           ; >= 0 => no
        cmp     #$50            ; "P" ?
        bne     again           ; not correct, so clear flag
        sei
        jmp     ComLynxReadAndExec

again:
        stz     flag
        cmp     #$81
        bne     exit
        sta     flag
;
; last action : clear interrupt
;
exit:
        clc
        rts

.segment "UPDATA"

flag:
        .byte   0

