;
; Oliver Schmidt, 30.12.2004
; Robert Justice, 2026
;
; File name handling for SOS file I/O
;

        .export         pushname_tos, pushname, popname
        .import         popax, subysp, addysp, decsp1, incsp1

        .include        "zeropage.inc"
        .include        "apple3.inc"
        .include        "sos.inc"

pushname_tos:
        jsr     popax
pushname:
        sta     ptr1
        stx     ptr1+1

        ; Alloc pathname buffer
        ldy     #FILENAME_MAX
        sty     sosparam + SOS::PREFIX::LENGTH
        jsr     subysp

        ; Check for full pathname
        ldy     #$00
        lda     (ptr1),y
        cmp     #'/'
        beq     copy
        
        ; Check for device pathname
        cmp     #'.'  
        beq     copy

        ; Check for system prefix
        ; Use allocated pathname buffer
        jsr     decsp1           ; To align with setlen below
        lda     c_sp
        ldx     c_sp+1
        sta     sosparam + SOS::PREFIX::PATHNAME
        stx     sosparam + SOS::PREFIX::PATHNAME+1
        ; Get prefix - returns with trailing slash
        lda     #GET_PREFIX_CALL
        ldx     #GET_PREFIX_COUNT
        jsr     callsos
        bne     addsp65

        ; Get prefix length
        ldy     #0
        lda     (c_sp),y
        beq     invpath
        jsr     incsp1           ; Restore sp

        ; Adjust source pointer for copy
        tay
        sta     tmp1
        lda     ptr1
        sec
        sbc     tmp1
        bcs     :+
        dec     ptr1+1
:       sta     ptr1

        ; Copy source to allocated pathname buffer
copy:   lda     (ptr1),y
        sta     (c_sp),y
        beq     setlen
        iny
        cpy     #FILENAME_MAX
        bcc     copy

        ; Load oserror code
invpath:
        lda     #$40            ; "Invalid pathname"

        ; Free pathname buffer
addsp65:
        ldy     #FILENAME_MAX
        bne     addsp           ; Branch always

        ; Alloc and set length byte
setlen: tya
        jsr     decsp1          ; Preserves A
        ldy     #$00
        sta     (c_sp),y

        ; Return success
        tya
        rts

popname:
        ; Cleanup stack
        ldy     #1 + FILENAME_MAX
addsp:  jmp     addysp          ; Preserves A and X
