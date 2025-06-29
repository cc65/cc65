.setcpu "SWEET16"

    RTN                 ; $00      Return to 6502 code.
    BR   LABEL          ; $01 ea   Unconditional Branch.
    BNC  LABEL          ; $02 ea   Branch if Carry=0.
    BC   LABEL          ; $03 ea   Branch if Carry=1.
    BP   LABEL          ; $04 ea   Branch if last result positive.
    BM   LABEL          ; $0S ea   Branch if last result negative.
    BZ   LABEL          ; $06 ea   Branch if last result zero.
    BNZ  LABEL          ; $07 ea   Branch if last result non-zero.
    BM1  LABEL          ; $08 ea   Branch if last result = -1.
    BNM1 LABEL          ; $09 ea   Branch if last result not -1.
    BK                  ; $0A      Execute 6502 BRK instruction.
    RS                  ; $0B      Return from SWEET-16 subroutine.
    BS   LABEL          ; $0C ea   Call SWEET-16 subroutine.
LABEL:

.repeat 16, count
    SET  count,$1234    ; $1n lo hi  Rn <-- value.
    LD   count          ; $2n        R0 <-- (Rn).
    ST   count          ; $3n        Rn <-- (R0).
    LD   @count         ; $4n        MA = (Rn), ROL <-- (MA), Rn <-- MA+1, R0H <-- 0.
    ST   @count         ; $5n        MA = (Rn), MA <-- (R0L), Rn <-- MA+1.
    LDD  @count         ; $6n        MA = (Rn), R0 <-- (MA, MA+1), Rn <-- MA+2.
    STD  @count         ; $7n        MA = (Rn), MA,MA+l <-- (R0), Rn <-- MA+2.
    POP  @count         ; $8n        MA = (Rn)-1, R0L <-- (MA), R0H <-- 0, Rn <-- MA.
    STP  @count         ; $9n        MA <-- (Rn)-1, (MA) <-- R0L, Rn <-- MA.
    ADD  count          ; $An        R0 <-- (R0) + (Rn).
    SUB  count          ; $Bn        R0 <-- (R0) - (Rn).
    POPD @count         ; $Cn        MA = (Rn)-2, MA,MA+l <-- R0, Rn <-- MA.
    CPR  count          ; $Dn        R13 <-- (R0) - (Rn), R14 <-- status flags.
    INR  count          ; $En        Rn <-- (Rn) + 1.
    DCR  count          ; $Fn        Rn <-- (Rn) - 1.
.endrepeat
