        .interruptor    ticktock, 24

        .include        "pce.inc"
        .include        "extzp.inc"

ticktock:
        bbr5    vdc_flags,@s1   ; not vertical-blank interrupt

        ; Increment the system tick counter.
        inc     tickcount
        bne     @s1
        inc     tickcount+1
        bne     @s1
        inc     tickcount+2
        bne     @s1
        inc     tickcount+3

@s1:    rts
