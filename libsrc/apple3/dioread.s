;
; Oliver Schmidt, 24.03.2005
;
; unsigned char __fastcall__ dio_read (dhandle_t handle, unsigned sect_num, void *buffer);
;

        .export         _dio_read
        .import         dioprolog, diocommon

        .include        "sos.inc"

_dio_read:
        jsr     dioprolog
        lda     #D_READ_CALL
        ldx     #D_READ_COUNT
        jmp     diocommon
