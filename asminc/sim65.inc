
; *******************************************************************************
; **                                                                           **
; **  sim65.inc : assembler definitions for the sim6502 and sim65c02 targets.  **
; **                                                                           **
; **  Sidney Cadot, January 2025                                               **
; **                                                                           **
; *******************************************************************************

            ; The '_peripherals' symbol is defined in the linker configuration
            ; file to correspond to the first address in the periperal memory
            ; aparture.
            ;
            ; We use it here as a base address for all peripheral addresses.

            .import _peripherals

; **************************************************************
; **                                                          **
; **  Define assembler symbols for the "counter" peripheral.  **
; **                                                          **
; **************************************************************

peripheral_counter_base   := _peripherals + 0

peripheral_counter_latch  := peripheral_counter_base + 0
peripheral_counter_select := peripheral_counter_base + 1
peripheral_counter_value  := peripheral_counter_base + 2

; Values for the peripheral_counter_select register.

COUNTER_SELECT_CLOCKCYCLE_COUNTER   = $00
COUNTER_SELECT_INSTRUCTION_COUNTER  = $01
COUNTER_SELECT_IRQ_COUNTER          = $02
COUNTER_SELECT_NMI_COUNTER          = $03
COUNTER_SELECT_WALLCLOCK_TIME       = $80
COUNTER_SELECT_WALLCLOCK_TIME_SPLIT = $81

; ********************************************************************
; **                                                                **
; **  Define assembler symbols for the "sim65 control" peripheral.  **
; **                                                                **
; ********************************************************************

peripheral_sim65_base       := _peripherals + 10

peripheral_sim65_cpu_mode   := peripheral_sim65_base + 0
peripheral_sim65_trace_mode := peripheral_sim65_base + 1

; Values for the peripheral_sim65_cpu_mode register.

SIM65_CPU_MODE_6502  = $00
SIM65_CPU_MODE_65C02 = $01
SIM65_CPU_MODE_6502X = $02

; Bitfield values for the peripheral_sim65_trace_mode field.

SIM65_TRACE_MODE_FIELD_INSTR_COUNTER  = $40
SIM65_TRACE_MODE_FIELD_CLOCK_COUNTER  = $20
SIM65_TRACE_MODE_FIELD_PC             = $10
SIM65_TRACE_MODE_FIELD_INSTR_BYTES    = $08
SIM65_TRACE_MODE_FIELD_INSTR_ASSEMBLY = $04
SIM65_TRACE_MODE_FIELD_CPU_REGISTERS  = $02
SIM65_TRACE_MODE_FIELD_CC65_SP        = $01

; Values for the peripheral_sim65_trace_mode field that fully disable / enable tracing.

SIM65_TRACE_MODE_DISABLE              = $00
SIM65_TRACE_MODE_ENABLE_FULL          = $7F

; ************************
; **                    **
; **  End of sim65.inc  **
; **                    **
; ************************
