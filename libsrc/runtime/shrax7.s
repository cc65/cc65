;
; Miloslaw Smyk, 2024
;
; CC65 runtime: Scale the primary register by 128, unsigned
;

        .export         shrax7

shrax7:                         ; HXXXXXXL hAAAAAAl
        asl                     ;          AAAAAAA0, h->C
        txa
        rol                     ;          XXXXXXLh, H->C
        ldx     #$00            ; 00000000 XXXXXXLh
        bcc     :+
        inx                     ; 0000000H XXXXXXLh if C
:       rts

        ; 12 cycles max, 9 bytes
