;
; Colin Leroy-Mira, 2024
;
; void* __fastcall__ realloc (void* block, register size_t size)
;

        .importzp       ptr1, ptr2, ptr3, ptr4, tmp1, tmp2, tmp3, tmp4, c_sp
        .import         _malloc, _memcpy, _free
        .import         pushax, popptr1, return0
        .import          incsp2, decsp2
        .export         _realloc

        .include        "_heap.inc"

        .macpack        generic

;----------------------------------------------------------------------------
; Aliases for clarity

block     = ptr1
size      = ptr2
ublock    = ptr3
oldsize   = ptr4
newblock  = tmp1                ; (and tmp2)
orgblock  = tmp3                ; (and tmp4)

;----------------------------------------------------------------------------
; Code

_realloc:
        sta     size            ; Store size
        stx     size+1

        jsr     popptr1         ; Pop block

        lda     block+1         ; Is block null?
        tax
        ora     block
        bne     :+

        lda     size            ; Block is null, just malloc
        ldx     size+1
        jmp     _malloc

:       lda     size            ; Is size 0?
        ora     size+1
        bne     :+

        lda     block           ; It is: free block (high byte already in X)
        jsr     _free
        jmp     return0

:       clc                     ; Add internal used size
        lda     size
        adc     #HEAP_ADMIN_SPACE
        sta     size
        bcc     :+
        inc     size+1
        bne     :+

        lda     #$00            ; Size high byte now 0: We overflowed!
        tax
        rts

:       ldx     size+1          ; Should we round size up?
        bne     :+
        cmp     #.sizeof (freeblock)
        bcs     :+

        lda     #.sizeof (freeblock)
        sta     size            ; (we presuppose that sizeof (freeblock) is < 256)

:       lda     block           ; Get pointer to raw memory block
        sta     orgblock        ; Store original pointer
        sec
        sbc     #.sizeof(usedblock)
        sta     ublock
        lda     block+1
        sta     orgblock+1      ; Finish storing original pointer
        sbc     #0
        sta     ublock+1        ; We have our usedblock struct

                                ; Get block start
        ldy     #usedblock::start+1
        lda     (ublock),y
        tax                     ; Backup ublock high
        dey
        lda     (ublock),y

        sta     ublock          ; Store ublock
        stx     ublock+1

                                ; Remember oldsize
        ldy     #usedblock::size+1
        lda     (ublock),y
        sta     oldsize+1
        dey
        lda     (ublock),y
        sta     oldsize

        clc                     ; Is the block at heap top?
        adc     ublock
        tay
        lda     ublock+1
        adc     oldsize+1
        cmp     ___heapptr+1
        bne     must_malloc_new
        cpy     ___heapptr
        bne     must_malloc_new

        tya                     ; Put ___heapptr back in A
        sec                     ; Check if we have enough memory at heap top
        sbc     oldsize         ; Subtract oldsize
        sta     newblock
        lda     ___heapptr+1
        sbc     oldsize+1
        sta     newblock+1
        clc
        lda     newblock        ; And add size
        adc     size
        sta     newblock
        lda     newblock+1
        adc     size+1
        sta     newblock+1
        bcs     must_malloc_new ; If we have a carry there we overflowed

        cmp     ___heapend+1
        bne     :+
        lda     newblock
        cmp     ___heapend
:       bcc     :+
        bne     must_malloc_new

:       lda     newblock        ; There is enough space
        sta     ___heapptr      ; Update heapptr
        lda     newblock+1
        sta     ___heapptr+1

        ldy     #usedblock::start+1
        lda     ublock+1
        sta     (ublock),y      ; Update block start
        dey
        lda     ublock
        sta     (ublock),y
        dey

        .assert usedblock::size = usedblock::start-2, error
        lda     size+1
        sta     (ublock),y      ; Update block size
        dey
        lda     size
        sta     (ublock),y

        lda     orgblock        ; Return original block
        ldx     orgblock+1
        rts

must_malloc_new:                ; The block is not at heap top, or too big
        lda     size+1
        pha                     ; Backup new size (at this point the only ptr
        tax                     ; we'll need after malloc). tmp* are safe
        lda     size            ; from malloc, memcpy and free.
        pha
        jsr     _malloc

        cmp     #$00            ; Did malloc succeed?
        bne     :+
        cpx     #$00
        bne     :+
        pla                     ; Pop size backup and return NULL
        pla
        txa                     ; X already 0
        rts                     ; No

:       sta     newblock        ; Yes, store newblock
        stx     newblock+1
        jsr     pushax          ; Push newblock for memcpy

        lda     orgblock        ; Push orgblock for memcpy
        ldx     orgblock+1
        jsr     pushax

        sec                     ; Remove admin space from oldsize
        lda     oldsize
        sbc     #<HEAP_ADMIN_SPACE
        sta     oldsize
        lda     oldsize+1
        sbc     #>HEAP_ADMIN_SPACE
        sta     oldsize+1

        pla                     ; Restore new size to AX
        tay
        pla
        tax
        tya

        cmp     oldsize         ; Find the smallest size
        bcc     :+
        cpx     oldsize+1
        bcc     :+

        lda     oldsize
        ldx     oldsize+1

:       jsr     _memcpy         ; And copy data

        lda     orgblock        ; Free old block
        ldx     orgblock+1
        jsr     _free

        lda     newblock        ; Return new block
        ldx     newblock+1
        rts
