; ---------------------------------------------------------------------------
;
; SYM-1 definitions
;
; ---------------------------------------------------------------------------


RAMSTART := $0200     ; Entry point


; ---------------------------------------------------------------------------
; Monitor Functions
; ---------------------------------------------------------------------------
WARM     := $8003     ; Monitor entry
SVNMI    := $809B     ; Save NMI entry
INBYTE   := $81D9     ; Get two HEX characters and pack
ASCNIB   := $8275     ; Test for carriage-return
INCCMP   := $82B2     ; Increment pointer
CHKSAD   := $82DD     ; Compute checksum
OUTPC    := $82EE     ; Display program counter
OUTBYT   := $82FA     ; Print byte as two ASCII characters
OUTS2    := $8319     ; Print pointer
INSTAT   := $8386     ; Determine if key is pressed
GETKEY   := $88AF     ; Get key (disregarding monitor login)
SCAND    := $8906     ; Flash LED display (once)
KEYQ     := $8923     ; Test for keypress
BEEP     := $8972     ; Make a beep
CONFIG   := $89A5     ; Configure I/O
OUTDSP   := $89C1     ; Output to on-board LED display
INCHR    := $8A1B     ; Input character and convert to uppercase
OUTCHR   := $8A47     ; Output character
INTCHR   := $8A58     ; Input character without case conversion
DLYF     := $8AE6     ; Delay 1 bit time
DLYH     := $8AE9     ; Delay 1/2 bit time
RESET    := $8B4A     ; Hard reset
ACCESS   := $8B86     ; Unlock lowest 4K memory
NACCES   := $8B9C     ; Lock lowest 4K memory
L8C78    := $8C78     ; Link to tape
DUMPT    := $8E87     ; Dump memory to tape
LOADT    := $8C78     ; Load memory from tape
TAPEMODE := $00FD     ; Top bit on for high-speed


; ---------------------------------------------------------------------------
; System Memory
; ---------------------------------------------------------------------------
DISBUF   := $A640     ; On-Board Display Buffer
DISBUF0  := $A640     ; Left-Most digit
DISBUF1  := $A641     ; Second digit
DISBUF2  := $A642     ; Third
DISBUF3  := $A643     ; Fourth
DISBUF4  := $A644     ; Fifth
DISBUF5  := $A645     ; Sixth and right-most digit
DISBUF6  := $A646     ; Not-used / right of display (shift buffer)
RDIG     := $A645     ; Right-most digit (same as DISBUF5)
P3L      := $A64A     ; Parameter 3 (low-byte)
P3H      := $A64B     ;             (high-byte)
P2L      := $A64C     ; Parameter 2
P2H      := $A64D     ;
P1L      := $A64E     ; Parameter 1
P1H      := $A64F
PARNR    := $A649     ; Number of Parameters Entered
PADBIT   := $A650     ; Pad Bits for Carriage Return
SDBYT    := $A651     ; Baud Rate for RS232 (01-4800,06-2400,10-1200,24-600,4C-300,D5-110)
ERCNT    := $A652     ; Error Count (Max FF)
TECHO    := $A653     ; Terminal Echo (bit-7=ECHO/NO, 6=CTL-O TOGGLE)
TOUTFL   := $A654     ; Output Flags (bit-7=CRT IN, 6=TTY IN, 5=TTY OUT, 4=CRT OUT)
KSHFL    := $A655     ; Keyboard Shift Flag
TV       := $A656     ; Trace Velocity (0=Single Step)
LSTCOM   := $A657     ; Last Monitor Command
MAXRC    := $A658     ; Maximum Record Length for Memory Dump


; ---------------------------------------------------------------------------
; Register Followers
; ---------------------------------------------------------------------------
PCLR     := $A659     ; Program Counter (low-byte)
PCHR     := $A65A     ;                 (high-byte)
SR       := $A65B     ; Stack Pointer
FR       := $A65C     ; Status Register Flags
AR       := $A65D     ; A Register
XR       := $A65E     ; X Register
YR       := $A65F     ; Y Register


; ---------------------------------------------------------------------------
; I/O Vectors (3 bytes each)
; ---------------------------------------------------------------------------
INVEC    := $A660     ; Input Character
OUTVEC   := $A663     ; Output Character
INSVEC   := $A666     ; Input Status
URSVEC   := $A669     ; Unrecognized Syntax
URCVEC   := $A66C     ; Unrecognized Command / Error
SCNVEC   := $A66F     ; Scan On-board Display


; ---------------------------------------------------------------------------
; Trace and Interrupt Vectors (2 bytes each)
; ---------------------------------------------------------------------------
EXEVEC   := $A672     ; Exec and Alternate InVec
TRCVEC   := $A674     ; Trace
UBRKVC   := $A676     ; User Break after Monitor
UIRQVC   := $A678     ; User non-break IRQ after Monitor
NMIVEC   := $A67A     ; Non-Maskable Interrupt
RSTVEC   := $A67C     ; Reset
IRQVEC   := $A67E     ; Interrupt Request


; ---------------------------------------------------------------------------
; I/O Registers
; ---------------------------------------------------------------------------
;
; 6532 (U27)
;
PADA     := $A400     ; Keyboard / Display
P3DA     := $A402     ; Serial I/O
DDPADA   := $A401     ; Data-Direction Register for PADA
DDP3DA   := $A403     ; Data-Direction Register for P3DA
WEDRTA   := $A404     ; Write-Edge Detect Read Timer A
WEDRFA   := $A405     ; Write-Edge Detect Read-Int Flags A
WEDRTB   := $A406     ; Write-Edge Detect Read Timer B
WEDRFB   := $A407     ; Write-Edge Detect Read-Int Flags B
TIM0001  := $A41C     ; Timer / 1
TIM0008  := $A41D     ; Timer / 8
TIM0064  := $A41E     ; Timer / 64
TIM1024  := $A41F     ; Timer / 1024
;
; 6522 (U25)
;
OR1A     := $A001     ; Input / Output Register for 1A
DDR1A    := $A003     ; Data-Direction Register for 1A
OR1B     := $A000     ; Input / Output Register for 1B
DDR1B    := $A002     ; Data-Direction Register for 1B
TIC1L    := $A004     ;
TIC1H    := $A005     ;
TIL1L    := $A006     ;
TIL1H    := $A007     ;
T2L1L    := $A008     ;
T2C1L    := $A008     ;
T2C1H    := $A009     ;
SR1      := $A00A     ;
ACR1     := $A00B     ;
PCR1     := $A00C     ;
IFR1     := $A00D     ;
IER1     := $A00E     ;
DR1A     := $A00F     ;
;
; 6522 (U28)
;
OR2A     := $A801     ; Input / Output Register for 2A
DDR2A    := $A803     ; Data-Direction Register for 2A
OR2B     := $A800     ; Input / Output Register for 2B
DDR2B    := $A802     ; Data-Direction Register for 2B
TIC2L    := $A804     ;
TIC2H    := $A805     ;
TIL2L    := $A806     ;
TIL2H    := $A807     ;
T2L2L    := $A808     ;
T2C2L    := $A808     ;
T2C2H    := $A809     ;
SR2      := $A80A     ;
ACR2     := $A80B     ;
PCR2     := $A80C     ;
IFR2     := $A80D     ;
IER2     := $A80E     ;
DR2A     := $A80F     ;
;
; 6522 (U29)
;
OR3A     := $AC01     ; Write-Protect RAM, Debug On/Off, I/O-3A
DDR3A    := $AC03     ; Data-Direction Register for 3A
OR3B     := $AC00     ; Input / Output Register for 3B
DDR3B    := $AC02     ; Data-Direction Register for 3B
TIC3L    := $AC04     ;
TIC3H    := $AC05     ;
TIL3L    := $AC06     ;
TIL3H    := $AC07     ;
T2L3L    := $AC08     ;
T2C3L    := $AC08     ;
T2C3H    := $AC09     ;
SR3      := $AC0A     ;
ACR3     := $AC0B     ;
PCR3     := $AC0C     ;
IFR3     := $AC0D     ;
IER3     := $AC0E     ;
DR3A     := $AC0F     ;
