;
; Oliver Schmidt, 24.03.2005
;
; unsigned char __fastcall__ dio_read (dhandle_t handle, unsigned sect_num, void *buffer);
;

        .export         _dio_read
        .import         dioprolog, diocommon

        .include        "mli.inc"

_dio_read:
        jsr     dioprolog
        lda     #READ_BLOCK_CALL
        jmp     diocommon
