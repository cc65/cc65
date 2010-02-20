;
; Ullrich von Bassewitz, 2003-12-27
;
; Additional zero page locations for the CBM510.
;

; ------------------------------------------------------------------------

        .globalzp       sysp1, sysp3, vic, sid, ipccia, cia1, cia2, acia
        .globalzp       tpi1, tpi2, ktab1, ktab2, ktab3, ktab4

        .globalzp       sysp0, time, segsave, ktmp, CURS_X, CURS_Y, CURS_FLAG
        .globalzp       CURS_STATE, CURS_BLINK, CURS_COLOR, CHARCOLOR, RVS
        .globalzp       DEVNUM, SCREEN_PTR, CRAM_PTR
        .globalzp       keyidx, keybuf, keyscanbuf, keysave, modkey
        .globalzp       norkey, graphmode, lastidx, rptdelay, rptcount



