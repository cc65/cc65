; wait.s

                .include "c39.inc"

                .export _wait_1ms
                .export _wait_10ms

;; count loops in x

.code

_wait_10ms:
                phy
                ldy     #100
                bra     wait_enter1
_wait_1ms:                                
                phy
                ldy     #010
wait_enter1:                            
                jsr     wait_sub
                dey
                bne     wait_enter1
                ply
                rts
wait_sub:                               
                phx
                phy
wait_loop00:
.if .defined(XTAL_52_416)
                ldy     #163    ; R6746/R6750/R6760, cpu clock only 52.416 / 4 = 13.104 MHz!?
.elseif .defined(XTAL_14_7456)
                ldy     #184    ; CREATIX L39
.elseif .defined(XTAL_9_8304)
                ldy     #122    ; SIMBA C39
.else
.error "INVALID XTAL"
.endif
        
wait_loop11:                            
                bra     wait_loop_enter
wait_loop_enter:                        
                dey
                bne     wait_loop11
                dex                     ; counter arg
                bne     wait_loop00
                ply
                plx
                rts
