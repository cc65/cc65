;
; int __fastcall__ write (int fd, const void* buf, int count);
;

.include        "kim1.inc"

.import         popax, popptr1
.importzp       ptr1, ptr2, ptr3

.export         _write

.proc           _write

        sta     ptr3
        stx     ptr3+1          ; Count in ptr3
        inx
        stx     ptr2+1          ; Increment and store in ptr2
        tax
        inx
        stx     ptr2
        jsr     popptr1         ; Buffer address in ptr1
        jsr     popax

begin:  dec     ptr2
        bne     outch
        dec     ptr2+1
        beq     done

outch:  ldy     #0
        lda     (ptr1),y
        pha                     ; Save A (changed by OUTCHR)
        jsr     OUTCHR          ; Send character using Monitor call
        pla                     ; Restore A
        cmp     #$0A            ; Check for '\n'
        bne     next            ; ...if LF character
        lda     #$0D            ; Add a carriage return
        jsr     OUTCHR

next:   inc     ptr1
        bne     begin
        inc     ptr1+1
        jmp     begin

done:   lda     ptr3
        ldx     ptr3+1
        rts                     ; Return count

.endproc
