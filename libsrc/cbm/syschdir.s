;
; 2012-10-16, Oliver Schmidt
; 2014-07-16, Greg King
;
; unsigned char __fastcall__ _syschdir (const char* name);
;

        .export         __syschdir
        .import         diskinit, fnunit, curunit, initcwd
        .importzp       ptr1, tmp1, tmp2

;--------------------------------------------------------------------------
; __syschdir

.proc   __syschdir

; Save name

        sta     ptr1
        stx     ptr1+1

; Process first character

        ldy     #0
        lda     (ptr1),y
        beq     err
        jsr     getdigit
        bcs     err
        tax

; Process second character

        iny
        lda     (ptr1),y
        beq     init
        jsr     getdigit
        bcs     err
        stx     tmp1            ; First digit
        sta     tmp2            ; Second digit

; Multiply first digit by 10

        txa
        asl     a               ; * 2
        asl     a               ; * 4, carry cleared
        adc     tmp1            ; * 5
        asl     a               ; * 10, carry cleared

; Add second digit to product

        adc     tmp2
        tax

; Process third character

        iny
        lda     (ptr1),y
        bne     err

; Check device readiness

init:   txa
        jsr     diskinit
        bne     done

; Success, update cwd

        lda     fnunit          ; Set by diskinit
        sta     curunit
        jmp     initcwd         ; Returns with A = 0

; Return with error in A

err:    lda     #9              ; "Illegal device"
done:   rts

.endproc

;--------------------------------------------------------------------------
; getdigit -- Converts PetSCII to binary.
; Sets carry if the character is outside of '0'-'9'.

.proc   getdigit

        sec
        sbc     #'0'
        bcs     @L0
        sec
        rts
@L0:    cmp     #10
        rts

.endproc
