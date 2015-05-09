; init_proc_creatix.s - ca65, creatix modem..

                .setcpu "c39-native"

                .include "c39.inc"
        
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

PTG_LO = $00
        
;; XTAL: 9.8304 MHz     
PTG_HI_4800_9_8304   = $0C
PTG_HI_9600_9_8304   = $18
PTG_HI_19200_9_8304  = $30
PTG_HI_38400_9_8304  = $60
        
;; XTAL: 14.7456 MHz    
PTG_HI_4800_14_7456   = $08
PTG_HI_9600_14_7456   = $10
PTG_HI_19200_14_7456  = $20
PTG_HI_38400_14_7456  = $40

                .code
        
init_serial3:
                ; PTGs
                ; XTAL: 14.7456 MHz

                ; 20 00  -> 19200

                ;; 16#2000 = 8192
                ;; 8192  * ((14745600) / 2^17) =  921600
                ;; UIB = 921600 / (3 * (15+1)) = 19200

                ; 40 00 -> 38400

                ;; 16#4000 = 16384
                ;; 16384  * ((14745600) / 2^17) =  1843200
                ;; UIB = 1843200 / (3 * (15+1)) = 38400
        
                ; 80 00 -> 76800 ???
        
                ;; PTGA
                lda     #PTG_LO
                sta     $0035           ; PAB

.if .defined(XTAL_14_7456)
                lda     #PTG_HI_38400_14_7456
.elseif .defined(XTAL_9_8304)      
                lda     #PTG_HI_38400_9_8304
.else
                .error "Invalid XTAL"
.endif
        
                sta     $0037           ; PAB

                ;; PTGB
                lda     #PTG_LO
                sta     $000D           ; PBB

.if .def (XTAL_14_7456)
                lda     #PTG_HI_38400_14_7456
.elseif .def (XTAL_9_8304)      
                lda     #PTG_HI_38400_9_8304
.else
                .error "Invalid XTAL"
.endif

                sta     $000F           ; PBB

                sti     #$0F, $003E   ; SOUT (RxD) divider latch
                sti     #$0F, $003F   ; SIN (TxD) divider latch

                sti     #$00, $0034   ; PTG A mode
                sti     #$00, $000C   ; PTG B Mode
                rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        
init_serial:
                jsr     serial_irq_disable_0
                jsr     serial_irq_disable_1
        
                rmb     #$02, $0010   ; Timer A Mode, divide by 32 off

                rmb     #$00, $0034   ; PTG A mode
                rmb     #$01, $0034   ; PTG A mode
                rmb     #$06, $0034   ; PTG A mode
                rmb     #$00, $000C   ; PTG B Mode
                rmb     #$01, $000C   ; PTG B Mode
                rmb     #$06, $000C   ; PTG B Mode

                rmb     #$05, $003A   ; serial mode register, use ptgs

                jsr     init_serial3

                sti     #$83, $003B   ; serial line control register, PARITY STUFF BIT | 8 BITS 

                smb     #$06, $003A   ; serial mode register, serial in on pa2, rxd on

                lda     $0038         ; serial I/O buffers

                smb     #$07, $003A   ; serial mode register, serial out on pa6, txt on

                rmb     #$06, $0010   ; Timer A Mode, clear irq enable
                rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

serial_irq_disable_0: 
                rmb     #$00, $0039   ; serial interrupt enable
                rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

serial_irq_enable_1:
                smb     #$01, $0039   ; serial interrupt enable
                smb     #$03, $0064
                rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

serial_irq_disable_1:
                rmb     #$01, $0039   ; serial interrupt enable
                rmb     #$03, $0064
                rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

serial_irq_enable_x:
                rmb     #$01, $0039   ; serial interrupt enable
                bbr     #$03, $0064, serial_irq_enable_x_done
                smb     #$01, $0039   ; serial interrupt enable

serial_irq_enable_x_done:
                rts

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

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
        
.proc init_proc
                sei
                ldx     #$FF
                txs
                cld

                sti     #$00, $0032   ; host control register
                sti     #$C0, $0000   ; Port A 
                sti     #$FF, $0001   ; Port B 
                sti     #$4F, $0007   ; Port E 

                lda     #$C0
                sta     $0004         ; Port D DATA AND DIRECTION

                sti     #$C0, $0005   ; Port B Select
                sti     #$6F, $0006   ; Port C Direction
                sti     #$10, $0003   ; Port A Direction
                lda     #$6F
                sta     $0008         ; Port E Direction/Mask option
                sti     #$83, $003B   ; serial line control register
                sti     #$C0, $003A   ; serial mode register
                sti     #$0F, $003F   ; SIN (TxD) divider latch
                sti     #$0F, $003E   ; SOUT (RxD) divider latch
                sti     #$00, $000B   ; Clear External Interrupt
                sti     #$03, $000A   ; External Interrupt Register
                sti     #$00, $0009   ; Low Power Register

                sti     #$0D, $0033   ; chip select fast/slow 1101

                sti     #$70, $0018   ; Bank switch register 0000-1FFF (R/W)
                sti     #$71, $0019   ; Bank switch register 2000-3FFF (R/W)
                sti     #$72, $001A   ; Bank switch register 4000-5FFF (R/W)
                sti     #$73, $001B   ; Bank switch register 6000-7FFF (R/W)
                ; 16 KB RAM
                sti     #$B0, $001C   ; Bank switch register 8000-9FFF (R/W)
                sti     #$B1, $001D   ; Bank switch register A000-BFFF (R/W)
                sti     #$E0, $001E   ; Bank switch register C000-DFFF (R/W)
                sti     #$77, $001F   ; Bank switch register E000-FFFF (R/W)

                jsr init_serial

                ;;  jump to your code..
                ;jmp main
.endproc
        
