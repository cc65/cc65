; c39.s
        
        .export _timer_a_handler, _set_byte_zp, _get_byte_zp
        .export _install_timer_a_irq_handler, _install_timer_b_irq_handler
        .export _install_serial_status_irq_handler
        
        .export _cts_led_on, _cts_led_off
        .export _dsr_led_on, _dsr_led_off
        .export _rlsd_led_on, _rlsd_led_off
        .export _R6746_led_on, _R6746_led_off
        
        .export _serial_check_break
        .export _serial_enable_status_irq
        .export _serial_disable_status_irq

        .import _timer_a_irq_handler_c
        .import _timer_b_irq_handler_c
        .import _serial_status_irq_handler_c
        
        .include "c39.inc"
        .include "c39_sfrs.inc"
        
.proc _timer_a_handler
        psh     
        lda P_TASC ; clears irq
        jsr _timer_a_irq_handler_c
L2:     
        pul
        rti
.endproc

.proc _timer_b_handler
        psh
        
        ; Timer B Interrupt Flag. Status bit. Reading or writing
        ; clears the Timer B Interrupt Flag (TBM7).
        lda P_TBSC ; clears irq

        jsr _timer_b_irq_handler_c
        
        pul
        rti
.endproc

.proc _serial_status_irq_handler
        psh
        jsr _serial_status_irq_handler_c
        pul
        rti
.endproc
        
        ;; x is not passed in fastcall!!
.proc _set_byte_zp
        brk
        sta 0,x
        rts
.endproc

.proc _get_byte_zp
        brk
        lda 0,x
        rts
.endproc

        ;; $0108, reserved in boot code..
        
SER_STATUS_IRQ_RAM_LO = $0108
SER_STATUS_IRQ_RAM_HI = $0109

.proc _install_serial_status_irq_handler
        lda     #<_serial_status_irq_handler
        sta     SER_STATUS_IRQ_RAM_LO
        lda     #>_serial_status_irq_handler
        sta     SER_STATUS_IRQ_RAM_HI
        rts
.endproc
        
.proc _install_timer_a_irq_handler
        lda     #<_timer_a_handler
        sta     TIMER_A_RAM_LO
        lda     #>_timer_a_handler
        sta     TIMER_A_RAM_HI
        lda     TIMER_A_RAM_LO
        lda     TIMER_A_RAM_HI
        rts
.endproc

.proc _install_timer_b_irq_handler
        lda     #<_timer_b_handler
        sta     TIMER_B_RAM_LO
        lda     #>_timer_b_handler
        sta     TIMER_B_RAM_HI
        lda     TIMER_B_RAM_LO
        lda     TIMER_B_RAM_HI
        rts
.endproc

.proc _dsr_led_on
        rmb     #$00, P_C
        rts
.endproc
        
.proc _dsr_led_off
        smb     #$00, P_C
        rts
.endproc
        
.proc _cts_led_on
        rmb     #$01, P_C
        rts
.endproc
        
.proc _cts_led_off
        smb     #$01, P_C
        rts
.endproc

        ;;not connected to led on creatix
        
.proc _rlsd_led_on
        rmb     #$02, P_C
        rts
.endproc
        
.proc _rlsd_led_off
        smb     #$02, P_C
        rts
.endproc

;; R6760 modems also have an led on this pin..
        
.proc _R6746_led_on
        rmb     #$0, P_E
        rts
.endproc
        
.proc _R6746_led_off
        smb     #$0, P_E
        rts
.endproc
        
        ;; ok, break is received form usb serial port..
.proc _serial_check_break
        lda #0
        bbr #BIT_SIN_BREAK_DETECTED, SERIAL_STATUS, L1
        lda SERIAL_STATUS
L1:     
        rts
.endproc

.proc _serial_enable_status_irq
        smb #BIT_SIR_SIN_STATUS_ENA, SERIAL_SIR
        rts
.endproc

.proc _serial_disable_status_irq
        rmb #BIT_SIR_SIN_STATUS_ENA, SERIAL_SIR
        rts
.endproc
        
