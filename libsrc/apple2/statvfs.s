;
; Colin Leroy-Mira, 2023 <colin@colino.net>
;
; int __fastcall__ statvfs(const char *pathname, struct statvfs *statvfsbuf);
;

        .export         _statvfs
        .import         _dio_query_sectsize
        .import         mli_file_info, pushax, popax, popptr1, pushptr1
        .include        "zeropage.inc"
        .include        "apple2.inc"
        .include        "errno.inc"
        .include        "mli.inc"
        .include        "statvfs.inc"

_statvfs:
        ; Store statbuf
        sta     ptr4
        stx     ptr4+1

        ; Clear statbuf
        lda     #$00
        ldy     #.sizeof(statvfs)-1
:       sta     (ptr4),y
        dey
        bpl     :-

        ; Store pathname, keeping only volume name
        jsr     popptr1
        ldy     #$00
        sty     vol_sep
        lda     (ptr1),y
        cmp     #'/'            ; Is the path absolute?
        beq     :+
        lda     #EINVAL
        jmp     ___directerrno

:       iny
        lda     (ptr1),y
        beq     :+              ; End of string, no other /
        cpy     #FILENAME_MAX
        beq     :+              ; Max filename length reached
        cmp     #'/'
        bne     :-              ; Not a slash, keep looking
        sty     vol_sep         ; Register '/' index
        lda     #$00
        sta     (ptr1),y        ; Cut pathname at first slash
:       jsr     pushptr1

        jsr     mli_file_info

        php
        ldy     vol_sep         ; Put slash back in pathname
        lda     #'/'
        sta     (ptr1),y
        plp

        bcc     got_info

        jmp     ___mappederrno

got_info:
        ; f_fsid
        lda     DEVNUM
        lsr                     ; Shift right to cc65 representation
        lsr
        lsr
        lsr
        ldy     #statvfs::f_fsid
        sta     (ptr4),y

        ; total number of blocks
        lda     mliparam + MLI::INFO::AUX_TYPE
        ldy     #statvfs::f_blocks
        sta     (ptr4),y
        lda     mliparam + MLI::INFO::AUX_TYPE+1
        iny
        sta     (ptr4),y

        ; blocks free & avail
        sec
        lda     mliparam + MLI::INFO::AUX_TYPE
        sbc     mliparam + MLI::INFO::BLOCKS
        ldy     #statvfs::f_bfree
        sta     (ptr4),y
        ldy     #statvfs::f_bavail
        sta     (ptr4),y

        lda     mliparam + MLI::INFO::AUX_TYPE+1
        sbc     mliparam + MLI::INFO::BLOCKS+1
        iny
        sta     (ptr4),y
        ldy     #statvfs::f_bfree+1
        sta     (ptr4),y

        ; block sizes
        jsr     _dio_query_sectsize
        ; low bytes
        ldy     #statvfs::f_bsize
        sta     (ptr4),y
        ldy     #statvfs::f_frsize
        sta     (ptr4),y
        ; f_frsize high byte
        iny
        txa
        sta     (ptr4),y
        ; f_bsize high byte
        ldy     #statvfs::f_bsize+1
        sta     (ptr4),y

        ; f_namemax
        lda     #FILENAME_MAX
        ldy     #statvfs::f_namemax
        sta     (ptr4),y

        lda     #$00
        sta     ___errno
        tax
        rts

        .bss

vol_sep:.res 1
