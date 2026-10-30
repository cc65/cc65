; 45GS02 Absolute X-Indexed opcodes.
; There is no Absolute Long,X addressing mode.
;
; NOTE: This test can be deleted, if it is in the way, after the dust settles.

.setcpu "45GS02"
    ; Legal Absolute,X
    aslq    $003456,x
    lsrq    $003456,x
    rolq    $003456,x
    rorq    $003456,x
    deq     $003456,x
    inq     $003456,x

    ; Illegal Absolute Long,X
    ; Must be rejected
    aslq    $123456,x
    lsrq    $123456,x
    rolq    $123456,x
    rorq    $123456,x
    deq     $123456,x
    inq     $123456,x
