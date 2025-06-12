;
; Colin Leroy-Mira <colin@colino.net>, 2024
;
; DIR* __fastcall__ opendir (register const char* name)
;

        .export   _opendir, read_dir_block_ptr1

        .import   closedir_ptr1
        .import   _open, _read, _close
        .import   _malloc
        .import    ___directerrno

        .import   ___oserror, __cwd

        .import   pushptr1, popptr1
        .import   pushax, pusha0
        .import   return0, returnFFFF

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

:       ; Open directory
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
        jsr       _close      ; Close it

        lda       #ENOMEM     ; Set error
        jsr       ___directerrno
@return_null:
        jmp       return0

:       ; Store dir struct to pointer
        sta       ptr1
        stx       ptr1+1

        ; Save fd to dir struct
        lda       #$00
        ldy       #DIR::FD + 1
        sta       (ptr1),y

        dey
        pla                   ; Get fd back
        sta       (ptr1),y

        jsr       read_dir_block_ptr1
        bcc       @read_ok

        ; Close directory, free it
        jsr       closedir_ptr1
        jmp       return0     ; Return NULL

@read_ok:
        ; Read succeeded, populate dir struct

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

; Read a directory for the DIR* pointer in ptr1
; Return with carry clear on success
read_dir_block_ptr1:
        ; Push ptr1, read will destroy it
        jsr       pushptr1

        ldy       #DIR::FD
        lda       (ptr1),y

        jsr       pusha0      ; Push fd for read
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
        bne       @read_err
        cmp       #<.sizeof(DIR::BYTES)
        beq       @read_ok

@read_err:
        ; Read failed, exit
        lda       ___oserror
        bne       :+
        lda       #EINVAL
        jsr       ___directerrno
:       sec
        bcs       @out
@read_ok:
        clc
@out:
        jmp       popptr1
