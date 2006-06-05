;                         
; Ullrich von Bassewitz, 2006-06-05
;
; unsigned char __fastcall__ mouse_load_driver (const struct mouse_callbacks* c,
;                                               const char* name)
; /* Load a mouse driver and return an error code */


        .include        "mouse-kernel.inc"
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

.proc   _mouse_load_driver

; Save name on the C stack. We will need it later as parameter passed to open()

        jsr     pushax

; Check if we do already have a driver loaded. If so, remove it.

        lda     _mouse_drv
        ora     _mouse_drv+1
        beq     @L1
        jsr     _mouse_uninstall

; Open the file. The name parameter is already on stack and will get removed
; by open().
; ctrl.callerdata = open (name, O_RDONLY);

@L1:    lda     #<O_RDONLY
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

; Check the driver signature, install the driver. c is already on stack and
; will get removed by mouse_install().
; Res = mouse_install (c, ctrl.module);

        lda     ctrl + MOD_CTRL::MODULE
        ldx     ctrl + MOD_CTRL::MODULE+1
        jsr     _mouse_install

; If mouse_install was successful, we're done

        tax
        beq     @L2

; The driver didn't install correctly. Remove it from memory and return the
; error code.

        pha                             ; Save the error code
        lda     _mouse_drv
        ldx     _mouse_drv+1
        jsr     _mod_free               ; Free the driver memory
        jsr     _mouse_clear_ptr        ; Clear mouse_drv
        pla                             ; Restore the error code
        ldx     #0                      ; We must return an int
@L2:    rts                             ; Done

; Open or mod_load failed. Remove excess arguments from stack and return an
; error code.

@L3:    jsr     incsp2
        lda     #<MOUSE_ERR_CANNOT_LOAD
        ldx     #>MOUSE_ERR_CANNOT_LOAD
        rts

.endproc


