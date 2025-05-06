;
; Colin Leroy-Mira <colin@colino.net>, 2024
;
; struct dirent * __fastcall__ readdir (DIR *dir)
;
        .export                  _readdir
        .import                  read_dir_block_ptr1

        .import                  incax1, return0
        .import                  tosaddax, tosumula0, incaxy
        .import                  pushax, pusha0, pushptr1, popptr1
        .importzp                ptr1, ptr4

        .include                 "dir.inc"

.proc _readdir
        sta     ptr1
        stx     ptr1+1

@next_entry:
        ; Do we need to read the next directory block?
        ldy     #DIR::CURRENT_ENTRY
        lda     (ptr1),y
        ldy     #DIR::ENTRIES_PER_BLOCK
        cmp     (ptr1),y
        bne     @read_entry            ; We don't

        jsr     read_dir_block_ptr1
        bcc     @read_ok

        ; We had a read error
        jmp     return0

@read_ok:
        ldy     #DIR::CURRENT_ENTRY
        lda     #$00
        sta     (ptr1),y

@read_entry:
        ; Compute pointer to current entry:
        ; entry = dir->block.content.entries +
        ;    dir->current_entry * dir->entry_length

        jsr     pushptr1                ; Backup ptr1
        lda     ptr1
        ldx     ptr1+1
        ldy     #DIR::BYTES + DIR::CONTENT::ENTRIES
        jsr     incaxy
        jsr     pushax
        ldy     #DIR::CURRENT_ENTRY
        lda     (ptr1),y
        jsr     pusha0
        ldy     #DIR::ENTRY_LENGTH
        lda     (ptr1),y
        jsr     tosumula0
        jsr     tosaddax
        ; Store pointer to current entry
        sta     ptr4
        stx     ptr4+1
        jsr     popptr1

        ; Switch to next entry
        ldy     #DIR::CURRENT_ENTRY
        lda     (ptr1),y
        clc
        adc     #1
        sta     (ptr1),y

        ; Check if entry[0] == 0
        ldy     #$00
        lda     (ptr4),y
        beq     @next_entry             ; Yes, skip entry

        ; Move creation date/time to allow for next step below
        ; 18-19-1A-1B => 1A-1B-1C-1D
        ldy     #$1B
:       lda     (ptr4),y
        iny
        iny
        sta     (ptr4),y
        dey
        dey
        dey
        cpy     #$17
        bne     :-

        ; Feature unsigned long access to EOF by extension from 3 to 4 bytes
        ; entry[0x18] = 0
        iny
        lda     #$00
        sta     (ptr4),y

        ; Move file type to allow for next step below
        ; entry[0x19] = entry[0x10]
        ldy     #$10
        lda     (ptr4),y
        ldy     #$19
        sta     (ptr4),y

        ; Zero-terminate file name
        ldy     #$00
        lda     (ptr4),y
        and     #$0F
        tay
        iny
        lda     #$00
        sta     (ptr4),y

        ; Return pointer to entry+1
        lda     ptr4
        ldx     ptr4+1
        jmp     incax1
.endproc
