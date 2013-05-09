;
; Oliver Schmidt, 24.03.2005
;
; unsigned char __fastcall__ dio_write (dhandle_t handle, unsigned sect_num, const void *buffer);
;

        .export         _dio_write
        .import         dioprolog, diocommon

        .include        "mli.inc"

_dio_write:
        jsr     dioprolog
        lda     #WRITE_BLOCK_CALL
        jmp     diocommon
