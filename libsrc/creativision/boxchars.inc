; Boxchars

boxchars:
    ; Vertical Line
    .byte       $18     
    .byte       $18
    .byte       $18
    .byte       $18
    .byte       $18
    .byte       $18
    .byte       $18
    .byte       $18

    ; Horizontal Line
    .byte       0
    .byte       0
    .byte       0
    .byte       $ff
    .byte       0
    .byte       0
    .byte       0
    .byte       0
    
    ; Top Left
    .byte       0
    .byte       0
    .byte       0
    .byte       $1f
    .byte       $18
    .byte       $18
    .byte       $18
    .byte       $18     
    
    ; Top Right
    .byte       0
    .byte       0
    .byte       0
    .byte       $F8
    .byte       $18
    .byte       $18
    .byte       $18
    .byte       $18     

    ; Bottom Left
    .byte       $18
    .byte       $18
    .byte       $18
    .byte       $1F
    .byte       0
    .byte       0
    .byte       0
    .byte       0       

    ; Bottom Right
    .byte       $18
    .byte       $18
    .byte       $18
    .byte       $F8
    .byte       0
    .byte       0
    .byte       0
    .byte       0       
