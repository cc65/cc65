;
; Oliver Schmidt, 15.09.2009
;
; Copy the LC segment from the end of the binary to the Language Card
; using AppleSoft's BLTU2 routine.
;
        .export         bltu2

bltu2 := $D39A
