
        .setcpu         "6502"

            adc $1234           ; 6d 34 12
            adc $1234,x         ; 7d 34 12
            adc $1234,y         ; 79 34 12
            adc $12             ; 65 12
            adc #$12            ; 69 12
            adc ($12,x)         ; 61 12
            adc $12,x           ; 75 12
            adc ($12),y         ; 71 12

            and $12             ; 25 12
            and #$12            ; 29 12
            and $1234           ; 2d 34 12
            and $1234,x         ; 3d 34 12
            and $1234,y         ; 39 34 12
            and ($12,x)         ; 21 12
            and $12,x           ; 35 12
            and ($12),y         ; 31 12

            asl $12             ; 06 12
            asl $1234           ; 0e 34 12
            asl $1234,x         ; 1e 34 12
            asl $12,x           ; 16 12
            asl a               ; 0a

            bcc *+$14           ; 90 12
            bcs *+$14           ; b0 12
            beq *+$14           ; f0 12
            bmi *+$14           ; 30 12
            bne *+$14           ; d0 12
            bpl *+$14           ; 10 12
            bvc *+$14           ; 50 12
            bvs *+$14           ; 70 12

            bit $12             ; 24 12
            bit $1234           ; 2c 34 12

            brk                 ; 00

            clc                 ; 18
            cld                 ; d8
            cli                 ; 58
            clv                 ; b8

            cmp $1234           ; cd 34 12
            cmp $1234,x         ; dd 34 12
            cmp $1234,y         ; d9 34 12
            cmp $12             ; c5 12
            cmp #$12            ; c9 12
            cmp ($12,x)         ; c1 12
            cmp $12,x           ; d5 12
            cmp ($12),y         ; d1 12

            cpx $1234           ; ec 34 12
            cpx #$12            ; e0 12
            cpx $12             ; e4 12

            cpy $1234           ; cc 34 12
            cpy #$12            ; c0 12
            cpy $12             ; c4 12

            dec $1234           ; ce 34 12
            dec $1234,x         ; de 34 12
            dec $12             ; c6 12
            dec $12,x           ; d6 12

            dex                 ; ca
            dey                 ; 88

            eor $1234           ; 4d 34 12
            eor $1234,x         ; 5d 34 12
            eor $1234,y         ; 59 34 12
            eor $12             ; 45 12
            eor #$12            ; 49 12
            eor ($12,x)         ; 41 12
            eor $12,x           ; 55 12
            eor ($12),y         ; 51 12

            inc $1234           ; ee 34 12
            inc $1234,x         ; fe 34 12
            inc $12             ; e6 12
            inc $12,x           ; f6 12

            inx                 ; e8
            iny                 ; c8

            jmp $1234           ; 4c 34 12
            jmp ($1234)         ; 6c 34 12

            jsr $1234           ; 20 34 12

            lda $1234           ; ad 34 12
            lda $1234,x         ; bd 34 12
            lda $1234,y         ; b9 34 12
            lda $12             ; a5 12
            lda #$12            ; a9 12
            lda ($12,x)         ; a1 12
            lda $12,x           ; b5 12
            lda ($12),y         ; b1 12

            ldx $1234           ; ae 34 12
            ldx $1234,y         ; be 34 12
            ldx #$12            ; a2 12
            ldx $12             ; a6 12
            ldx $12,y           ; b6 12

            ldy $1234           ; ac 34 12
            ldy $1234,x         ; bc 34 12
            ldy #$12            ; a0 12
            ldy $12             ; a4 12
            ldy $12,x           ; b4 12

            lsr $1234           ; 4e 34 12
            lsr $1234,x         ; 5e 34 12
            lsr $12             ; 46 12
            lsr $12,x           ; 56 12
            lsr a               ; 4a

            nop                 ; ea

            ora $12             ; 05 12
            ora #$12            ; 09 12
            ora $1234           ; 0d 34 12
            ora $1234,x         ; 1d 34 12
            ora $1234,y         ; 19 34 12
            ora ($12,x)         ; 01 12
            ora $12,x           ; 15 12
            ora ($12),y         ; 11 12

            pha                 ; 48
            php                 ; 08
            pla                 ; 68
            plp                 ; 28

            rol $12             ; 26 12
            rol $1234           ; 2e 34 12
            rol $1234,x         ; 3e 34 12
            rol $12,x           ; 36 12
            rol a               ; 2a
            ror $1234           ; 6e 34 12
            ror $1234,x         ; 7e 34 12
            ror $12             ; 66 12
            ror $12,x           ; 76 12
            ror a               ; 6a

            rti                 ; 40
            rts                 ; 60

            sbc $1234           ; ed 34 12
            sbc $1234,x         ; fd 34 12
            sbc $1234,y         ; f9 34 12
            sbc $12             ; e5 12
            sbc #$12            ; e9 12
            sbc ($12,x)         ; e1 12
            sbc $12,x           ; f5 12
            sbc ($12),y         ; f1 12

            sec                 ; 38
            sed                 ; f8
            sei                 ; 78

            sta $1234           ; 8d 34 12
            sta $1234,x         ; 9d 34 12
            sta $1234,y         ; 99 34 12
            sta $12             ; 85 12
            sta ($12,x)         ; 81 12
            sta $12,x           ; 95 12
            sta ($12),y         ; 91 12

            stx $1234           ; 8e 34 12
            stx $12             ; 86 12
            stx $12,y           ; 96 12

            sty $1234           ; 8c 34 12
            sty $12             ; 84 12
            sty $12,x           ; 94 12

            tax                 ; aa
            tay                 ; a8
            tsx                 ; ba
            txa                 ; 8a
            txs                 ; 9a
            tya                 ; 98
