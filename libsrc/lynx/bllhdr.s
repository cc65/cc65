;
; Karri Kaksonen, 2011
;
; This header is required for BLL builds.
;
        .import         __BSS_LOAD__
        .import         __MAIN_START__
        .export         __BLLHDR__: absolute = 1
 
; ------------------------------------------------------------------------
; BLL header (BLL header)

        .segment "BLLHDR"
        .word   $0880
        .dbyt   __MAIN_START__
        .dbyt   __BSS_LOAD__ - __MAIN_START__ + 10
        .byte   $42,$53
        .byte   $39,$33
