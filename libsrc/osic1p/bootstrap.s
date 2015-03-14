;
; 2015-03-08, Greg King
;

; When you want to create a program with the alternate file format,
; add "-u __BOOT__" to the cl65/ld65 command line.  Then, the linker
; will import this symbol name; and, link this module at the front
; of your program file.
;
        .export         __BOOT__:abs = 1

        .import         __RAM_START__, __RAM_SIZE__, __BSS_RUN__

; ------------------------------------------------------------------------

load_addr       :=      __RAM_START__
load_size       =       __BSS_RUN__ - __RAM_START__
ram_top         :=      __RAM_START__ + __RAM_SIZE__

        .segment        "BOOT"

; If you want to change how this bootstrap loader works, then:
; 1. edit this assembly source code,
; 2. define the constant ASM (uncomment the line below),
; 3. assemble this file (and, make a listing of that assembly),
; 4. copy the listing's hex codes into the .byte lines below (notice that most
;    of the strings are followed by CR; it's required by the OS65V monitor)
;    (be sure to match the listing's lines against the .byte lines),
; 5. undefine ASM (recomment the line),
; 6. assemble this file, again,
; 7. and, add the object file to "osic1p.lib".

;ASM = 1

.ifdef ASM

        .include        "osic1p.inc"
        .macpack        generic

load            :=      $08             ; private variables
count           :=      $0A

GETCHAR         :=      $FFBF           ; gets one character from ACIA

FIRSTVISC       =       $85             ; Offset of first visible character in video RAM
LINEDIST        =       $20             ; Offset in video RAM between two lines

        ldy     #<$0000
        lda     #<load_addr
        ldx     #>load_addr
        sta     load
        stx     load+1
        lda     #<load_size
        eor     #$FF
        sta     count                   ; store (-size - 1)
        lda     #>load_size
        eor     #$FF
        sta     count+1

L1:     inc     count                   ; pre-count one's-complement upwards
        bnz     L2
        inc     count+1
        bze     L3
L2:     jsr     GETCHAR                 ; (doesn't change .Y)
        sta     (load),y

; Show that the file is being loaded by rotating an arrow on the screen.
;
        tya
        lsr     a
        lsr     a
        and     #8 - 1
        ora     #$10                    ; eight arrow characters
        sta     SCRNBASE + FIRSTVISC + 2 * LINEDIST + 11

        iny
        bnz     L1
        inc     load+1
        bnz     L1                      ; branch always

L3:     jmp     load_addr

.else

.mac    hex1    h
        .lobytes        ((h) & $0F) + (((h) & $0F) > 9) * 7 + '0'
.endmac

.mac    hex2    h
        hex1    (h) >> 4
        hex1    (h) >> 0
.endmac

.mac    hex4    h
        hex2    >(h)
        hex2    <(h)
.endmac

CR      =       $0D

        .byte   CR, CR
        .byte   "."                     ; set an address
        hex4    ram_top                 ; put loader where stack will sit
        .byte   "/"                     ; write bytes into RAM

; ASCII-coded hexadecimal translation of the above assembly code.
; It was copied from the assembler listing.

        .byte   "A0", CR, "00", CR
        .byte   "A9", CR
        hex2    <load_addr
        .byte   CR, "A2", CR
        hex2    >load_addr
        .byte   CR, "85", CR, "08", CR
        .byte   "86", CR, "09", CR
        .byte   "A9", CR
        hex2    <load_size
        .byte   CR, "49", CR, "FF", CR
        .byte   "85", CR, "0A", CR
        .byte   "A9", CR
        hex2    >load_size
        .byte   CR, "49", CR, "FF", CR
        .byte   "85", CR, "0B", CR

        .byte   "E6", CR, "0A", CR
        .byte   "D0", CR, "04", CR
        .byte   "E6", CR, "0B", CR
        .byte   "F0", CR, "16", CR
        .byte   "20", CR, "BF", CR, "FF", CR
        .byte   "91", CR, "08", CR

        .byte   "98", CR
        .byte   "4A", CR
        .byte   "4A", CR
        .byte   "29", CR, "07", CR
        .byte   "09", CR, "10", CR
        .byte   "8D", CR, "D0", CR, "D0", CR

        .byte   "C8", CR
        .byte   "D0", CR, "E6", CR
        .byte   "E6", CR, "09", CR
        .byte   "D0", CR, "E2", CR

        .byte   "4C", CR
        hex2    <load_addr
        .byte   CR
        hex2    >load_addr

        .byte   CR, "."
        hex4    ram_top
        .byte   "G"                     ; go to address

.endif
