;-------------------------------------------------------------------------
; POKEY Address Equates
;-------------------------------------------------------------------------

; Read Addresses

POT0    = POKEY + $00         ;potentiometer 0
POT1    = POKEY + $01         ;potentiometer 1
POT2    = POKEY + $02         ;potentiometer 2
POT3    = POKEY + $03         ;potentiometer 3
POT4    = POKEY + $04         ;potentiometer 4
POT5    = POKEY + $05         ;potentiometer 5
POT6    = POKEY + $06         ;potentiometer 6
POT7    = POKEY + $07         ;potentiometer 7

ALLPOT  = POKEY + $08         ;potentiometer port status
KBCODE  = POKEY + $09         ;keyboard code
RANDOM  = POKEY + $0A         ;random number generator
SERIN   = POKEY + $0D         ;serial port input
IRQST   = POKEY + $0E         ;IRQ interrupt status
SKSTAT  = POKEY + $0F         ;serial port and keyboard status

; Write Addresses

AUDF1   = POKEY + $00         ;channel 1 audio frequency
AUDC1   = POKEY + $01         ;channel 1 audio control

AUDF2   = POKEY + $02         ;channel 2 audio frequency
AUDC2   = POKEY + $03         ;channel 2 audio control

AUDF3   = POKEY + $04         ;channel 3 audio frequency
AUDC3   = POKEY + $05         ;channel 3 audio control

AUDF4   = POKEY + $06         ;channel 4 audio frequency
AUDC4   = POKEY + $07         ;channel 4 audio control

AUDCTL  = POKEY + $08         ;audio control
STIMER  = POKEY + $09         ;start timers
SKRES   = POKEY + $0A         ;reset SKSTAT status
POTGO   = POKEY + $0B         ;start potentiometer scan sequence
SEROUT  = POKEY + $0D         ;serial port output
IRQEN   = POKEY + $0E         ;IRQ interrupt enable
SKCTL   = POKEY + $0F         ;serial port and keyboard control
