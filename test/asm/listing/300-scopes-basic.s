
        .org $800

        symbol1 := *                    ; ::symbol1
        .addr symbol1                   ; ::symbol1


.scope scope1
        symbol1 := *                    ; ::scope1::symbol1

        .addr symbol1                   ; ::scope1::symbol1
        .addr ::symbol1                 ; ::symbol1
.endscope

        .addr symbol1                   ; ::symbol1
        .addr scope1::symbol1           ; ::scope1::symbol
