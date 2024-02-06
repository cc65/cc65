;
; Ullrich von Bassewitz, 2002-12-20
;
; unsigned char joy_count (void);
; /* Return the number of joysticks supported by the driver */
;

        .include        "joy-kernel.inc"

        _joy_count      = joy_count             ; Use driver entry
