; '*' in the operands of bit branch, block transfer and LDM instructions
; must refer to the start of the instruction, like for all other
; instructions. Branch offsets are relative to the end of the instruction.

.setcpu "w65c02"
        bbr0    $12, *          ; 0F 12 FD  (branch to itself)
        bbs7    $34, *+3        ; FF 34 00  (branch to next instruction)
        bbr3    $56, *+130      ; 3F 56 7F  (largest forward offset)
L0:     bbs1    $78, L0         ; 9F 78 FD  (same, with a label)

.setcpu "huc6280"
L1:     tii     *-L1+$1234, $2000, 1    ; 73 34 12 00 20 01 00
L2:     tdd     $1000, *-L2, *-L2+1     ; C3 00 10 00 00 01 00

.setcpu "m740"
L3:     bbs0    a, L3           ; 03 FE  (2 byte instruction)
        bbc1    a, *+2          ; 33 00
L4:     ldm     $12, #*-L4      ; 3C 12 00
        bbs2    $20, *          ; 47 20 FD
