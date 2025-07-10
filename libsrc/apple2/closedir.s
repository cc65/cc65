;
; Colin Leroy-Mira <colin@colino.net>, 2024
;
; int __fastcall__ closedir (DIR *dir)
;

        .export   _closedir, closedir_ptr1

        .import   _close
        .import   _free
        .import   pushax, popax, pushptr1, swapstk

        .importzp ptr1

        .include  "apple2.inc"
        .include  "dir.inc"
        .include  "errno.inc"
        .include  "fcntl.inc"
        .include  "zeropage.inc"

_closedir:
        sta       ptr1
        stx       ptr1+1
closedir_ptr1:
        ; Close fd
        jsr       pushptr1    ; Backup ptr1
        ldy       #$00
        lda       (ptr1),y    ; Get fd
        ldx       #$00
        jsr       _close
        jsr       swapstk     ; Store result, pop ptr1

        ; Free dir structure
        jsr       _free
        jmp       popax       ; Return result
