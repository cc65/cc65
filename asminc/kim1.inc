; ---------------------------------------------------------------------------
;
; KIM-1 definitions
;
; ---------------------------------------------------------------------------


RAMSTART := $0200     ; Entry point


; ---------------------------------------------------------------------------
; Monitor Functions
; ---------------------------------------------------------------------------
OUTCHR   := $1EA0     ; Output character
INTCHR   := $1E5A     ; Input character without case conversion
DUMPT    := $1800     ; Dump memory to tape
LOADT    := $1873     ; Load memory from tape
START    := $1C4F     ; Enter KIM-1 monitor
SCANDS   := $1F1F     ; Scan 7-segment display
KEYIN    := $1F40     ; Open up keyboard channel
GETKEY   := $1F6A     ; Return key from keyboard


; ---------------------------------------------------------------------------
; System Memory
; ---------------------------------------------------------------------------
SAL      := $17F5     ; Tape load address low
SAH      := $17F6     ; Tape load address high
EAL      := $17F7     ; Tape address end low
EAH      := $17F8     ; Tape address end high
ID       := $17F9     ; Tape Identification number
