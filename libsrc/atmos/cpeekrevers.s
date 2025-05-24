;
; 2017-06-08, Greg King
;
; unsigned char cpeekrevers (void);
;
; Atmos version
;

        .export         _cpeekrevers

        .import         setscrptr
        .importzp       ptr2


_cpeekrevers:
        jsr     setscrptr       ; Set ptr2 and .Y to the cursor's address
        lda     (ptr2),y        ; Get char
        and     #$80            ; get reverse bit
        asl     a
        tax                     ; ldx #>$0000
        rol     a               ; return boolean value
        rts
