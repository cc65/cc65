; i2c.s

        .setcpu "c39-native"
        
        .include "c39.inc"
        .include "c39_sfrs.inc"

        .export _i2c_init, i2c_start, i2c_stop, i2c_clock, i2c_read, i2c_write

.zeropage

.code

;; Tested with R6746 board, seems to be the same for all boards..

;; Port D contains three I/O pins with three corresponding data
;; directional control bits, two special purpose output only ports to
;; internal connections, and three input only pins (Table 3-13 and Table 3-14).
        
;; PD0-PD2 are general purpose bidirectional input/output pins, with
;; internal pull-downs. The PD0-PD2 output level is controlled by writing
;; to bits 0-2 at address $0003 ($0004 ?), while their direction control is
;; contained in bits 4-6, respectively.

;; PD3 and PD4 are output pins reflecting internal power down status and
;; modem chip select signals, respectively.

;; PD5-PD6 are input only pins, with internal pull-ups.

;; PD7 is an input only pin.
        
.proc _i2c_init
                lda     #$C0                   ; from pwrint
                sta     port_d_dir_shadow
                sta     P_DD
                rts
.endproc
        
.proc i2c_start
                jsr     i2c_set_port_d_dir
                smb     #$01, P_A
                jsr     i2c_wait2
                smb     #$07, P_A
                jsr     i2c_wait1
                rmb     #$01, P_A
                jsr     i2c_wait1
                rmb     #$07, P_A
                jsr     i2c_wait2
                rts
.endproc

.proc i2c_stop
                jsr     i2c_set_port_d_dir
                rmb     #$01, P_A
                jsr     i2c_wait2
                smb     #$07, P_A
                jsr     i2c_wait1
                smb     #$01, P_A
                jsr     i2c_wait1
                jsr     i2c_wait2
                jmp     i2c_clr_port_d_dir
.endproc

.proc i2c_clock
                jsr     i2c_wait1
                smb     #$07, P_A      ; clock
                jsr     i2c_wait1
                rmb     #$07, P_A      ; clock
                jsr     i2c_wait2
                rts
.endproc

.proc i2c_write
                jsr     i2c_set_port_d_dir
                ldy     #$08           ; 8 bits

i2c_write_loop1:
                rol                    ; rotate into carry
                bcc     i2c_write_0
                smb     #$01, P_A      ; set data bit
                bra     i2c_write_1

i2c_write_0:
                rmb     #$01, P_A      ; clr data bit

i2c_write_1:
                jsr     i2c_clock
                dey                    ; 8 bits
                bne     i2c_write_loop1
                rol
                rmb     #$07, P_A      ; clock
                jsr     i2c_clr_port_d_dir
                lda     #$00
                jsr     i2c_wait1
                smb     #$07, P_A      ; clock
                jsr     i2c_wait1
                bbr     #$01, P_A, i2c_write_done ; ack?
                lda     #$FF

i2c_write_done:                         ; clock
                rmb     #$07, P_A
                jsr     i2c_wait2
                rts
.endproc

.proc i2c_read
                jsr     i2c_clr_port_d_dir
                phy
                ldy     #$08           ; nbits
                lda     #$00           ; clear for rotate carry in

i2c_read_loop1:
                rmb     #$07, P_A      ; clock
                jsr     i2c_wait1
                smb     #$07, P_A      ; clock
                jsr     i2c_wait1
                bbs     #$01, P_A, i2c_read_set_carry ; data in
                clc
                rol
                bra     i2c_read_clr_carry

i2c_read_set_carry:
                sec
                rol

i2c_read_clr_carry:
                dey
                bne     i2c_read_loop1
                rmb     #$07, P_A      ; clock
                ply
                rts
.endproc

;; PORT D:
;;
;; Port D contains three I/O pins with three corresponding data
;; directional control bits, two special purpose output only ports to
;; internal connections, and three input only pins (Table 3-13 and Table 3-14).
;;        
;; PD0-PD2 are general purpose bidirectional input/output pins, with
;; internal pull-downs. The PD0-PD2 output level is controlled by writing
;; to bits 0-2 at address $0003 ($0004 ?), while their direction control is
;; contained in bits 4-6, respectively.
;;        
;; PD3 and PD4 are output pins reflecting internal power down status and
;; modem chip select signals, respectively.
;;        
;; PD5-PD6 are input only pins, with internal pull-ups.
;;        
;; PD7 is an input only pin.

;; Write Control (/WC):
;;
;; The hardware Write Control pin (/WC) is useful for protecting the
;; entire contents of the memory from inadvertent erase/write. The Write
;; Control signal is used to enable (/WC=VIL) or disable (/WC=VIH) write
;; instructions to the entire memory area. When unconnected, the /WC input
;; is internally read as VIL, and write operations are allowed.  When
;; /WC=1, Device Select and Address bytes are acknowledged, Data bytes are
;; not acknowledged.
        
.proc i2c_set_port_d_dir
                php
                sei
                pha
                sba     #$02, port_d_dir_shadow ; /WDC write control of 24CXX?
                lda     port_d_dir_shadow
                sta     P_DD                    ; PORT D DATA AND DIRECTION
                pla
                plp
                rts
.endproc

.proc i2c_clr_port_d_dir
                php
                sei
                pha
                rba     #$02, port_d_dir_shadow ; /WDC write control of 24CXX?
                lda     port_d_dir_shadow
                sta     P_DD                    ; PORT D DATA AND DIRECTION
                pla
                plp
                rts
.endproc

i2c_wait1:
                jsr     i2c_wait2
                jsr     i2c_wait2
                jsr     i2c_wait2
                jsr     i2c_wait2
i2c_wait2:
                phi
                pli
                rts

.bss

port_d_dir_shadow: .res 1
