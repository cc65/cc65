;
; Ullrich von Bassewitz, 2009-11-01
;
; void tgi_free_vectorfont (const tgi_vectorfont* font);
; /* Free a vector font that was previously loaded into memory. */
;
; This function is identical to free(void*)
;

        .import _free
        .export _tgi_free_vectorfont := _free

