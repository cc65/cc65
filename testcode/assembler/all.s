        .setcpu         "6502X"

; all legal and illegal opcodes as they would be disassembled by da65
; note that this would not assemble into the exact same binary

            brk                 ; 00
            ora ($12,x)         ; 01 12
            jam                 ; 02
            slo ($12,x)         ; 03 12
            nop $12             ; 04 12
            ora $12             ; 05 12
            asl $12             ; 06 12
            slo $12             ; 07 12
            php                 ; 08
            ora #$12            ; 09 12
            asl a               ; 0a
            anc #$12            ; 0b 12
            nop $1234           ; 0c 34 12
            ora $1234           ; 0d 34 12
            asl $1234           ; 0e 34 12
            slo $1234           ; 0f 34 12
            bpl *+$14           ; 10 12
            ora ($12),y         ; 11 12
            jam                 ; 12
            slo ($12),y         ; 13 12
            nop $12,x           ; 14 12
            ora $12,x           ; 15 12
            asl $12,x           ; 16 12
            slo $12,x           ; 17 12
            clc                 ; 18
            ora $1234,y         ; 19 34 12
            nop                 ; 1a
            slo $1234,y         ; 1b 34 12
            nop $1234,x         ; 1c 34 12
            ora $1234,x         ; 1d 34 12
            asl $1234,x         ; 1e 34 12
            slo $1234,x         ; 1f 34 12
            jsr $1234           ; 20 34 12
            and ($12,x)         ; 21 12
            jam                 ; 22
            rla ($12,x)         ; 23 12
            bit $12             ; 24 12
            and $12             ; 25 12
            rol $12             ; 26 12
            rla $12             ; 27 12
            plp                 ; 28
            and #$12            ; 29 12
            rol a               ; 2a
            anc #$12            ; 2b 12
            bit $1234           ; 2c 34 12
            and $1234           ; 2d 34 12
            rol $1234           ; 2e 34 12
            rla $1234           ; 2f 34 12
            bmi *+$14           ; 30 12
            and ($12),y         ; 31 12
            jam                 ; 32
            rla ($12),y         ; 33 12
            nop $12,x           ; 34 12
            and $12,x           ; 35 12
            rol $12,x           ; 36 12
            rla $12,x           ; 37 12
            sec                 ; 38
            and $1234,y         ; 39 34 12
            nop                 ; 3a
            rla $1234,y         ; 3b 34 12
            nop $1234,x         ; 3c 34 12
            and $1234,x         ; 3d 34 12
            rol $1234,x         ; 3e 34 12
            rla $1234,x         ; 3f 34 12
            rti                 ; 40
            eor ($12,x)         ; 41 12
            jam                 ; 42
            sre ($12,x)         ; 43 12
            nop $12             ; 44 12
            eor $12             ; 45 12
            lsr $12             ; 46 12
            sre $12             ; 47 12
            pha                 ; 48
            eor #$12            ; 49 12
            lsr a               ; 4a
            alr #$12            ; 4b 12
            jmp $1234           ; 4c 34 12
            eor $1234           ; 4d 34 12
            lsr $1234           ; 4e 34 12
            sre $1234           ; 4f 34 12
            bvc *+$14           ; 50 12
            eor ($12),y         ; 51 12
            jam                 ; 52
            sre ($12),y         ; 53 12
            nop $12,x           ; 54 12
            eor $12,x           ; 55 12
            lsr $12,x           ; 56 12
            sre $12,x           ; 57 12
            cli                 ; 58
            eor $1234,y         ; 59 34 12
            nop                 ; 5a
            sre $1234,y         ; 5b 34 12
            nop $1234,x         ; 5c 34 12
            eor $1234,x         ; 5d 34 12
            lsr $1234,x         ; 5e 34 12
            sre $1234,x         ; 5f 34 12
            rts                 ; 60
            adc ($12,x)         ; 61 12
            jam                 ; 62
            rra ($12,x)         ; 63 12
            nop $12             ; 64 12
            adc $12             ; 65 12
            ror $12             ; 66 12
            rra $12             ; 67 12
            pla                 ; 68
            adc #$12            ; 69 12
            ror a               ; 6a
            arr #$12            ; 6b 12
            jmp ($1234)         ; 6c 34 12
            adc $1234           ; 6d 34 12
            ror $1234           ; 6e 34 12
            rra $1234           ; 6f 34 12
            bvs *+$14           ; 70 12
            adc ($12),y         ; 71 12
            jam                 ; 72
            rra ($12),y         ; 73 12
            nop $12,x           ; 74 12
            adc $12,x           ; 75 12
            ror $12,x           ; 76 12
            rra $12,x           ; 77 12
            sei                 ; 78
            adc $1234,y         ; 79 34 12
            nop                 ; 7a
            rra $1234,y         ; 7b 34 12
            nop $1234,x         ; 7c 34 12
            adc $1234,x         ; 7d 34 12
            ror $1234,x         ; 7e 34 12
            rra $1234,x         ; 7f 34 12
            nop #$12            ; 80 12
            sta ($12,x)         ; 81 12
            nop #$12            ; 82 12
            sax ($12,x)         ; 83 12
            sty $12             ; 84 12
            sta $12             ; 85 12
            stx $12             ; 86 12
            sax $12             ; 87 12
            dey                 ; 88
            nop #$12            ; 89 12
            txa                 ; 8a
            ane #$12            ; 8b 12
            sty $1234           ; 8c 34 12
            sta $1234           ; 8d 34 12
            stx $1234           ; 8e 34 12
            sax $1234           ; 8f 34 12
            bcc *+$14           ; 90 12
            sta ($12),y         ; 91 12
            jam                 ; 92
            sha ($12),y         ; 93 12
            sty $12,x           ; 94 12
            sta $12,x           ; 95 12
            stx $12,y           ; 96 12
            sax $12,y           ; 97 12
            tya                 ; 98
            sta $1234,y         ; 99 34 12
            txs                 ; 9a
            tas $1234,y         ; 9b 34 12
            shy $1234,x         ; 9c 34 12
            sta $1234,x         ; 9d 34 12
            shx $1234,y         ; 9e 34 12
            sha $1234,y         ; 9f 34 12
            ldy #$12            ; a0 12
            lda ($12,x)         ; a1 12
            ldx #$12            ; a2 12
            lax ($12,x)         ; a3 12
            ldy $12             ; a4 12
            lda $12             ; a5 12
            ldx $12             ; a6 12
            lax $12             ; a7 12
            tay                 ; a8
            lda #$12            ; a9 12
            tax                 ; aa
            lax #$12            ; ab 12
            ldy $1234           ; ac 34 12
            lda $1234           ; ad 34 12
            ldx $1234           ; ae 34 12
            lax $1234           ; af 34 12
            bcs *+$14           ; b0 12
            lda ($12),y         ; b1 12
            jam                 ; b2
            lax ($12),y         ; b3 12
            ldy $12,x           ; b4 12
            lda $12,x           ; b5 12
            ldx $12,y           ; b6 12
            lax $12,y           ; b7 12
            clv                 ; b8
            lda $1234,y         ; b9 34 12
            tsx                 ; ba
            las $1234,y         ; bb 34 12
            ldy $1234,x         ; bc 34 12
            lda $1234,x         ; bd 34 12
            ldx $1234,y         ; be 34 12
            lax $1234,y         ; bf 34 12
            cpy #$12            ; c0 12
            cmp ($12,x)         ; c1 12
            nop #$12            ; c2 12
            dcp ($12,x)         ; c3 12
            cpy $12             ; c4 12
            cmp $12             ; c5 12
            dec $12             ; c6 12
            dcp $12             ; c7 12
            iny                 ; c8
            cmp #$12            ; c9 12
            dex                 ; ca
            axs #$12            ; cb 12
            cpy $1234           ; cc 34 12
            cmp $1234           ; cd 34 12
            dec $1234           ; ce 34 12
            dcp $1234           ; cf 34 12
            bne *+$14           ; d0 12
            cmp ($12),y         ; d1 12
            jam                 ; d2
            dcp ($12),y         ; d3 12
            nop $12,x           ; d4 12
            cmp $12,x           ; d5 12
            dec $12,x           ; d6 12
            dcp $12,x           ; d7 12
            cld                 ; d8
            cmp $1234,y         ; d9 34 12
            nop                 ; da
            dcp $1234,y         ; db 34 12
            nop $1234,x         ; dc 34 12
            cmp $1234,x         ; dd 34 12
            dec $1234,x         ; de 34 12
            dcp $1234,x         ; df 34 12
            cpx #$12            ; e0 12
            sbc ($12,x)         ; e1 12
            nop #$12            ; e2 12
            isc ($12,x)         ; e3 12
            cpx $12             ; e4 12
            sbc $12             ; e5 12
            inc $12             ; e6 12
            isc $12             ; e7 12
            inx                 ; e8
            sbc #$12            ; e9 12
            nop                 ; ea
            sbc #$12            ; eb 12
            cpx $1234           ; ec 34 12
            sbc $1234           ; ed 34 12
            inc $1234           ; ee 34 12
            isc $1234           ; ef 34 12
            beq *+$14           ; f0 12
            sbc ($12),y         ; f1 12
            jam                 ; f2
            isc ($12),y         ; f3 12
            nop $12,x           ; f4 12
            sbc $12,x           ; f5 12
            inc $12,x           ; f6 12
            isc $12,x           ; f7 12
            sed                 ; f8
            sbc $1234,y         ; f9 34 12
            isc $1234,y         ; fb 34 12
            nop $1234,x         ; fc 34 12
            sbc $1234,x         ; fd 34 12
            inc $1234,x         ; fe 34 12
            isc $1234,x         ; ff 34 12
