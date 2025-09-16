;
; Ullrich von Bassewitz, 05.08.1998
;
; CC65 runtime: Increment ax by 1
;

        .export         incax1

        .macpack        generic

.proc   incax1

.if .cap(CPU_HAS_INA)
        ina                     ; 65C02 version
        bne     @L9
.else
        add     #1
        bcc     @L9
.endif
        inx
@L9:    rts

.endproc
