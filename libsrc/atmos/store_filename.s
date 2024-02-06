; Helper function

        .export         store_filename
        .importzp       ptr1

        .include        "atmos.inc"

store_filename:
        sta     ptr1
        stx     ptr1+1
        ldy     #FNAME_LEN - 1  ; store filename
:       lda     (ptr1),y
        sta     CFILE_NAME,y
        dey
        bpl     :-
        rts
