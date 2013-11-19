;
; Atari XL, shared data between 2nd load chunk and main chunk, definition file
;
; Contains old values of modified system variables and ports.
;
; Christian Groessler, chris@groessler.org, 2013
;

.ifdef __ATARIXL__

.export         SAVMSC_save
.export         MEMTOP_save
.export         APPMHI_save
.export         RAMTOP_save
.export         PORTB_save

.segment        "LOWBSS"

SAVMSC_save:    .res    2
MEMTOP_save:    .res    2
APPMHI_save:    .res    2
RAMTOP_save:    .res    1
PORTB_save:     .res    1

.endif
