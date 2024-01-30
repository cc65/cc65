;
; Colin Leroy-Mira, 2023 <colin@colino.net>
;
; int __fastcall__ stat(const char *pathname, struct stat *statbuf);
;

        .export         _stat
        .import         __errno, _open,_close
        .import         mli_file_info
        .import         popax, pushax, pusha0, incsp2
        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "filedes.inc"
        .include        "mli.inc"
        .include        "stat.inc"

_stat:
        ; Store statbuf pointer
        sta     ptr4
        sta     stbuf
        stx     ptr4+1
        stx     stbuf+1

        ; Clear statbuf
        lda     #$00
        ldy     #.sizeof(stat)-1
:       sta     (ptr4),y
        dey
        bpl     :-

        ; Reset errno
        sta     ___errno

        ; Store pathname
        jsr     popax
        jsr     pushax          ; Push it back for mli_file_info
        jsr     pushax          ; and for open

        jsr     mli_file_info

        bcc     got_info
        jmp     incsp2          ; Drop filename copy for open

got_info:
        ; st_dev
        lda     DEVNUM
        lsr                     ; Shift right to cc65 representation
        lsr
        lsr
        lsr
        ldy     #stat::st_dev
        sta     (ptr4),y

        ; st_mode (S_IFDIR/S_IFREG only)
        lda     mliparam + MLI::INFO::FILE_TYPE
        ldy     #stat::st_mode
        cmp     #$0f
        bne     is_reg
        lda     #S_IFDIR
        bne     set_st_mode

is_reg: lda     #S_IFREG

set_st_mode:
        sta     (ptr4),y

        ; st_access through st_create_time
        ldx     #MLI::INFO::ACCESS
        ldy     #stat::st_access
:       lda     mliparam,x
        sta     (ptr4),y
        inx
        iny
        cpy     #stat::st_create_time + .sizeof(stat::st_create_time)
        bne     :-

        ; st_size
        lda     #O_RDONLY
        jsr     pusha0
        ldy     #$04
        jsr     _open
        cmp     #$FF
        beq     done
        pha                     ; Save file descriptor for closing

        ; Get ProDOS's REF_NUM from file descriptor
        jsr     getfd
        ; Get file information
        sta     mliparam + MLI::EOF::REF_NUM
        lda     #GET_EOF_CALL
        ldx     #EOF_COUNT
        jsr     callmli
        bcs     eoferr

        ; Get struct stat in ptr4 back, open destroyed it
        lda     stbuf
        ldx     stbuf+1
        sta     ptr4
        stx     ptr4+1

        ; Store size
        ldy     #stat::st_size
        lda     mliparam + MLI::EOF::EOF
        sta     (ptr4),y
        lda     mliparam + MLI::EOF::EOF+1
        iny
        sta     (ptr4),y
        lda     mliparam + MLI::EOF::EOF+2
        iny
        sta     (ptr4),y

        ; Close file
eoferr:
        pla
        ldx     #$00
        jsr     _close

        ; Set return value if we had an error
        lda     ___errno
        beq     done
        lda     #$FF
done:
        tax
        rts

        .bss

stbuf:  .res 2
