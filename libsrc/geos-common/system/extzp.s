;
; Maciej 'YTM/Elysium' Witkowiak <ytm@elysium.pl>
; 31.12.2002
;
; zeropage locations for exclusive use by the library
;

            .exportzp cursor_x, cursor_y
            .exportzp cursor_c, cursor_r

.segment        "EXTZP" : zeropage

cursor_x:
        .res 2                  ; Cursor column (0-319/639)
cursor_y:
        .res 1                  ; Cursor row    (0-199)

cursor_c:
        .res 1                  ; Cursor column (0-39/79)
cursor_r:
        .res 1                  ; Cursor row    (0-24)
