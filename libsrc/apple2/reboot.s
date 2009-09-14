;
; Oliver Schmidt, 14.09.2009
;
; void rebootafterexit (void);
;

        .constructor	initreboot
        .export	        _rebootafterexit
        .import         done, return

_rebootafterexit := return

        .segment        "INIT"

initreboot:
        ; Quit to PWRUP
        lda     #<$FAA6
        ldx     #>$FAA6
        sta     done
        stx     done+1
        rts
