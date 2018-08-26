;
; Ullrich von Bassewitz, 11.06.1998
; Christian Krueger: 08-Aug-2013, optimization
;
; size_t strspn (const char* s1, const char* s2);
;

        .export         _strspn
        .import         popptr1, _strlen
        .importzp       ptr1, ptr2, tmp1, tmp2

_strspn:
        jsr _strlen         ; get length in a/x and transfer s2 to ptr2
                            ; Note: It does not make sense to
                            ; have more than 255 test chars, so
                            ; we don't support a high byte here! (ptr2+1 is
                            ; also unchanged in strlen then (important!))
                            ; -> the original implementation also
                            ; ignored this case

        sta tmp1            ; tmp1 = strlen of test chars
        jsr popptr1         ; get and save s1 to ptr1

        ldx #0              ; low counter byte
        stx tmp2            ; high counter byte

loadChar:
        ldy #0
        lda (ptr1),y        ; get next char from s1
        beq leave           ; handly byte of s1
advance:
        inc ptr1            ; advance string position to test
        bne check
        inc ptr1+1
        dey                 ; correct next iny (faster/shorter than bne...)

checkNext:
        iny
check:  cpy tmp1            ; compare with length of test character string
        beq leave
        cmp (ptr2),y        ; found matching char?
        bne checkNext

foundTestChar:
        inx
        bne loadChar
        inc tmp2
        bne loadChar        ; like bra...

leave:  txa                 ; restore position of finding
        ldx tmp2            ; and return
        rts
