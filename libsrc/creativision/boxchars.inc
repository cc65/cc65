; Boxchars

boxchars:
        ; Vertical Line
        .byte   $18     
        .byte   $18
        .byte   $18
        .byte   $18
        .byte   $18
        .byte   $18
        .byte   $18
        .byte   $18

        ; Horizontal Line
        .byte   $00
        .byte   $00
        .byte   $00
        .byte   $FF
        .byte   $00
        .byte   $00
        .byte   $00
        .byte   $00

        ; Top Left
        .byte   $00
        .byte   $00
        .byte   $00
        .byte   $1F
        .byte   $18
        .byte   $18
        .byte   $18
        .byte   $18     

        ; Top Right
        .byte   $00
        .byte   $00
        .byte   $00
        .byte   $F8
        .byte   $18
        .byte   $18
        .byte   $18
        .byte   $18     

        ; Bottom Left
        .byte   $18
        .byte   $18
        .byte   $18
        .byte   $1F
        .byte   $00
        .byte   $00
        .byte   $00
        .byte   $00       

        ; Bottom Right
        .byte   $18
        .byte   $18
        .byte   $18
        .byte   $F8
        .byte   $00
        .byte   $00
        .byte   $00
        .byte   $00       
