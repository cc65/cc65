;
; jede jede@oric.org 2017-01-22
;

    .export         _read

    .import         popax

    .include        "zeropage.inc"
    .include        "telestrat.inc"

;int read (int fd, void* buf, unsigned count);

.proc   _read

    sta     ptr1           ; Count
    stx     ptr1+1         ; Count
    jsr     popax          ; Get buf

    sta     PTR_READ_DEST
    stx     PTR_READ_DEST+1
    sta     ptr2           ; In order to calculate nb of bytes read
    stx     ptr2+1 ;


    lda     ptr1 ;
    ldy     ptr1+1 ;
    BRK_TELEMON     XFREAD ; calls telemon30 routine
    ;  Compute nb of bytes read
    lda     PTR_READ_DEST+1
    sec
    sbc     ptr2+1
    tax
    lda     PTR_READ_DEST
    sec
    sbc     ptr2
    ; Here A and X contains number of bytes read
    rts
.endproc
