;
; Oliver Schmidt, 08.03.2004
;
; Identify DOS version we're running on as one of these:
;
; AppleDOS 3.3   - $00
; ProDOS 8 1.0.1 - $10
; ProDOS 8 1.0.2 - $10
; ProDOS 8 1.1.1 - $11
; ProDOS 8 1.2   - $12
; ProDOS 8 1.3   - $13
; ProDOS 8 1.4   - $14
; ProDOS 8 1.5   - $15
; ProDOS 8 1.6   - $16
; ProDOS 8 1.7   - $17
; ProDOS 8 1.8   - $18
; ProDOS 8 1.9   - $18
; ProDOS 8 2.0.1 - $21
; ProDOS 8 2.0.2 - $22
; ProDOS 8 2.0.3 - $23
;

        .constructor    initdostype, 25
        .export         __dos_type

        .include        "mli.inc"

; Identify DOS version according to:
; - Beneath Apple ProDOS, chapter 6-63
; - Apple II ProDOS 8 TechNote #23, ProDOS 8 Changes and Minutia
; - ProDOS TechRefMan, chapter 5.2.4

        .segment        "ONCE"

initdostype:
        lda     $BF00
        cmp     #$4C            ; Is MLI present? (JMP opcode)
        bne     done
        lda     KVERSION        ; ProDOS 8 kernel version
        cmp     #$10
        bcs     :+
        ora     #$10            ; Make high nibble match major version
:       sta     __dos_type
done:   rts

        .bss

__dos_type:     .res    1
