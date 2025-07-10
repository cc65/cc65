
; Error: Arguments to .CAPABILITY must be identifiers
.if .cap()
.endif

; Error: Arguments to .CAPABILITY must be identifiers
; Error: ')' expected
.if .cap(
.endif

; Error: Not a valid capability name: CPU_HAS_BR
.if .cap(cpu_has_br)
.endif

; Error: ')' expected
; Error: Unexpected trailing garbage characters
.if .cap(cpu_has_bra8 cpu_has_bra8)
.endif

; Ok
.if .cap(cpu_has_bra8, CPU_HAS_PUSHXY, CPU_HAS_STZ, CPU_HAS_INA)
.endif

.setcpu "65SC02"
.if !.cap(cpu_has_bra8)
.error "Assembler says 65SC02 has no 8 bit bra"
.endif
.if !.cap(cpu_has_PUSHXY)
.error "Assembler says 65SC02 has no phx"
.endif
.if !.cap(cpu_has_STZ)
.error "Assembler says 65SC02 has no stz"
.endif
.if !.cap(cpu_has_INA)
.error "Assembler says 65SC02 has no ina"
.endif



