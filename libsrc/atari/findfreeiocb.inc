; find a free iocb
; no entry parameters
; return ZF = 0/1 for not found/found
;        index in X if found
; all registers destroyed

.proc   findfreeiocb

        ldx     #0
        ldy     #$FF
loop:   tya
        cmp     ICHID,x
        beq     found
        txa
        clc
        adc     #$10
        tax
        cmp     #$80
        bcc     loop
        inx                     ; return ZF cleared
found:  rts

.endproc        ; findfreeiocb
