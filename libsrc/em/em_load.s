;
; Ullrich von Bassewitz, 2012-07-22
;
; unsigned char __fastcall__ em_load_driver (const char* name)
; /* Load an extended memory driver and return an error code */


        .include        "em-kernel.inc"
        .include        "em-error.inc"
        .include        "modload.inc"
        .include        "fcntl.inc"

        .import         pushax
        .import         pusha0
        .import         incsp2
        .import         _open
        .import         _read
        .import         _close



;----------------------------------------------------------------------------
; Variables

.data

ctrl:   .addr   _read
        .res    2                       ; CALLERDATA
        .res    2                       ; MODULE
        .res    2                       ; MODULE_SIZE
        .res    2                       ; MODULE_ID
                   
;----------------------------------------------------------------------------
; Code

.code

.proc   _em_load_driver

; Check if we do already have a driver loaded. This is an error. Do not
; touch A/X because they contain the file name.

        ldy     _em_drv
        bne     @L0
        ldy     _em_drv+1
        beq     @L1
@L0:    lda     #EM_ERR_INSTALLED
        bne     @L4

; Push the name onto the C stack and open the file. The parameter will get
; removed by open().
; ctrl.callerdata = open (name, O_RDONLY);

@L1:    jsr     pushax
        lda     #<O_RDONLY
        jsr     pusha0
        ldy     #4                      ; Argument size
        jsr     _open
        sta     ctrl + MOD_CTRL::CALLERDATA
        stx     ctrl + MOD_CTRL::CALLERDATA+1

; if (ctrl.callerdata >= 0) {

        txa
        bmi     @L3

; /* Load the module */
; Res = mod_load (&ctrl);

        lda     #<ctrl
        ldx     #>ctrl
        jsr     _mod_load
        pha

; /* Close the input file */
; close (ctrl.callerdata);

        lda     ctrl + MOD_CTRL::CALLERDATA
        ldx     ctrl + MOD_CTRL::CALLERDATA+1
        jsr     _close

; /* Check the return code */
; if (Res == MLOAD_OK) {

        pla
        bne     @L3

; Check the driver signature, install the driver.
; Res = em_install (ctrl.module);

        lda     ctrl + MOD_CTRL::MODULE
        ldx     ctrl + MOD_CTRL::MODULE+1
        jsr     _em_install

; If em_install was successful, we're done

        tax
        beq     @L2

; The driver didn't install correctly. Remove it from memory and return the
; error code.

        pha                             ; Save the error code
        lda     _em_drv
        ldx     _em_drv+1
        jsr     _mod_free               ; Free the driver memory
        jsr     em_clear_ptr            ; Clear em_drv
        pla                             ; Restore the error code
        ldx     #0                      ; We must return an int
@L2:    rts                             ; Done

; Open or mod_load failed. Return an error code.

@L3:    lda     #<EM_ERR_CANNOT_LOAD
@L4:    ldx     #0
        rts

.endproc


