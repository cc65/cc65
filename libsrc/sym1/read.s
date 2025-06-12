;
; Wayne Parham (wayne@parhamdata.com)
;
; int __fastcall__ read (int fd, void* buf, unsigned count);
;

.include        "sym1.inc"

.import         popax, popptr1
.importzp       ptr1, ptr2, ptr3

.export         _read

.proc           _read

        sta     ptr3
        stx     ptr3+1           ; Count in ptr3
        inx
        stx     ptr2+1           ; Increment and store in ptr2
        tax
        inx
        stx     ptr2
        jsr     popptr1          ; Buffer address in ptr1
        jsr     popax

begin:  dec     ptr2
        bne     getch
        dec     ptr2+1
        beq     done             ; If buffer full, return

getch:  jsr     INTCHR           ; Get character using Monitor ROM call
        jsr     OUTCHR           ; Echo it
        and     #$7F             ; Clear top bit
        cmp     #$07             ; Check for '\a'
        bne     chkcr            ; ...if BEL character
        jsr     BEEP             ; Make beep sound
chkcr:  cmp     #$0D             ; Check for '\r'
        bne     putch            ; ...if CR character
        lda     #$0A             ; Replace with '\n'
        jsr     OUTCHR           ; and echo it

putch:  ldy     #$00             ; Put char into return buffer
        sta     (ptr1),y
        inc     ptr1             ; Increment pointer
        bne     begin
        inc     ptr1+1
        bne     begin

done:   lda     ptr3
        ldx     ptr3+1
        rts                      ; Return count

.endproc
