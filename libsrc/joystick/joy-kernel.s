;
; Ullrich von Bassewitz, 2002-12-20
;
; Common functions of the joystick API.
;

        .import         joy_libref
        .importzp       ptr1
        .interruptor    joy_irq         ; Export as IRQ handler

        .include        "joy-kernel.inc"
        .include        "joy-error.inc"


;----------------------------------------------------------------------------
; Variables


.bss
_joy_drv:       .res    2               ; Pointer to driver

_joy_masks:     .res    .sizeof(JOY_HDR::MASKS)

; Jump table for the driver functions.
.data
joy_vectors:
joy_install:    jmp     $0000
joy_uninstall:  jmp     $0000
joy_count:      jmp     $0000
joy_read:       jmp     $0000
joy_irq:        .byte   $60, $00, $00   ; RTS plus two dummy bytes

; Driver header signature
.rodata
joy_sig:        .byte   $6A, $6F, $79, JOY_API_VERSION  ; "joy", version


.code
;----------------------------------------------------------------------------
; unsigned char __fastcall__ joy_install (void* driver);
; /* Install the driver once it is loaded */


_joy_install:
        sta     _joy_drv
        sta     ptr1
        stx     _joy_drv+1
        stx     ptr1+1

; Check the driver signature

        ldy     #.sizeof(joy_sig)-1
@L0:    lda     (ptr1),y
        cmp     joy_sig,y
        bne     inv_drv
        dey
        bpl     @L0

; Set the library reference

        ldy     #JOY_HDR::LIBREF
        lda     #<joy_libref
        sta     (ptr1),y
        iny
        lda     #>joy_libref
        sta     (ptr1),y

; Copy the mask array

        ldy     #JOY_HDR::MASKS + .sizeof(JOY_HDR::MASKS) - 1
        ldx     #.sizeof(JOY_HDR::MASKS)-1
@L1:    lda     (ptr1),y
        sta     _joy_masks,x
        dey
        dex
        bpl     @L1

; Copy the jump vectors

        ldy     #JOY_HDR::JUMPTAB
        ldx     #0
@L2:    inx                             ; Skip the JMP opcode
        jsr     copy                    ; Copy one byte
        jsr     copy                    ; Copy one byte
        cpy     #(JOY_HDR::JUMPTAB + .sizeof(JOY_HDR::JUMPTAB))
        bne     @L2

        jsr     joy_install             ; Call driver install routine
        tay                             ; Test error code
        bne     @L3                     ; Bail out if install had errors

; Install the IRQ vector if the driver needs it. A/X contains the error code
; from joy_install, so don't use it.

        ldy     joy_irq+2               ; Check high byte of IRQ vector
        beq     @L3                     ; Jump if vector invalid
        ldy     #$4C                    ; JMP opcode
        sty     joy_irq                 ; Activate IRQ routine
@L3:    rts

; Driver signature invalid

inv_drv:
        lda     #JOY_ERR_INV_DRIVER
        ldx     #0
        rts

; Copy one byte from the jump vectors

copy:   lda     (ptr1),y
        iny
        sta     joy_vectors,x
        inx
        rts

;----------------------------------------------------------------------------
; unsigned char joy_uninstall (void);
; /* Uninstall the currently loaded driver. Note: This call does not free
; ** allocated memory.
; */

_joy_uninstall:
        lda     #$60                    ; RTS opcode
        sta     joy_irq                 ; Disable IRQ entry point

        jsr     joy_uninstall           ; Call the driver routine

_joy_clear_ptr:                         ; External entry point
        lda     #0
        sta     _joy_drv
        sta     _joy_drv+1              ; Clear the driver pointer

        tax                             ; Return zero
        rts
