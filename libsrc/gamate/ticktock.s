        .interruptor    ticktock, 24

        .include        "gamate.inc"
        .include        "extzp.inc"

ticktock:

        ; Increment the system tick counter.
        inc     tickcount
        bne     @s1
        inc     tickcount+1
        bne     @s1
        inc     tickcount+2
        bne     @s1
        inc     tickcount+3
@s1:
        rts
