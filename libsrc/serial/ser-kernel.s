;
; Ullrich von Bassewitz, 2003-04-15
;
; Common functions of the serial drivers
;

        .export         ser_clear_ptr
        .import         return0
        .importzp       ptr1

        .include        "ser-kernel.inc"
        .include        "ser-error.inc"


;----------------------------------------------------------------------------
; Variables


.bss
_ser_drv:       .res    2      		; Pointer to driver

; Jump table for the driver functions.
.data
ser_vectors:
ser_install:   	jmp     return0
ser_uninstall: 	jmp     return0

; Driver header signature
.rodata
ser_sig:        .byte   $73, $65, $72, $00      ; "ser", version
ser_sig_len     = * - ser_sig


;----------------------------------------------------------------------------
; unsigned char __fastcall__ ser_install (void* driver);
; /* Install the driver once it is loaded */


_ser_install:
       	sta     _ser_drv
  	sta	ptr1
  	stx     _ser_drv+1
  	stx    	ptr1+1

; Check the driver signature

        ldy     #ser_sig_len-1
@L0:    lda     (ptr1),y
        cmp     ser_sig,y
        bne     inv_drv
        dey
        bpl     @L0

; Copy the jump vectors

        ldy     #SER_HDR_JUMPTAB
        ldx     #0
@L1:    inx                             ; Skip the JMP opcode
        jsr     copy                    ; Copy one byte
        jsr     copy                    ; Copy one byte
        cpx     #(SER_HDR_JUMPCOUNT*3)
        bne     @L1

        jmp     ser_install             ; Call driver install routine

; Driver signature invalid

inv_drv:
        lda     #SER_ERR_INV_DRIVER
        ldx     #0
        rts

; Copy one byte from the jump vectors

copy:   lda     (ptr1),y
        sta     ser_vectors,x
        iny
        inx
        rts

;----------------------------------------------------------------------------
; unsigned char __fastcall__ ser_uninstall (void);
; /* Uninstall the currently loaded driver and return an error code.
;  * Note: This call does not free allocated memory.
;  */

_ser_uninstall:
        jsr     ser_uninstall           ; Call driver routine

ser_clear_ptr:                          ; External entry point
        lda     #0
        sta     _ser_drv
        sta     _ser_drv+1              ; Clear the driver pointer

        tax
        rts                             ; Return zero

