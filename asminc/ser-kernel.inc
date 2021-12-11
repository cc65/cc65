;****************************************************************************
;*                                                                          *
;*                              ser-kernel.inc                              *
;*                                                                          *
;*                        Serial communication API                          *
;*                                                                          *
;*                                                                          *
;*                                                                          *
;*(C) 2003-2006, Ullrich von Bassewitz                                      *
;*               Roemerstrasse 52                                           *
;*               D-70794 Filderstadt                                        *
;*EMail:         uz@cc65.org                                                *
;*                                                                          *
;*                                                                          *
;*This software is provided 'as-is', without any expressed or implied       *
;*warranty.  In no event will the authors be held liable for any damages    *
;*arising from the use of this software.                                    *
;*                                                                          *
;*Permission is granted to anyone to use this software for any purpose,     *
;*including commercial applications, and to alter it and redistribute it    *
;*freely, subject to the following restrictions:                            *
;*                                                                          *
;*1. The origin of this software must not be misrepresented; you must not   *
;*   claim that you wrote the original software. If you use this software   *
;*   in a product, an acknowledgment in the product documentation would be  *
;*   appreciated but is not required.                                       *
;*2. Altered source versions must be plainly marked as such, and must not   *
;*   be misrepresented as being the original software.                      *
;*3. This notice may not be removed or altered from any source              *
;*   distribution.                                                          *
;*                                                                          *
;****************************************************************************



;------------------------------------------------------------------------------
; The driver header

.struct SER_HDR
        ID      .byte   3       ; Contains 0x73, 0x65, 0x72 ("ser")
        VERSION .byte   1       ; Interface version
        LIBREF  .addr           ; Library reference
        JUMPTAB .struct
            SER_INSTALL   .addr   ; SER_INSTALL routine
            SER_UNINSTALL .addr   ; SER_UNINSTALL routine
            SER_OPEN      .addr   ; SER_OPEN routine
            SER_CLOSE     .addr   ; SER_CLOSE routine
            SER_GET       .addr   ; SER_GET routine
            SER_PUT       .addr   ; SER_PUT routine
            SER_STATUS    .addr   ; SER_STATUS routine
            SER_IOCTL     .addr   ; SER_IOCTL routine
            SER_IRQ       .addr   ; SER_IRQ routine
        .endstruct
.endstruct


;------------------------------------------------------------------------------
; The SER API version, stored SER_HDR::VERSION

SER_API_VERSION         = $02

;------------------------------------------------------------------------------
; ser_params

.struct SER_PARAMS
        BAUDRATE        .byte           ; Baudrate
        DATABITS        .byte           ; Number of data bits
        STOPBITS        .byte           ; Number of stop bits
        PARITY          .byte           ; Parity setting
        HANDSHAKE       .byte           ; Type of handshake to use
.endstruct

;------------------------------------------------------------------------------
; Serial parameters

; Baudrate
SER_BAUD_45_5           =       $00
SER_BAUD_50             =       $01
SER_BAUD_75             =       $02
SER_BAUD_110            =       $03
SER_BAUD_134_5          =       $04
SER_BAUD_150            =       $05
SER_BAUD_300            =       $06
SER_BAUD_600            =       $07
SER_BAUD_1200           =       $08
SER_BAUD_1800           =       $09
SER_BAUD_2400           =       $0A
SER_BAUD_3600           =       $0B
SER_BAUD_4800           =       $0C
SER_BAUD_7200           =       $0D
SER_BAUD_9600           =       $0E
SER_BAUD_19200          =       $0F
SER_BAUD_38400          =       $10
SER_BAUD_57600          =       $11
SER_BAUD_115200         =       $12
SER_BAUD_230400         =       $13
SER_BAUD_31250          =       $14
SER_BAUD_62500          =       $15
SER_BAUD_56_875         =       $16

; Data bit settings
SER_BITS_5              =       $00
SER_BITS_6              =       $01
SER_BITS_7              =       $02
SER_BITS_8              =       $03

; Stop bit settings
SER_STOP_1              =       $00
SER_STOP_2              =       $01

; Parity
SER_PAR_NONE            =       $00
SER_PAR_ODD             =       $01
SER_PAR_EVEN            =       $02
SER_PAR_MARK            =       $03
SER_PAR_SPACE           =       $04

; Handshake
SER_HS_NONE             =       $00    ; No handshake
SER_HS_HW               =       $01    ; Hardware (RTS/CTS) handshake
SER_HS_SW               =       $02    ; Software handshake

; Bit masks to mask out things from the status returned by ser_status
SER_STATUS_PE           =       $01     ; Parity error
SER_STATUS_FE           =       $02     ; Framing error
SER_STATUS_OE           =       $04     ; Overrun error
SER_STATUS_DCD          =       $20     ; NOT data carrier detect
SER_STATUS_DSR          =       $40     ; NOT data set ready

;------------------------------------------------------------------------------
; Variables

        .global _ser_drv                         ; Pointer to driver

;------------------------------------------------------------------------------
; Driver entry points

        .global ser_install
        .global ser_uninstall
        .global ser_open
        .global ser_close
        .global ser_get
        .global ser_put
        .global ser_status
        .global ser_ioctl
        .global ser_irq

;------------------------------------------------------------------------------
; C callable functions

        .global _ser_load_driver
        .global _ser_unload
        .global _ser_install
        .global _ser_uninstall
        .global _ser_open
        .global _ser_close
        .global _ser_get
        .global _ser_put
        .global _ser_status
        .global _ser_ioctl

        .global _ser_clear_ptr
