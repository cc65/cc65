;
; Colin Leroy-Mira, 06.03.2025
;
; Copy the LC segment from the end of the binary to the Language Card
; using _memcpy. This allows running apple2 programs on the original
; Integer ROM Apple ][.
;

        .export         bltu2

        .import         _memcpy, pushax
        .import         __ONCE_LOAD__, __ONCE_SIZE__    ; Linker generated
        .import         __LC_START__, __LC_LAST__       ; Linker generated

        .segment        "ONCE"

bltu2:
        ; Get the destination start address.
        lda     #<__LC_START__
        ldx     #>__LC_START__
        jsr     pushax

        ; Get the source start address.
        lda     #<(__ONCE_LOAD__ + __ONCE_SIZE__)
        ldx     #>(__ONCE_LOAD__ + __ONCE_SIZE__)
        jsr     pushax

        ; Set the length
        lda     #<(__LC_LAST__ - __LC_START__)
        ldx     #>(__LC_LAST__ - __LC_START__)

        ; And do the copy
        jmp     _memcpy
