;
; Colin Leroy-Mira <colin@colino.net>, 2024
;
; unsigned int __fastcall__ dir_entry_count(DIR *dir);
;

        .export   _dir_entry_count

        .importzp ptr1

        .include  "apple2.inc"
        .include  "dir.inc"

.proc _dir_entry_count
        sta       ptr1
        stx       ptr1+1

        ldy       #DIR::FILE_COUNT + 1
        lda       (ptr1),y
        tax
        dey
        lda       (ptr1),y
        rts
.endproc
