; VIC sprite color for the mouse pointer

        .export         _mouse_def_pointercolor


.segment        "INIT"

_mouse_def_pointercolor:

        .byte       $01         ; White
