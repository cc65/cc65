;
; Christian Groessler, February 2005
;
; unsigned __fastcall__ dio_query_sectsize(dhandle_t handle);
;

        .include        "atari.inc"
        .export         _dio_query_sectsize
        .importzp       ptr1,tmp1
        .import         popax, __oserror

.proc   _dio_query_sectsize

        sta     ptr1            ; handle
        stx     ptr1+1 

        lda     #0
        sta     __oserror

        ldy     #sst_sectsize+1
        lda     (ptr1),y
        tax
        dey
        lda     (ptr1),y
        rts

.endproc
