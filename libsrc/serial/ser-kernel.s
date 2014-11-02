;
; Ullrich von Bassewitz, 2003-04-15
;
; Common functions of the serial drivers
;

        .import         return0, ser_libref
        .importzp       ptr1
        .interruptor    ser_irq, 29     ; Export as high priority IRQ handler

        .include        "ser-kernel.inc"
        .include        "ser-error.inc"


;----------------------------------------------------------------------------
; Variables


.bss
_ser_drv:       .res    2               ; Pointer to driver

; Jump table for the driver functions.
.data
ser_vectors:
ser_install:    jmp     return0
ser_uninstall:  jmp     return0
ser_open:       jmp     return0
ser_close:      jmp     return0
ser_get:        jmp     return0
ser_put:        jmp     return0
ser_status:     jmp     return0
ser_ioctl:      jmp     return0
ser_irq:        .byte   $60, $00, $00   ; RTS plus two dummy bytes

; Driver header signature
.rodata
ser_sig:        .byte   $73, $65, $72, SER_API_VERSION  ; "ser", version


.code
;----------------------------------------------------------------------------
; unsigned char __fastcall__ ser_install (void* driver);
; /* Install the driver once it is loaded */


_ser_install:
        sta     _ser_drv
        sta     ptr1
        stx     _ser_drv+1
        stx     ptr1+1

; Check the driver signature

        ldy     #.sizeof(ser_sig)-1
@L0:    lda     (ptr1),y
        cmp     ser_sig,y
        bne     inv_drv
        dey
        bpl     @L0

; Set the library reference

        ldy     #SER_HDR::LIBREF
        lda     #<ser_libref
        sta     (ptr1),y
        iny
        lda     #>ser_libref
        sta     (ptr1),y

; Copy the jump vectors

        ldy     #SER_HDR::JUMPTAB
        ldx     #0
@L1:    inx                             ; Skip the JMP opcode
        jsr     copy                    ; Copy one byte
        jsr     copy                    ; Copy one byte
        cpy     #(SER_HDR::JUMPTAB + .sizeof(SER_HDR::JUMPTAB))
        bne     @L1

        jsr     ser_install             ; Call driver install routine

        ldy     ser_irq+2               ; Check high byte of IRQ vector
        beq     @L2                     ; Jump if vector invalid
        ldy     #$4C                    ; Jump opcode
        sty     ser_irq                 ; Activate IRQ routine
@L2:    rts

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
; unsigned char ser_uninstall (void);
; /* Uninstall the currently loaded driver and return an error code.
; ** Note: This call does not free allocated memory.
; */

_ser_uninstall:
        jsr     ser_uninstall           ; Call driver routine

        lda     #$60                    ; RTS opcode
        sta     ser_irq                 ; Disable IRQ entry point

_ser_clear_ptr:                         ; External entry point
        lda     #0
        sta     _ser_drv
        sta     _ser_drv+1              ; Clear the driver pointer

        tax
        rts                             ; Return zero
