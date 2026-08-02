;
; Oliver Schmidt, 24.03.2005
; Robert Justice, 2026
;
; unsigned char __fastcall__ dio_write (dhandle_t handle, unsigned sect_num, const void *buffer);
;

        .export         _dio_write
        .import         dioprolog, diocommon

        .include        "sos.inc"

_dio_write:
        jsr     dioprolog
        lda     #D_WRITE_CALL
        ldx     #D_WRITE_COUNT
        jmp     diocommon
