;
; Colin Leroy-Mira <colin@colino.net>, 2024
;
; DIR* __fastcall__ opendir (register const char* name)
;

        .export   _opendir

        .import   _open, _read, _close
        .import   _malloc, _free
        .import    ___directerrno

        .import   ___oserror, __cwd

        .import   pushptr1, popptr1
        .import   pushax, pusha0

        .importzp ptr1

        .include  "apple2.inc"
        .include  "dir.inc"
        .include  "errno.inc"
        .include  "fcntl.inc"
        .include  "zeropage.inc"

.proc _opendir
        sta       ptr1
        stx       ptr1+1

        ldy       #$00
        lda       (ptr1),y
        cmp       #'.'
        bne       :+

        lda       #<__cwd
        ldx       #>__cwd
        sta       ptr1
        stx       ptr1+1

:       ; open directory
        jsr       pushptr1
        lda       #O_RDONLY
        jsr       pusha0

        ldy       #$04
        jsr       _open

        cmp       #$FF        ; Did we succeed?
        beq       @return_null
        pha                   ; Yes - Push fd for backup

        ; malloc the dir struct
        lda       #<.sizeof(DIR)
        ldx       #>.sizeof(DIR)
        jsr       _malloc
        bne       :+

        ; We failed to allocate
        pla                   ; Get fd back
        ldx       #$00
        jsr       _close      ; close it

        lda       #ENOMEM     ; Set error
        jsr       ___directerrno

@return_null:
        lda       #$00
        tax
        rts

:       ; Store dir struct to pointer
        sta       ptr1
        stx       ptr1+1

        ; Push ptr1, read will destroy it
        jsr       pushptr1

        ; Save fd to dir struct
        lda       #$00
        ldy       #DIR::FD + 1
        sta       (ptr1),y

        dey
        pla                   ; Get fd back
        sta       (ptr1),y

        jsr       pusha0      ; push fd for read
        lda       #<DIR::BYTES
        clc
        adc       ptr1
        pha
        lda       #>DIR::BYTES
        adc       ptr1+1
        tax
        pla
        jsr       pushax      ; Push dir->block.bytes for read

        lda       #<.sizeof(DIR::BYTES)
        ldx       #>.sizeof(DIR::BYTES)

        jsr       _read       ; Read directory block
        cpx       #>.sizeof(DIR::BYTES)
        bne       @err_read
        cmp       #<.sizeof(DIR::BYTES)
        beq       @read_ok

@err_read:
        ; Read failed, exit
        lda       ___oserror
        bne       :+
        lda       #EINVAL
        jsr       ___directerrno

:       ; Close fd
        jsr       popptr1     ; Restore our dir pointer
        ldy       #$00
        lda       (ptr1),y    ; Get fd
        ldx       #$00
        jsr       _close

        ; Free dir structure
        lda       ptr1
        ldx       ptr1+1
        jsr       _free
        jmp       @return_null

@read_ok:
        ; Read succeeded, populate dir struct
        jsr       popptr1     ; Restore our dir pointer

        ; Get file_count to entry_length from block
        ldy       #$26 + DIR::BYTES
:       lda       (ptr1),y
        pha
        dey
        cpy       #$23 + DIR::BYTES - 1
        bne       :-

        ; Set entry_length to file_count in struct
        ldy       #DIR::ENTRY_LENGTH
:       pla
        sta       (ptr1),y
        iny
        cpy       #DIR::CURRENT_ENTRY
        bne       :-

        ; Skip directory header entry
        lda       #$01
        sta       (ptr1),y

        ; Return pointer to dir struct
        lda       ptr1
        ldx       ptr1+1
        rts
.endproc
