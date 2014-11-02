;
; Standard joystick driver for the C16. May be used multiple times when linked
; to the statically application.
;
; Ullrich von Bassewitz, 2002-12-21
;

        .define         MODULE_LABEL    _c16_stdjoy_joy

        .include        "../../plus4/joy/plus4-stdjoy.s"
