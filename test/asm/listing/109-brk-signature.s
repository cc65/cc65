; test of optional signature byte for BRK on all 6502-derived CPUs
; and also COP on 65C816

.setcpu "6502"
brk ; 1 byte
brk 0 ; 2 bytes
brk $60 ; 2 bytes
brk #$60 ; 2 bytes

.setcpu "6502X"
brk
brk $60
brk #$60

.setcpu "6502DTV"
brk
brk $60
brk #$60

.setcpu "65SC02"
brk
brk $60
brk #$60

.setcpu "65816"
brk
brk $60
brk #$60
cop
cop $60
cop #$60
; WDM is a NOP that gives +2 PC, probably not useful to make its signature byte optional
;wdm
wdm $60
wdm #$60

.setcpu "4510"
brk
brk $60
brk #$60

.setcpu "HuC6280"
brk
brk $60
brk #$60
