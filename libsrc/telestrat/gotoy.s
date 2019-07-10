;
; jede jede@oric.org 2017-02-25
;
    .export    _gotoy

    .import    update_adscr

    .include   "telestrat.inc"

.proc _gotoy
   sta     SCRY
   jsr     update_adscr

   ; We change the current line, it means that we need to put color attributes again.
   ; That is not the case with _gotox because, it's on the same line attribute are already set
   rts
.endproc
