; A huge width or precision in .SPRINTF used to crash ca65.

        .byte   .sprintf("%2147483647d", 1)
        .byte   .sprintf("%.99999999999s", "x")
        .byte   .sprintf("%5d|%-4s|", 42, "ab")
