;
; int __fastcall__ vcprintf (const char* Format, va_list ap);
;
; Ullrich von Bassewitz, 2.12.2000
;

        .export         _vcprintf
        .import         pushax, popax
        .import         __printf, _cputc
        .importzp       sp, ptr1, ptr2, ptr3, tmp1

        .macpack        generic


.data

; ----------------------------------------------------------------------------
;
; Static data for the _vsprintf routine
;

outdesc:                        ; Static outdesc structure
        .word   0               ; ccount
        .word   out             ; Output function pointer
        .word   0               ; ptr
        .word   0               ; uns

.code

; ----------------------------------------------------------------------------
; Callback routine used for the actual output.
;
; static void __cdecl__ out (struct outdesc* d, const char* buf, unsigned count)
; /* Routine used for writing */
; {
;     /* Fast screen output */
;     d->ccount += count;
;     while (count) {
;         cputc (*buf);
;         ++buf;
;         --count;
;     }
; }
;
; We're using ptr1 and tmp1, since we know that the cputc routine will not use
; them (they're also used in cputs, so they must be safe).

out:    jsr     popax           ; count
        sta     ptr2
        eor     #$FF
        sta     outdesc+6
        txa
        sta     ptr2+1
        eor     #$FF
        sta     outdesc+7

        jsr     popax           ; buf
        sta     ptr1
        stx     ptr1+1

        jsr     popax           ; d
        sta     ptr3
        stx     ptr3+1

; Sum up the total count of characters

        ldy     #0              ; ccount in struct outdesc
        sty     tmp1            ; Initialize tmp1 while we have zero available
        lda     (ptr3),y
        add     ptr2
        sta     (ptr3),y
        iny
        lda     (ptr3),y
        adc     ptr2+1
        sta     (ptr3),y

; Loop outputting characters

@L1:    inc     outdesc+6
        beq     @L4
@L2:    ldy     tmp1
        lda     (ptr1),y
        iny
        bne     @L3
        inc     ptr1+1
@L3:    sty     tmp1
        jsr     _cputc
        jmp     @L1

@L4:    inc     outdesc+7
        bne     @L2
        rts

; ----------------------------------------------------------------------------
; vcprintf - formatted console i/o
;
; int __fastcall__ vcprintf (const char* format, va_list ap)
; {
;     struct outdesc d;
;
;     /* Setup descriptor */
;     d.fout = out;
;
;     /* Do formatting and output */
;     _printf (&d, format, ap);
;
;     /* Return bytes written */
;     return d.ccount;
; }

_vcprintf:
        sta     ptr1            ; Save ap
        stx     ptr1+1

; Setup the outdesc structure

        lda     #0
        sta     outdesc
        sta     outdesc+1       ; Clear ccount

; Get the format parameter and push it again

        ldy     #1
        lda     (sp),y
        tax
        dey
        lda     (sp),y
        jsr     pushax

; Replace the passed format parameter on the stack by &d - this creates
; exactly the stack frame _printf expects. Parameters will get dropped
; by _printf.

        ldy     #2              ; Low byte of d
        lda     #<outdesc
        sta     (sp),y
        iny
        lda     #>outdesc
        sta     (sp),y

; Restore ap and call _printf

        lda     ptr1
        ldx     ptr1+1
        jsr     __printf

; Return the number of bytes written.

        lda     outdesc         ; ccount
        ldx     outdesc+1
        rts
