;
; Stefan 'MonteCarlos' Andree, 26.08.2024
;
; Helper function for _cbm_set_working_screen (contains no checks, if desired screen addr is valid)
; Called by concrete implementations in the target subfolders

        .include        "cbm.inc"

        .export         cbm_update_working_screen_pointers

cbm_update_working_screen_pointers:
        sec
        jsr PLOT       ; Get cursor position ...
        clc
        jmp PLOT       ; ... and set it again to update address of char below cursor with new hi-byte
