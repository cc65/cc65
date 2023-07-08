; Shared timer for:
;   sim65-timein.s
;   sim65-timeout.s

; wait A * 100,000,000 cycles, plus small amount of overhead
wait100m:
    tay
    bne :+
    rts ; return quickly if A=0
:
    jsr wait50331648 ; 50331648
    jsr wait25165824 ; 75497472
    jsr wait12582912 ; 88080384
    jsr wait6291456  ; 94371840
    jsr wait3145728  ; 97517568
    jsr wait1572864  ; 99090432
    jsr wait786432   ; 99876864
    jsr wait98304    ; 99975168
    jsr wait24576    ; 99999744
    jsr wait192      ; 99999936
    jsr wait48       ; 99999984
    nop              ; 99999986
    nop              ; 99999988
    php              ; 99999991
    plp              ; 99999995
    dey              ; 99999997
    bne :-           ; 100000000
    rts
; Note that this branch could cross a page if poorly aligned,
; adding an additional 1 cycle per loop.
; This precision is not important for the tests used.

wait50331648:  jsr wait25165824
wait25165824:  jsr wait12582912
wait12582912:  jsr wait6291456
wait6291456:   jsr wait3145728
wait3145728:   jsr wait1572864
wait1572864:   jsr wait786432
wait786432:    jsr wait393216
wait393216:    jsr wait196608
wait196608:    jsr wait98304
wait98304:     jsr wait49152
wait49152:     jsr wait24576
wait24576:     jsr wait12288
wait12288:     jsr wait6144
wait6144:      jsr wait3072
wait3072:      jsr wait1536
wait1536:      jsr wait768
wait768:       jsr wait384
wait384:       jsr wait192
wait192:       jsr wait96
wait96:        jsr wait48
wait48:        jsr wait24
wait24:        jsr wait12
wait12:        rts
