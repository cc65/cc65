;
; Oliver Schmidt, 31.03.2005
;
; unsigned __fastcall__ dio_query_sectcount (dhandle_t handle);
;

        .export         _dio_query_sectcount
        .import         _dio_query_sectsize, _malloc, _free

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "mli.inc"

_dio_query_sectcount:

        ; Set handle
        sta     mliparam + MLI::ON_LINE::UNIT_NUM

        ; Get ProDOS 8 block size (clears __oserror)
        jsr     _dio_query_sectsize

        ; Alloc buffer
        jsr     _malloc
        sta     ptr4
        stx     ptr4+1

        ; Set buffer
        sta     mliparam + MLI::ON_LINE::DATA_BUFFER
        stx     mliparam + MLI::ON_LINE::DATA_BUFFER+1

        ; Check buffer (hibyte is enough)
        txa
        beq     nomem

        ; Get device state
        lda     #ON_LINE_CALL
        ldx     #ON_LINE_COUNT
        jsr     callmli
        bcs     check

        ; UNIT_NUM already set
        .assert MLI::RW_BLOCK::UNIT_NUM = MLI::ON_LINE::UNIT_NUM, error

        ; DATA_BUFFER already set
        .assert MLI::RW_BLOCK::DATA_BUFFER = MLI::ON_LINE::DATA_BUFFER, error

        ; Read volume directory key block
        tax                     ; A = 0
        lda     #$02
        sta     mliparam + MLI::RW_BLOCK::BLOCK_NUM
        stx     mliparam + MLI::RW_BLOCK::BLOCK_NUM+1
        lda     #READ_BLOCK_CALL
        ldx     #RW_BLOCK_COUNT
        jsr     callmli
        bcs     oserr

        ; Get and save total blocks from volume directory header
        ldy     #$29
        lda     (ptr4),y
        pha
        iny
        lda     (ptr4),y
        pha

        ; Cleanup buffer
done:   lda     ptr4
        ldx     ptr4+1
        jsr     _free

        ; Restore total blocks and return
        pla
        tax
        pla
        rts

nomem:  lda     #$FF            ; Error code for sure not used by MLI
oserr:  sta     __oserror

        ; Save total blocks for failure
        lda     #$00
        pha
        pha
        beq     done            ; Branch always

        ; Check for non-ProDOS disk
check:  cmp     #$52            ; "Not a ProDOS volume"
        bne     oserr
        sta     __oserror

        ; Save total blocks for a 16-sector disk
        lda     #<280
        pha
        lda     #>280
        pha
        bne     done            ; Branch always
