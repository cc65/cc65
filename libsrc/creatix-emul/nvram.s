; nvram.s

                .setcpu "c39-native"
        
                .include "c39.inc"
                .include "c39_sfrs.inc"

                .export __nvram_read_test
                .export __nvram_write_test

                .export __nvram_read
                .export __nvram_write
        
                .export _nvram_read_addr
                .export _nvram_write_addr
        
                .import _i2c_init, i2c_start, i2c_stop, i2c_clock, i2c_read, i2c_write
                .import _wait_10ms

.zeropage
        
_nvram_read_addr:  .res 2
_nvram_write_addr: .res 2

nvram_cmd:        .res 1
nvram_a_save:     .res 1
; not needed!?
nvram_y_save:     .res 1
; not needed!?
nvram_z_save:     .res 1

.code

;; tested with R6746 board, seems to be the same for all boards..

.proc __nvram_read_test
                jsr _i2c_init

                sti #0, _nvram_read_addr
                sti #0, _nvram_read_addr+1
                ldx #0
L1:
                jsr __nvram_read
                sta $0400,x
                inx
                cpx #64
                bne L1
                rts
.endproc

.proc __nvram_write_test
                jsr _i2c_init

                sti #0, _nvram_write_addr
                sti #0, _nvram_write_addr+1
                ldx #0
L1:
                lda $0400,x
                jsr __nvram_write
                inx
                cpx #64
                bne L1
                rts
.endproc
        
; offset from base in x

nvram_prep_cmd: 
                txa
                ada     _nvram_write_addr
                lda     #$00
                adc     _nvram_write_addr+1
                and     #$07                   ; 24C64 chip selects
                clc
                rol
                ora     nvram_cmd              ; i.e. $A0
                sta     nvram_cmd
                rts
        
nvram_prep_cmd_a1:
                txa
                ada     _nvram_read_addr
                lda     #$00
                adc     _nvram_read_addr+1
                and     #$07                   ; 24C64 chip enables
                clc
                rol
                ora     #$A1                   ; cmd with read bit
                rts

; offset from base in x

__nvram_read:
                phy
                jsr     i2c_start
                jsr     nvram_prep_cmd_a1       ; sets a to cmd
                and     #$FE                    ; bit 0 cleared, write
                jsr     i2c_write
                txa                             ; arg in x, addr offset
                ada     _nvram_read_addr
                jsr     i2c_write               ; addr
                jsr     i2c_start
                jsr     nvram_prep_cmd_a1       ; read cmd
                jsr     i2c_write
                jsr     i2c_read                ; read
                jsr     i2c_stop
                ply
                and     #$FF
                rts

__nvram_write:
                pha
                lda     #$A0
                sta     nvram_cmd
                jsr     i2c_start
                jsr     nvram_prep_cmd
                jsr     i2c_write
                txa
                ada     _nvram_write_addr
                jsr     i2c_write
                pla
                jsr     i2c_write
                jsr     i2c_stop
                phx
                ldx     #$01
                jsr     _wait_10ms
                plx
                rts

__nvram_write_if_changed:
                sty     nvram_y_save
                sta     nvram_a_save
                lda     nvram_z_save
                pha
                lda     _nvram_read_addr
                pha
                lda     _nvram_read_addr+1
                pha
                lda     _nvram_write_addr
                sta     _nvram_read_addr
                lda     _nvram_write_addr+1
                sta     _nvram_read_addr+1
                jsr     __nvram_read                    ; read
                cmp     nvram_a_save                    ; check if value changed
                beq     __nvram_write_no_change

                lda     nvram_a_save
                jsr     __nvram_write

__nvram_write_no_change:
                lda     _nvram_read_addr
                sta     _nvram_write_addr
                lda     _nvram_read_addr+1
                sta     _nvram_write_addr+1
                pla
                sta     _nvram_read_addr+1
                pla
                sta     _nvram_read_addr
                pla
                sta     nvram_z_save
                lda     nvram_a_save
                ldy     #$00
                rts
