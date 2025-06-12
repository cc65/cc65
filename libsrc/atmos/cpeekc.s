;
; 2016-02-28, Groepaz
; 2017-06-19, Greg King
;
; char cpeekc (void);
;
; Atmos version
;

        .export         _cpeekc

        .import         setscrptr
        .importzp       ptr2


_cpeekc:
        jsr     setscrptr       ; Set ptr2 and .Y to the cursor's address
        lda     (ptr2),y        ; Get char
        and     #<~$80          ; Remove revers() bit
        ldx     #>$0000
        rts
