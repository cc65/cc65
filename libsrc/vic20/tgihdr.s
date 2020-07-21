;
; This code sits immediately after the BASIC stub program.
; Therefore, it's executed by that stub.
;
; 2018-04-17, Greg King
;

        .export         __TGIHDR__ : abs = 1    ; Mark as TGI housekeeper

        .import         __MAIN_LAST__
        .importzp       ptr1, ptr2, tmp1


basic_reset     :=      $C000           ; vector to BASIC's cold-start code

; This code moves the program to $2000.  That move allows $1000 - $1FFF
; to be used by the TGI driver to hold its graphics data.

.segment        "TGI1HDR"

        lda     #<(tgi1end + prog_size) ; source
        ldx     #>(tgi1end + prog_size)
        sta     ptr1
        stx     ptr1+1

        lda     #<(tgi2hdr + prog_size) ; destination
        ldx     #>(tgi2hdr + prog_size)
        sta     ptr2
        stx     ptr2+1

        ldx     #<~prog_size
        lda     #>~prog_size            ; use -(prog_size + 1)
        sta     tmp1

        ldy     #<$0000

; Copy loop

@L1:    inx                             ; bump counter's low byte
        beq     @L4

@L2:    tya                             ; will .Y underflow?
        bne     @L3
        dec     ptr1+1                  ; yes, do next lower page
        dec     ptr2+1
@L3:    dey

        lda     (ptr1),y
        sta     (ptr2),y
        jmp     @L1

@L4:    inc     tmp1                    ; bump counter's high byte
        bne     @L2

        jmp     tgi2hdr                 ; go to moved program
tgi1end:

.segment        "TGI2HDR"

tgi2hdr:
        jsr     tgi2end                 ; run actual program
        jmp     (basic_reset)
tgi2end:

; The length of the TGI program (including the TGI2HDR segment)

prog_size       =       __MAIN_LAST__ - tgi2hdr
