;
; Ullrich von Bassewitz, 02.04.1999
;
; unsigned char getcpu (void);
;
        .include        "zeropage.inc"
        .export         _getcpu

; ---------------------------------------------------------------------------
; Subroutine to detect an 816. Returns
;
;   - carry clear and 0 in A for a NMOS 6502 CPU
;   - carry set and 1 in A for a 65C02
;   - carry set and 2 in A for a 65816
;   - carry set and 3 in A for a 4510
;   - carry set and 4 in A for a 65SC02
;   - carry set and 5 in A for a 65CE02
;   - carry set and 6 in A for a HuC6280
;   - carry clear and 7 in A for a 2a03/2a07
;   - carry set and 8 in A for a 45GS02
;
; This function uses a $1A opcode which is a INA on the 816 and ignored
; (interpreted as a NOP) on a NMOS 6502. There are several CMOS versions
; of the 6502, but all of them interpret unknown opcodes as NOP so this is
; just what we want.

.p816                           ; Enable 65816 instructions

_getcpu:

        lda     #0
        inc     a               ; .byte $1A ; nop on nmos, inc on every cmos
        cmp     #1
        bcc     @IsNMOS

; This is at least a 65C02, check for a 65CE02/4510

        .byte   $42,$EA         ; neg on 65CE02/4510, nop #$EA on 65C02, wdm $EA on 65816
        cmp     #1
        beq     @HasINCA

; This is at least a 65CE02, check for 4510

        lda     #5              ; CPU_65CE02 constant
        ldx     #0              ; to make sure MAP doesn't do anything, the upper nybl of X and Z must be clear
        .byte   $5C             ; map on 4510, aug on 65CE02 (acts like 4 byte nop)
        lda     #3              ; CPU_4510 constant
        nop
        cmp     #5
        beq     @LoadXAndReturn

; It is either a 4510 (C65) or a 45GS02 (MEGA65)

        ; 45GS02 supports 32-bit ZP indirect, so use that to check CPU type
        ; without requiring a functioning MEGA65 hypervisor.
        ; We setup a read of $200xx, then store a different value in $xx
        ; and then re-read $200xx to see if it is unchanged.

        ; Setup 32-bit pointer to $00020000+tmp1
        lda     #<$020000+tmp1
        sta     regsave
        lda     #>$020000+tmp1
        sta     regsave+1
        sta     regsave+3       ; also write to upper byte of pointer to save an extra LDA #$00
        lda     #^$020000+tmp1
        sta     regsave+2

        ; Prefixing LDA ($nn),Z with a NOP uses 32-bit ZP pointer on 45GS02,
        ; but normal 16-bit ZP pointer on 4510
        ; (We assume Z=$00, which will be the normal case)
        nop                     ; prefix to tell next instruction to be 32-bit ZP
        .byte   $b2,regsave     ; LDA (regsave),Z
        eor     #$ff            ; change the value
        sta     tmp1            ; store in $xx
        ; now try again to load it: If the same, then 45GS02, as $200xx is unchanged
        nop                     ; prefix to tell next instruction to be 32-bit ZP
        .byte   $b2,regsave     ; LDA (regsave),Z
        cmp     tmp1            ; does the loaded value match what is in $xx?
        bne     @Is45GS02       ; $200xx and $xx have different values, so must be a MEGA65 45GS02
@Is4510:
        lda     #3              ; CPU_4510 constant
        ldx     #0              ; load high byte of word
        rts

@Is45GS02:
        lda     #8              ; CPU_45GS02 constant
        ldx     #0              ; load high byte of word
        rts

; 6502 type of cpu, check for a 2a03/2a07
@IsNMOS:
        sed                     ; set decimal mode, no decimal mode on the 2a03/2a07
        lda     #9
        clc
        adc     #1              ; $01+$09 = $10 on 6502, $01+$09 = $0A on 2a03/2a07
        cld
        cmp     #$0a
        beq     @Is2a03
        lda     #0              ; CPU_6502 constant
        beq     @LoadXAndReturn
@Is2a03:
        lda     #7              ; CPU_2A0x constant
        bne     @LoadXAndReturn

; 65C02 cpu type, check for HuC6280
@CheckHuC6280:
        ldx     #6              ; CPU_HUC6280 constant
        .byte   $22,$EA         ; sax nop on HuC6280 (A=$06, X=$01), nop #$EA on 65C02 (A=$01, X=$06)
        bne     @LoadXAndReturn

; Check for 65816/65802
@HasINCA:
        xba                     ; .byte $EB, put $01 in B accu (nop on 65C02/65SC02)
        dec     a               ; .byte $3A, A=$00
        xba                     ; .byte $EB, A=$01 if 65816/65802 and A=$00 if 65C02/65SC02
        inc     a               ; .byte $1A, A=$02 if 65816/65802 and A=$01 if 65C02/65SC02
        cmp     #2
        beq     @LoadXAndReturn

; check for 65SC02

        ldy     $F7
        ldx     #0
        stx     $F7
        .byte   $F7,$F7         ; nop nop on 65SC02, smb7 $F7 on 65C02
        ldx     $F7
        sty     $F7
        cpx     #$00
        bne     @CheckHuC6280
        lda     #4              ; CPU_65SC02 constant
@LoadXAndReturn:
        ldx     #0
        rts
