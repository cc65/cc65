;
; jede jede@oric.org 2017-01-22
;

    .export         _read

    .import         popax

    .include        "zeropage.inc"
    .include        "telestrat.inc"
    .include        "fcntl.inc"

;int read (int fd, void* buf, unsigned count);

.proc   _read
    sta     ptr1             ; count
    stx     ptr1+1           ; count
    jsr     popax            ; get buf

    sta     PTR_READ_DEST
    stx     PTR_READ_DEST+1
    sta     ptr2             ; in order to calculate nb of bytes read
    stx     ptr2+1 ;

    jsr     popax            ; fp pointer don't care in this version
    cpx     #$00
    bne     @is_not_stdin
    cmp     #STDIN_FILENO
    bne     @is_not_stdin
    ; stdin
@L1:
    BRK_TELEMON XRD0         ; waits until key is pressed
    bcs     @L1
    
    rts 

@is_not_stdin:
    lda     ptr1 ;
    ldy     ptr1+1 ;
    BRK_TELEMON     XFREAD ; calls telemon30 routine
    ;  compute nb of bytes read
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
