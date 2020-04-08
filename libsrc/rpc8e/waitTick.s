; ---------------------------------------------------------------------------
; waitTick.s
; ---------------------------------------------------------------------------
;
; Wait for interrupt (gametick, the only interrupt on this system) and return

.export  _waitTick

; ---------------------------------------------------------------------------
; Wait for interrupt:  Forces the assembler to emit a WAI opcode ($CB)
; ---------------------------------------------------------------------------

.segment  "CODE"

.proc _waitTick: near

.byte      $CB                    ; Inserts a WAI opcode
           RTS                    ; Return to caller

.endproc