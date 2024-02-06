.macpack cbm

start:
    lda #14
    jsr $ffd2

    ldy #0
loop:
    lda message,y
    beq done
    sta $0400,y
    iny
    bne loop
done:
    rts

message:
    scrcode "Hello, World!@"
