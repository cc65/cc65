;
; Ullrich von Bassewitz, 2002-12-20
;
; unsigned char __fastcall__ joy_read (unsigned char joystick);
; /* Read a particular joystick */
;

        .include        "joy-kernel.inc"

        _joy_read       = joy_read              ; Use driver entry

                                                
