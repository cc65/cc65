; -*- asm -*-

.if .not .defined(C39_INC)
C39_INC = 1
        
TIMER_A_RAM_LO = $0102
TIMER_A_RAM_HI = $0103  

TIMER_B_RAM_LO = $0106
TIMER_B_RAM_HI = $0107

; enable only one of these!        
;XTAL_52_416    = 1
XTAL_14_7456   = 1
;XTAL_9_8304    = 1

.define BIT_SIR_SIN_STATUS_ENA 2
.define BIT_SIR_SIN_STATUS     7
        
.define BIT_SIN_BUFFER_FULL    0
.define BIT_SIN_OVERRUN_ERROR  1
.define BIT_SIN_PARITY_ERROR   2
.define BIT_SIN_FRAMING_ERROR  3
.define BIT_SIN_BREAK_DETECTED 4
.define BIT_SOUT_BUFFER_EMPTY  5
.define BIT_SOUT_UNDERRUN      6
.define BIT_SIN_PARIITY_BIT    7

.endif
        
