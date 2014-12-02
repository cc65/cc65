
        .setcpu         "6502X"

; all so called "illegal" opcodes. duplicated (functionally identical) ones
; are commented out

; first all totally stable undocs:

        slo $12             ; 07 12
        slo $1234           ; 0f 34 12
        slo $1234,x         ; 1f 34 12
        slo $1234,y         ; 1b 34 12
        slo ($12,x)         ; 03 12
        slo $12,x           ; 17 12
        slo ($12),y         ; 13 12

        rla $12             ; 27 12
        rla $1234           ; 2f 34 12
        rla $1234,x         ; 3f 34 12
        rla $1234,y         ; 3b 34 12
        rla ($12,x)         ; 23 12
        rla $12,x           ; 37 12
        rla ($12),y         ; 33 12

        sre $1234           ; 4f 34 12
        sre $1234,x         ; 5f 34 12
        sre $1234,y         ; 5b 34 12
        sre $12             ; 47 12
        sre ($12,x)         ; 43 12
        sre $12,x           ; 57 12
        sre ($12),y         ; 53 12

        rra $1234           ; 6f 34 12
        rra $1234,x         ; 7f 34 12
        rra $1234,y         ; 7b 34 12
        rra $12             ; 67 12
        rra ($12,x)         ; 63 12
        rra $12,x           ; 77 12
        rra ($12),y         ; 73 12

        dcp $1234           ; cf 34 12
        dcp $1234,x         ; df 34 12
        dcp $1234,y         ; db 34 12
        dcp $12             ; c7 12
        dcp ($12,x)         ; c3 12
        dcp $12,x           ; d7 12
        dcp ($12),y         ; d3 12

        isc $1234           ; ef 34 12
        isc $1234,x         ; ff 34 12
        isc $1234,y         ; fb 34 12
        isc $12             ; e7 12
        isc ($12,x)         ; e3 12
        isc $12,x           ; f7 12
        isc ($12),y         ; f3 12

        sax $1234           ; 8f 34 12
        sax $12             ; 87 12
        sax ($12,x)         ; 83 12
        sax $12,y           ; 97 12

        lax $1234           ; af 34 12
        lax $1234,y         ; bf 34 12
        lax $12             ; a7 12
        lax ($12,x)         ; a3 12
        lax ($12),y         ; b3 12
        lax $12,y           ; b7 12

        anc #$12            ; 0b 12
        ;anc #$12           ; 2b 12

        arr #$12            ; 6b 12

        alr #$12            ; 4b 12

        axs #$12            ; cb 12

        nop $1234           ; 0c 34 12
        nop $1234,x         ; 1c 34 12
        nop $12             ; 04 12
        nop $12,x           ; 14 12
        nop #$12            ; 80 12
        ;nop $1234,x        ; 3c 34 12
        ;nop $1234,x        ; 5c 34 12
        ;nop $1234,x        ; 7c 34 12
        ;nop $1234,x        ; dc 34 12
        ;nop $1234,x        ; fc 34 12
        ;nop $12            ; 44 12
        ;nop $12            ; 64 12
        ;nop #$12           ; 82 12
        ;nop #$12           ; 89 12
        ;nop #$12           ; c2 12
        ;nop #$12           ; e2 12
        ;nop $12,x          ; 34 12
        ;nop $12,x          ; 54 12
        ;nop $12,x          ; 74 12
        ;nop $12,x          ; d4 12
        ;nop $12,x          ; f4 12
        ;nop                ; 1a
        ;nop                ; 3a
        ;nop                ; 5a
        ;nop                ; 7a
        ;nop                ; da

        jam                 ; 02
        ;jam                ; 12
        ;jam                ; 22
        ;jam                ; 32
        ;jam                ; 42
        ;jam                ; 52
        ;jam                ; 62
        ;jam                ; 72
        ;jam                ; 92
        ;jam                ; b2
        ;jam                ; d2
        ;jam                ; f2

        ;sbc #$12           ; eb 12

; the so-called "unstable" ones:

        sha ($12),y         ; 93 12
        sha $1234,y         ; 9f 34 12

        shx $1234,y         ; 9e 34 12
        shy $1234,x         ; 9c 34 12

        tas $1234,y         ; 9b 34 12
        las $1234,y         ; bb 34 12

; the two so-called "highly unstable" ones:

        lax #$12            ; ab 12

        ane #$12            ; 8b 12
