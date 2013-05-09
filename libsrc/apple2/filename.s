;
; Oliver Schmidt, 30.12.2004
;
; File name handling for ProDOS 8 file I/O
;

        .export         pushname, popname
        .import         subysp, addysp, decsp1

        .include        "zeropage.inc"
        .include        "mli.inc"

pushname:
        sta     ptr1
        stx     ptr1+1

        ; Alloc pathname buffer
        ldy     #64+1           ; Max pathname length + zero
        jsr     subysp

        ; Check for full pathname
        ldy     #$00
        lda     (ptr1),y
        cmp     #'/'
        beq     copy

        ; Check for system prefix
        lda     PFIXPTR
        bne     copy

        ; Use unit number of most recent accessed device
        lda     DEVNUM
        sta     mliparam + MLI::ON_LINE::UNIT_NUM

        ; Use allocated pathname buffer
        lda     sp
        ldx     sp+1
        sta     mliparam + MLI::ON_LINE::DATA_BUFFER
        stx     mliparam + MLI::ON_LINE::DATA_BUFFER+1

        ; Get volume name
        lda     #ON_LINE_CALL
        ldx     #ON_LINE_COUNT
        jsr     callmli
        bcs     addsp65

        ; Get volume name length
        lda     (sp),y
        and     #15             ; Max volume name length

        ; Bracket volume name with slashes to form prefix
        sta     tmp1
        lda     #'/'
        sta     (sp),y
        ldy     tmp1
        iny                     ; Leading slash
        sta     (sp),y
        iny                     ; Trailing slash

        ; Adjust source pointer for copy
        sty     tmp1
        lda     ptr1
        sec
        sbc     tmp1
        bcs     :+
        dec     ptr1+1
:       sta     ptr1

        ; Copy source to allocated pathname buffer
copy:   lda     (ptr1),y
        sta     (sp),y
        beq     setlen
        iny
        cpy     #64+1           ; Max pathname length + zero
        bcc     copy

        ; Load oserror code
        lda     #$40            ; "Invalid pathname"

        ; Free pathname buffer
addsp65:ldy     #64+1
        bne     addsp           ; Branch always

        ; Alloc and set length byte
setlen: tya
        jsr     decsp1          ; Preserves A
        ldy     #$00
        sta     (sp),y

        ; Return success
        tya
        rts

popname:
        ; Cleanup stack
        ldy     #1 + 64+1       ; Length byte + max pathname length + zero
addsp:  jmp     addysp          ; Preserves A
