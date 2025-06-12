;
; Colin Leroy-Mira <colin@colino.net>, 2024
;
; void __fastcall__ rewinddir (DIR* dir)
;
        .export                  _rewinddir
        .import                  read_dir_block_ptr1

        .import                  pusha, pusha0, pushax
        .import                  pushptr1, popptr1
        .import                  incaxy
        .import                  _lseek, _memset

        .importzp                ptr1, sreg

        .include                 "dir.inc"
        .include                 "stdio.inc"

.proc _rewinddir
        sta     ptr1
        stx     ptr1+1
        jsr     pushptr1      ; Backup ptr1, destroyed by _lseek

        ; Rewind directory file
        ldy     #DIR::FD
        lda     (ptr1),y
        jsr     pusha0        ; Push dir->fd

        tya                   ; Y = 0 here
        jsr     pusha0
        jsr     pusha0        ; Push 0L

        lda     #SEEK_SET     ; X = 0 here
        jsr     _lseek

        ora     sreg          ; Check lseek returned 0L
        ora     sreg+1
        bne     @rewind_err
        txa
        bne     @rewind_err

        jsr     popptr1       ; Restore ptr1

        ; Read directory key block
        jsr     read_dir_block_ptr1
        bcs     @rewind_err

        ; Skip directory header entry
        lda     #$01
        ldy     #DIR::CURRENT_ENTRY
        sta     (ptr1),y
        rts

@rewind_err:
        jsr     popptr1       ; Restore ptr1

        ; Assert that no subsequent readdir() finds an active entry
        lda     ptr1
        ldx     ptr1+1
        ldy     #DIR::BYTES + DIR::CONTENT::ENTRIES
        jsr     incaxy
        jsr     pushax

        lda     #$00
        jsr     pusha

        lda     #<.sizeof(DIR::BYTES)
        ldx     #>.sizeof(DIR::BYTES)
        jmp     _memset
.endproc
