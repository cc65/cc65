;
; int mkdir (const char* name, ...);    /* An optional mode argument is ignored */
;
; Overrides the common mkdir(), which routes through __sysmkdir/__mappederrno.
; The RP6502 sets errno directly via the RIA, so this calls f_mkdir (RIA_OP_MKDIR)
; instead, mirroring how remove.s/chdir.s bypass __mappederrno.
;

        .export         _mkdir

        .import         _f_mkdir
        .import         addysp, popax

;--------------------------------------------------------------------------

.proc   _mkdir

        ; Discard any variadic arguments (e.g. mode), leaving the name on TOS.
        dey
        dey
        jsr     addysp

        ; Pop the name pointer into A/X and create the directory.
        jsr     popax
        jmp     _f_mkdir        ; Tail call: marshals name, RIA_OP_MKDIR, sets errno

.endproc
