;
; Ullrich von Bassewitz, 2012-07-22
;
; void __fastcall__ tgi_load_driver (const char* name);
; /* Load and install the given driver. */


        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"
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

.proc   _tgi_load_driver

; Check if we do already have a driver loaded. This is an error. Do not
; touch A/X because they contain the file name.

        ldy     _tgi_drv
        bne     @L0
        ldy     _tgi_drv+1
        beq     @L1
@L0:    lda     #TGI_ERR_INSTALLED
        bne     @L3

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
        bmi     @L2

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
        beq     @L5
@L2:    lda     #TGI_ERR_CANNOT_LOAD

; Set an error and exit

@L3:    sta     _tgi_error
@L4:    rts

; Check the driver signature, install the driver.
; tgi_install (ctrl.module);

@L5:    lda     ctrl + MOD_CTRL::MODULE
        ldx     ctrl + MOD_CTRL::MODULE+1
        jsr     _tgi_install

; If tgi_install was successful, we're done

        lda     _tgi_error
        beq     @L4

; The driver didn't install correctly. Remove it from memory. The error code
; will be retained.

        lda     _tgi_drv
        ldx     _tgi_drv+1
        jsr     _mod_free               ; Free the driver memory
        jmp     tgi_clear_ptr           ; Clear tgi_drv and return

.endproc



