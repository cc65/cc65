; .bss variable used by SpartaDOS-X implementations of
; gettime.s and settime.s

        .export sdxtry

        .bss

sdxtry: .res    1       ; limit of unsuccessful tries to call GETTD/SETTD
                        ; (see settime.s)
