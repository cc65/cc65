;
; Ullrich von Bassewitz, 2002-12-20
;
; Common functions of the joystick API.
;

        .export         _joy_install, _joy_deinstall, _joy_masks

        .importzp       ptr1

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"


;----------------------------------------------------------------------------
; Variables


.bss
_joy_drv:       .res    2      		; Pointer to driver

_joy_masks:     .res    JOY_MASK_COUNT

; Jump table for the driver functions.
.data
joy_vectors:
joy_install:   	jmp     $0000
joy_deinstall: 	jmp     $0000
joy_count:      jmp     $0000
joy_read:       jmp     $0000

; Driver header signature
.rodata
joy_sig:        .byte   $6A, $6F, $79, $00      ; "joy", version
joy_sig_len     = * - joy_sig


;----------------------------------------------------------------------------
; unsigned char __fastcall__ joy_install (void* driver);
; /* Install the driver once it is loaded */


_joy_install:
       	sta     _joy_drv
  	sta	ptr1
  	stx     _joy_drv+1
  	stx    	ptr1+1

; Check the driver signature

        ldy     #joy_sig_len-1
@L0:    lda     (ptr1),y
        cmp     joy_sig,y
        bne     inv_drv
        dey
        bpl     @L0

; Copy the mask array

        ldy     #JOY_MASKS + JOY_MASK_COUNT - 1
        ldx     #JOY_MASK_COUNT-1
@L1:    lda     (ptr1),y
        sta     _joy_masks,x
        dey
        dex
        bpl     @L1

; Copy the jump vectors

        ldy     #JOY_HDR_JUMPTAB
        ldx     #0
@L2:    inx                             ; Skip the JMP opcode
        jsr     copy                    ; Copy one byte
        jsr     copy                    ; Copy one byte
        cpx     #(JOY_HDR_JUMPCOUNT*3)
        bne     @L2

        jmp     joy_install             ; Call driver install routine

; Driver signature invalid

inv_drv:
        lda     #JOY_ERR_INV_DRIVER
        ldx     #0
        rts

; Copy one byte from the jump vectors

copy:   lda     (ptr1),y
        iny
set:    sta     joy_vectors,x
        inx
        rts

;----------------------------------------------------------------------------
; void __fastcall__ joy_deinstall (void);
; /* Deinstall the driver before unloading it */

_joy_deinstall  = joy_deinstall           ; Call driver routine

