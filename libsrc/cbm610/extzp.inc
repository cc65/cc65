;
; Ullrich von Bassewitz, 2003-12-20
;
; Additional zero page locations for the CBM610.
;

; ------------------------------------------------------------------------

        .globalzp       sysp1, sysp3, crtc, sid, ipccia, cia, acia, tpi1, tpi2
        .globalzp       ktab1, ktab2, ktab3, ktab4

        .globalzp       sysp0, time, segsave, ktmp, CURS_X, CURS_Y, RVS, DEVNUM
        .globalzp       config, CharPtr, keyidx, keybuf, keyscanbuf, keysave
        .globalzp       modkey, norkey, graphmode, lastidx, rptdelay, rptcount


