;-------------------------------------------------------------------------
; Atari System Equates
; by Freddy Offenga, Christian Groessler, and Christian Krueger
;
; References:
; - Atari 400/800 OS rev.B source code, Atari 1979
; - Atari OS manual - XL addendum
; - Atari XL/XE rev.2 source code, Atari 1984
; - Mapping the Atari - revised edition, Ian Chadwick 1985
; - SpartaDOS-X User Guide  (Aug-8-2016)
;
; ##old##       old OS rev.B label - moved or deleted
; ##1200xl##    new label introduced in 1200XL OS (rev.10/11)
; ##rev2##      new label introduced in XL/XE OS rev.2
;-------------------------------------------------------------------------

;-------------------------------------------------------------------------
; Configuration Equates
;-------------------------------------------------------------------------

MAXDEV  = 33            ;offset to last possible entry of HATABS
IOCBSZ  = 16            ;length of IOCB

SEIOCB  = 0*IOCBSZ      ;##rev2## screen editor IOCB index
MAXIOC  = 8*IOCBSZ      ;first invalid IOCB index

DSCTSZ  = 128           ;##rev2## disk sector size

LEDGE   = 2             ;left edge
REDGE   = 39            ;right edge

INIML   = $0700         ;##rev2## initial MEMLO

ICSORG  = $CC00         ;##rev2## international character set origin
DCSORG  = $E000         ;##rev2## domestic character set origin

; IOCB Command Code Equates

OPEN    = $03           ;open
GETREC  = $05           ;get record
GETCHR  = $07           ;get character(s)
PUTREC  = $09           ;put record
PUTCHR  = $0B           ;put character(s)
CLOSE   = $0C           ;close
STATIS  = $0D           ;status
SPECIL  = $0E           ;special

; DOS IOCB command codes

RENAME  = $20           ;rename disk file
DELETE  = $21           ;delete disk file
LOCKFL  = $23           ;lock file (set to read-only)
UNLOCK  = $24           ;unlock file
POINT   = $25           ;point sector
NOTE    = $26           ;note sector
GETFL   = $27           ;get file length
CHDIR_MYDOS     = $29   ;change directory (MyDOS)
MKDIR   = $2A           ;make directory (MyDOS/SpartaDOS)
RMDIR   = $2B           ;remove directory (SpartaDOS)
CHDIR_SPDOS     = $2C   ;change directory (SpartaDOS)
FORMAT  = $FE           ;format

GETCWD  = $30           ;get current directory (MyDOS/SpartaDOS)

; Special Entry Command Equates
; DOS Commands
; Command line table, Index values for (DOSVEC),Y -- COMTAB
; Compatible with OS/A+, DOS XL and SpartaDOS

COMTAB  = 0             ;DOS entry jump vector
ZCRNAME = 3             ;file name crunch routine jump vector
BUFOFF  = 10            ;next parameter buffer offset
COMFNAM = 33            ;destination buffer for crunch routine
LBUF    = 63            ;command line input buffer

; Screen IOCB Commands

DRAWLN  = $11           ;draw line
FILLIN  = $12           ;draw line with right fill

; ICAX1 Auxiliary Byte 1 Equates

APPEND  = $01           ;open write append (D:)
DIRECT  = $02           ;open for directory access (D:)
OPNIN   = $04           ;open for input (all devices)
OPNOT   = $08           ;open for output (all devices)
MXDMOD  = $10           ;open for mixed mode (E:, S:)
INSCLR  = $20           ;open for input without clearing screen

; Device Code Equates

CASSET  = 'C'           ;cassette
DISK    = 'D'           ;disk
SCREDT  = 'E'           ;screen editor
KBD     = 'K'           ;keyboard
PRINTR  = 'P'           ;printer
DISPLY  = 'S'           ;screen display

; SIO Command Code Equates

SIO_FORMAT   = $21      ;format disk (default density)
SIO_FORMATS  = $22      ;1050: format medium density
SIO_CONFIG   = $44      ;configure drive
SIO_CONFIGSF = $4B      ;slow/fast configure drive??
SIO_RDPERCOM = $4E      ;read PERCOM block (XF551)
SIO_WRPERCOM = $4F      ;write PERCOM block (XF551)
SIO_WRITE    = $50      ;write sector
SIO_READ     = $52      ;read sector
SIO_STAT     = $53      ;get status information
SIO_VERIFY   = $56      ;verify sector
SIO_WRITEV   = $57      ;write sector with verify
SIO_WRITETRK = $60      ;write track (Speedy)
SIO_READTRK  = $62      ;read track (Speedy)

; SIO Status Code (DSTATS)
; Input: data direction
;     Bit #7 - W (write operation)
;         #6 - R (read operation)
; Output: status code
;     $01 (001) -- OPERATION COMPLETE (NO ERRORS)
;     $8A (138) -- DEVICE TIMEOUT (DOESN'T RESPOND)
;     $8B (139) -- DEVICE NAK
;     $8C (140) -- SERIAL BUS INPUT FRAMING ERROR
;     $8E (142) -- SERIAL BUS DATA FRAME OVERRUN ERROR
;     $8F (143) -- SERIAL BUS DATA FRAME CHECKSUM ERROR
;     $90 (144) -- DEVICE DONE ERROR

; Character and Key Code Equates

CLS     = $7D           ;##rev2## clear screen
EOL     = $9B           ;end of line (RETURN)

HELP    = $11           ;##1200xl## key code for HELP
CNTLF1  = $83           ;##1200xl## key code for CTRL-F1
CNTLF2  = $84           ;##1200xl## key code for CTRL-F2
CNTLF3  = $93           ;##1200xl## key code for CTRL-F3
CNTLF4  = $94           ;##1200xl## key code for CTRL-F4
CNTL1   = $9F           ;##1200xl## key code for CTRL-1

; Status Code Equates

SUCCES  = 1             ;($01) succesful operation

BRKABT  = 128           ;($80) BREAK key abort
PRVOPN  = 129           ;($81) IOCB already open error
NONDEV  = 130           ;($82) nonexistent device error
WRONLY  = 131           ;($83) IOCB opened for write only error
NVALID  = 132           ;($84) invalid command error
NOTOPN  = 133           ;($85) device/file not open error
BADIOC  = 134           ;($86) invalid IOCB index error
RDONLY  = 135           ;($87) IOCB opened for read only error
EOFERR  = 136           ;($88) end of file error
TRNRCD  = 137           ;($89) truncated record error
TIMOUT  = 138           ;($8A) peripheral device timeout error
DNACK   = 139           ;($8B) device does not acknowledge command
FRMERR  = 140           ;($8C) serial bus framing error
CRSROR  = 141           ;($8D) cursor overrange error
OVRRUN  = 142           ;($8E) serial bus data overrun error
CHKERR  = 143           ;($8F) serial bus checksum error
DERROR  = 144           ;($90) device done (operation incomplete)
BADMOD  = 145           ;($91) bad screen mode number error
FNCNOT  = 146           ;($92) function not implemented in handler
SCRMEM  = 147           ;($93) insufficient memory for screen mode

DSKFMT  = 148           ;($94) SpartaDOS: unrecognized disk format
INCVER  = 149           ;($95) SpartaDOS: disk was made with incompat. version
DIRNFD  = 150           ;($96) SpartaDOS: directory not found
FEXIST  = 151           ;($97) SpartaDOS: file exists
NOTBIN  = 152           ;($98) SpartaDOS: file not binary
LSYMND  = 154           ;($9A) SDX: loader symbol not defined
BADPRM  = 156           ;($9C) SDX: bad parameter
OUTOFM  = 158           ;($9E) SDX: out of memory
INVDEV  = 160           ;($A0) invalid device number
TMOF    = 161           ;($A1) too many open files
DSKFLL  = 162           ;($A2) disk full
FATLIO  = 163           ;($A3) fatal I/O error
FNMSMT  = 164           ;($A4) internal file number mismatch
INVFNM  = 165           ;($A5) invalid file name
PDLERR  = 166           ;($A6) point data length error
EPERM   = 167           ;($A7) permission denied
DINVCM  = 168           ;($A8) command invalid for disk
DIRFLL  = 169           ;($A9) directory full
FNTFND  = 170           ;($AA) file not found
PNTINV  = 171           ;($AB) point invalid
BADDSK  = 173           ;($AD) bad disk
INCFMT  = 176           ;($B0) DOS 3: incompatible file system
XNTBIN  = 180           ;($B4) XDOS: file not binary

; DCB Device Bus Equates

DISKID  = $31           ;##rev2## disk bus ID
PDEVN   = $40           ;##rev2## printer bus ID
CASET   = $60           ;##rev2## cassette bus ID

; Bus Command Equates

FOMAT   = '!'           ;##rev2## format command
PUTSEC  = 'P'           ;##rev2## put sector command
READ    = 'R'           ;##rev2## read command
STATC   = 'S'           ;##rev2## status command
WRITE   = 'W'           ;##rev2## write command

; Command Auxiliary Byte Equates

DOUBLE  = 'D'           ;##rev2## print 20 characters double width
NORMAL  = 'N'           ;##rev2## print 40 characters normally
PLOT    = 'P'           ;##rev2## plot
SIDWAY  = 'S'           ;##rev2## print 16 characters sideways

; Bus Response Equates

ACK     = 'A'           ;##rev2## device acknowledged
COMPLT  = 'C'           ;##rev2## device succesfully completed operation
ERROR   = 'E'           ;##rev2## device incurred error
NACK    = 'N'           ;##rev2## device did not understand

; Floating Point Miscellaneous Equates

FPREC   = 6             ;precision

FMPREC  = FPREC-1       ;##rev2## length of mantissa

; Cassette Record Type Equates

HDR     = $FB           ;##rev2## header
DTA     = $FC           ;##rev2## data record
DT1     = $FA           ;##rev2## last data record
EOT     = $FE           ;##rev2## end of tape (file)

TONE1   = 2             ;##rev2## record
TONE2   = 1             ;##rev2## playback

; Cassette Timing Equates

WLEADN  = 1152          ;##rev2## NTSC 19.2 second WRITE file leader
RLEADN  = 576           ;##rev2## NTSC 9.6 second READ file leader
WIRGLN  = 180           ;##rev2## NTSC 3.0 second WRITE IRG
RIRGLN  = 120           ;##rev2## NTSC 2.0 second READ IRG
WSIRGN  = 15            ;##rev2## NTSC 0.25 second WRITE short IRG
RSIRGN  = 10            ;##rev2## NTSC 0.16 second READ short IRG
BEEPNN  = 30            ;##rev2## NTSC 0.5 second beep duration
BEEPFN  = 10            ;##rev2## NTSC 0.16 seconrd beep duration

WLEADP  = 960           ;##rev2## PAL 19.2 second WRITE file leader
RLEADP  = 480           ;##rev2## PAL 9.6 second READ file leader
WIRGLP  = 150           ;##rev2## PAL 3.0 second WRITE IRG
RIRGLP  = 100           ;##rev2## PAL 2.0 second READ IRG
WSIRGP  = 13            ;##rev2## PAL 0.25 second WRITE short IRG
RSIRGP  = 8             ;##rev2## PAL 0.16 second READ short IRG
BEEPNP  = 25            ;##rev2## PAL 0.5 second beep duration
BEEPFP  = 8             ;##rev2## PAL 0.16 seconrd beep duration

WIRGHI  = 0             ;##rev2## high WRITE IRG
RIRGHI  = 0             ;##rev2## high READ IRG

; Power-up Validation Byte Value Equates

PUPVL1  = $5C           ;##rev2## power-up validation value 1
PUPVL2  = $93           ;##rev2## power-up validation value 2
PUPVL3  = $25           ;##rev2## power-up validation value 3

; Relocating Loader Miscellaneous Equates

DATAER  = 156           ;##rev2## end of record appears before END
MEMERR  = 157           ;##rev2## memory insufficient for load error

; Miscellaneous Equates

IOCFRE  = $FF           ;IOCB free indication

B19200  = $0028         ;##rev2## 19200 baud POKEY counter value
B00600  = $05CC         ;##rev2## 600 baud POKEY counter value

HITONE  = $05           ;##rev2## FSK high freq. POKEY counter value
LOTONE  = $07           ;##rev2## FSK low freq. POKEY counter value

NCOMLO  = $34           ;##rev2## PIA lower NOT COMMAND line command
NCOMHI  = $3C           ;##rev2## PIA raise NOT COMMAND line command

MOTRGO  = $34           ;##rev2## PIA cassette motor ON command
MOTRST  = $3C           ;##rev2## PIA cassette motor OFF command

NODAT   = $00           ;##rev2## SIO immediate operation
GETDAT  = $40           ;##rev2## SIO read data frame
PUTDAT  = $80           ;##rev2## SIO write data frame

CRETRI  = 13            ;##rev2## number of command frame retries
DRETRI  = 1             ;##rev2## number of device retries
CTIM    = 2             ;##rev2## command frame ACK timeout

NBUFSZ  = 40            ;##rev2## print normal buffer size
DBUFSZ  = 20            ;##rev2## print double buffer size
SBUFSZ  = 29            ;##rev2## print sideways buffer size

;-------------------------------------------------------------------------
; Page Zero Address Equates
;-------------------------------------------------------------------------

LINZBS  = $00           ;LINBUG RAM (WILL BE REPLACED BY MONITOR RAM)
LNFLG   = $00           ;##1200xl## 1-byte LNBUG flag (0 = not LNBUG)
NGFLAG  = $01           ;##1200xl## 1-byte memory status (0 = failure)

; Not Cleared

CASINI  = $02           ;CASSETTE INIT LOCATION
RAMLO   = $04           ;RAM POINTER FOR MEMORY TEST
TRAMSZ  = $06           ;TEMPORARY REGISTER FOR RAM SIZE
;TSTDAT = $07           ;##old## RAM TEST DATA REGISTER
CMCMD   = $07           ;##rev2## 1-byte command communications

; Cleared upon Coldstart only

WARMST  = $08           ;WARM START FLAG
BOOTQ   = $09           ;SUCCESSFUL BOOT FLAG
DOSVEC  = $0A           ;DISK SOFTWARE START VECTOR
DOSINI  = $0C           ;DISK SOFTWARE INIT ADDRESS
APPMHI  = $0E           ;APPLICATIONS MEMORY HI LIMIT

; Cleared upon Coldstart or Warmstart

INTZBS  = $10           ;INTERRUPT HANDLER

POKMSK  = $10           ;SYSTEM MASK FOR POKEY IRQ ENABLE (shadow of IRQEN)
BRKKEY  = $11           ;BREAK KEY FLAG
RTCLOK  = $12           ;REAL TIME CLOCK (IN 16 MSEC UNITS>
BUFADR  = $15           ;INDIRECT BUFFER ADDRESS REGISTER
ICCOMT  = $17           ;COMMAND FOR VECTOR
DSKFMS  = $18           ;DISK FILE MANAGER POINTER
DSKUTL  = $1A           ;DISK UTILITIES POINTER
ABUFPT  = $1C           ;##1200xl## 4-byte ACMI buffer pointer area

;PTIMOT = $1C           ;##old## PRINTER TIME OUT REGISTER
;PBPNT  = $1D           ;##old## PRINT BUFFER POINTER
;PBUFSZ = $1E           ;##old## PRINT BUFFER SIZE
;PTEMP  = $1F           ;##old## TEMPORARY REGISTER

ZIOCB   = $20           ;ZERO PAGE I/O CONTROL BLOCK
IOCBAS  = $20           ;16-byte page zero IOCB
ICHIDZ  = $20           ;HANDLER INDEX NUMBER (FF = IOCB FREE)
ICDNOZ  = $21           ;DEVICE NUMBER (DRIVE NUMBER)
ICCOMZ  = $22           ;COMMAND CODE
ICSTAZ  = $23           ;STATUS OF LAST IOCB ACTION
ICBALZ  = $24           ;BUFFER ADDRESS LOW BYTE
ICBAHZ  = $25           ;1-byte high buffer address
ICPTLZ  = $26           ;PUT BYTE ROUTINE ADDRESS -1
ICPTHZ  = $27           ;1-byte high PUT-BYTE routine address
ICBLLZ  = $28           ;BUFFER LENGTH LOW BYTE
ICBLHZ  = $29           ;1-byte high buffer length
ICAX1Z  = $2A           ;AUXILIARY INFORMATION FIRST BYTE
ICAX2Z  = $2B           ;1-byte second auxiliary information
ICSPRZ  = $2C           ;4-byte spares

ENTVEC  = $2C           ;##rev2## 2-byte (not used)
ICIDNO  = $2E           ;IOCB NUMBER X 16
CIOCHR  = $2F           ;CHARACTER BYTE FOR CURRENT OPERATION

STATUS  = $30           ;INTERNAL STATUS STORAGE
CHKSUM  = $31           ;CHECKSUM (SINGLE BYTE SUM WITH CARRY)
BUFRLO  = $32           ;POINTER TO DATA BUFFER (LO BYTE)
BUFRHI  = $33           ;POINTER TO DATA BUFFER (HI BYTE)
BFENLO  = $34           ;NEXT BYTE PAST END OF THE DATA BUFFER LO
BFENHI  = $35           ;NEXT BYTE PAST END OF THE DATA BUFFER HI
;CRETRY = $36           ;##old## NUMBER OF COMMAND FRAME RETRIES
;DRETRY = $37           ;##old## NUMBER OF DEVICE RETRIES
LTEMP   = $36           ;##1200xl## 2-byte loader temporary
BUFRFL  = $38           ;DATA BUFFER FULL FLAG
RECVDN  = $39           ;RECEIVE DONE FLAG
XMTDON  = $3A           ;TRANSMISSION DONE FLAG
CHKSNT  = $3B           ;CHECKSUM SENT FLAG
NOCKSM  = $3C           ;NO CHECKSUM FOLLOWS DATA FLAG
BPTR    = $3D           ;1-byte cassette buffer pointer
FTYPE   = $3E           ;1-byte cassette IRG type
FEOF    = $3F           ;1-byte cassette EOF flag (0 = quiet)
FREQ    = $40           ;1-byte cassette beep counter
SOUNDR  = $41           ;NOISY I/0 FLAG. (ZERO IS QUIET)

CRITIC  = $42           ;DEFINES CRITICAL SECTION (CRITICAL IF NON-Z)

FMSZPG  = $43           ;DISK FILE MANAGER SYSTEM ZERO PAGE

;CKEY   = $4A           ;##old## FLAG SET WHEN GAME START PRESSED
ZCHAIN  = $4A           ;##1200xl## 2-byte handler linkage chain pointer
;CASSBT = $4B           ;##old## CASSETTE BOOT FLAG
DSTAT   = $4C           ;DISPLAY STATUS
ATRACT  = $4D           ;ATRACT FLAG
DRKMSK  = $4E           ;DARK ATRACT MASK
COLRSH  = $4F           ;ATRACT COLOR SHIFTER (EOR'ED WITH PLAYFIELD


TMPCHR  = $50           ;1-byte temporary character
HOLD1   = $51           ;1-byte temporary
LMARGN  = $52           ;left margin (normally 2, cc65 C startup code sets it to 0)
RMARGN  = $53           ;right margin (normally 39 if no XEP80 is used)
ROWCRS  = $54           ;1-byte cursor row
COLCRS  = $55           ;2-byte cursor column
DINDEX  = $57           ;1-byte display mode
SAVMSC  = $58           ;2-byte saved memory scan counter
OLDROW  = $5A           ;1-byte prior row
OLDCOL  = $5B           ;2-byte prior column
OLDCHR  = $5D           ;DATA UNDER CURSOR
OLDADR  = $5E           ;2-byte saved cursor memory address
FKDEF   = $60           ;##1200xl## 2-byte function key definition table
;NEWROW = $60           ;##old## POINT DRAW GOES TO
;NEWCOL = $61           ;##old##
PALNTS  = $62           ;##1200xl## 1-byte PAL/NTSC indicator (0 = NTSC)
LOGCOL  = $63           ;POINTS AT COLUMN IN LOGICAL LINE
ADRESS  = $64           ;2-byte temporary address

MLTTMP  = $66           ;1-byte temporary
OPNTMP  = $66           ;FIRST BYTE IS USED IN OPEN AS TEMP
TOADR   = $66           ;##rev2## 2-byte destination address

SAVADR  = $68           ;2-byte saved address
FRMADR  = $68           ;##rev2## 2-byte source address

RAMTOP  = $6A           ;RAM SIZE DEFINED BY POWER ON LOGIC
BUFCNT  = $6B           ;BUFFER COUNT
BUFSTR  = $6C           ;EDITOR GETCH POINTER
BITMSK  = $6E           ;BIT MASK
SHFAMT  = $6F           ;1-byte shift amount for pixel justifucation
ROWAC   = $70           ;2-byte draw working row
COLAC   = $72           ;2-byte draw working column
ENDPT   = $74           ;2-byte end point
DELTAR  = $76           ;1-byte row difference
DELTAC  = $77           ;2-byte column difference
KEYDEF  = $79           ;##1200xl## 2-byte key definition table address
;ROWINC = $79           ;##old##
;COLINC = $7A           ;##old##
SWPFLG  = $7B           ;NON-0 1F TXT AND REGULAR RAM IS SWAPPED
HOLDCH  = $7C           ;CH IS MOVED HERE IN KGETCH BEFORE CNTL & SH
INSDAT  = $7D           ;1-byte temporary
COUNTR  = $7E           ;2-byte draw iteration count

; Floating Point Package Page Zero Address Equates

FR0     = $D4           ;6-byte register 0
FR0M    = $D5           ;##rev2## 5-byte register 0 mantissa
QTEMP   = $D9           ;##rev2## 1-byte temporary

FRE     = $DA           ;6-byte (internal) register E

FR1     = $E0           ;FP REG1
FR1M    = $E1           ;##rev2## 5-byte register 1 mantissa

FR2     = $E6           ;6-byte (internal) register 2

FRX     = $EC           ;1-byte temporary

EEXP    = $ED           ;VALUE OF E

FRSIGN  = $EE           ;##rev2## 1-byte floating point sign
NSIGN   = $EE           ;SIGN OF #

PLYCNT  = $EF           ;##rev2## 1-byte polynomial degree
ESIGN   = $EF           ;SIGN OF EXPONENT

SGNFLG  = $F0           ;##rev2## 1-byte sign flag
FCHRFLG = $F0           ;1ST CHAR FLAG

XFMFLG  = $F1           ;##rev2## 1-byte transform flag
DIGRT   = $F1           ;# OF DIGITS RIGHT OF DECIMAL

CIX     = $F2           ;CURRENT INPUT INDEX
INBUFF  = $F3           ;POINTS TO USER'S LINE INPUT BUFFER

ZTEMP1  = $F5           ;2-byte temporary
ZTEMP4  = $F7           ;2-byte temporary
ZTEMP3  = $F9           ;2-byte temporary

;DEGFLG = $FB           ;##old## same as RADFLG
;RADFLG = $FB           ;##old## 0=RADIANS, 6=DEGREES

FLPTR   = $FC           ;2-byte floating point number pointer
FPTR2   = $FE           ;2-byte floating point number pointer

;-------------------------------------------------------------------------
; Page Two Address Equates
;-------------------------------------------------------------------------

INTABS  = $0200         ;INTERRUPT RAM

VDSLST  = $0200         ;DISPLAY LIST NMI VECTOR
VPRCED  = $0202         ;PROCEED LINE IRQ VECTOR
VINTER  = $0204         ;INTERRUPT LINE IRQ VECTOR
VBREAK  = $0206         ;SOFTWARE BREAK (00) INSTRUCTION IRQ VECTOR
VKEYBD  = $0208         ;POKEY KEYBOARD IRQ VECTOR
VSERIN  = $020A         ;POKEY SERIAL INPUT READY IRQ
VSEROR  = $020C         ;POKEY SERIAL OUTPUT READY IRQ
VSEROC  = $020E         ;POKEY SERIAL OUTPUT COMPLETE IRQ
VTIMR1  = $0210         ;POKEY TIMER 1 IRQ
VTIMR2  = $0212         ;POKEY TIMER 2 IRQ
VTIMR4  = $0214         ;POKEY TIMER 4 IRQ
VIMIRQ  = $0216         ;IMMEDIATE IRQ VECTOR
CDTMV1  = $0218         ;COUNT DOWN TIMER 1
CDTMV2  = $021A         ;COUNT DOWN TIMER 2
CDTMV3  = $021C         ;COUNT DOWN TIMER 3
CDTMV4  = $021E         ;COUNT DOWN TIMER 4
CDTMV5  = $0220         ;COUNT DOWN TIMER 5
VVBLKI  = $0222         ;IMMEDIATE VERTICAL BLANK NMI VECTOR
VVBLKD  = $0224         ;DEFERRED VERTICAL BLANK NMI VECTOR
CDTMA1  = $0226         ;COUNT DOWN TIMER 1 JSR ADDRESS
CDTMA2  = $0228         ;COUNT DOWN TIMER 2 JSR ADDRESS
CDTMF3  = $022A         ;COUNT DOWN TIMER 3 FLAG
SRTIMR  = $022B         ;SOFTWARE REPEAT TIMER
CDTMF4  = $022C         ;COUNT DOWN TIMER 4 FLAG
INTEMP  = $022D         ;IAN'S TEMP
CDTMF5  = $022E         ;COUNT DOWN TIMER FLAG 5
SDMCTL  = $022F         ;SAVE DMACTL REGISTER
SDLSTL  = $0230         ;SAVE DISPLAY LIST LOW BYTE
SDLSTH  = $0231         ;SAVE DISPLAY LIST HI BYTE
SSKCTL  = $0232         ;SKCTL REGISTER RAM
LCOUNT  = $0233         ;##1200xl## 1-byte relocating loader record
LPENH   = $0234         ;LIGHT PEN HORIZONTAL VALUE
LPENV   = $0235         ;LIGHT PEN VERTICAL VALUE
BRKKY   = $0236         ;BREAK KEY VECTOR
;RELADR = $0238         ;##1200xl## 2-byte relocatable loader address
VPIRQ   = $0238         ;##rev2## 2-byte parallel device IRQ vector
CDEVIC  = $023A         ;COMMAND FRAME BUFFER - DEVICE
CCOMND  = $023B         ;COMMAND
CAUX1   = $023C         ;COMMAND AUX BYTE 1
CAUX2   = $023D         ;COMMAND AUX BYTE 2

TEMP    = $023E         ;TEMPORARY RAM CELL

ERRFLG  = $023F         ;ERROR FLAG - ANY DEVICE ERROR EXCEPT TIME OUT

DFLAGS  = $0240         ;DISK FLAGS FROM SECTOR ONE
DBSECT  = $0241         ;NUMBER OF DISK BOOT SECTORS
BOOTAD  = $0242         ;ADDRESS WHERE DISK BOOT LOADER WILL BE PUT
COLDST  = $0244         ;COLDSTART FLAG (1=IN MIDDLE OF COLDSTART>
RECLEN  = $0245         ;##1200xl## 1-byte relocating loader record length
DSKTIM  = $0246         ;DISK TIME OUT REGISTER
;LINBUF = $0247         ;##old## CHAR LINE BUFFER
PDVMSK  = $0247         ;##rev2## 1-byte parallel device selection mask
SHPDVS  = $0248         ;##rev2## 1-byte PDVS (parallel device select)
PDIMSK  = $0249         ;##rev2## 1-byte parallel device IRQ selection
RELADR  = $024A         ;##rev2## 2-byte relocating loader relative adr.
PPTMPA  = $024C         ;##rev2## 1-byte parallel device handler temporary
PPTMPX  = $024D         ;##rev2## 1-byte parallel device handler temporary

CHSALT  = $026B         ;##1200xl## 1-byte character set alternate
VSFLAG  = $026C         ;##1200xl## 1-byte fine vertical scroll count
KEYDIS  = $026D         ;##1200xl## 1-byte keyboard disable
FINE    = $026E         ;##1200xl## 1-byte fine scrolling mode
GPRIOR  = $026F         ;GLOBAL PRIORITY CELL

PADDL0  = $0270         ;1-byte potentiometer 0
PADDL1  = $0271         ;1-byte potentiometer 1
PADDL2  = $0272         ;1-byte potentiometer 2
PADDL3  = $0273         ;1-byte potentiometer 3
PADDL4  = $0274         ;1-byte potentiometer 4
PADDL5  = $0275         ;1-byte potentiometer 5
PADDL6  = $0276         ;1-byte potentiometer 6
PADDL7  = $0277         ;1-byte potentiometer 7

STICK0  = $0278         ;1-byte joystick 0
STICK1  = $0279         ;1-byte joystick 1
STICK2  = $027A         ;1-byte joystick 2
STICK3  = $027B         ;1-byte joystick 3

PTRIG0  = $027C         ;1-byte paddle trigger 0
PTRIG1  = $027D         ;1-byte paddle trigger 1
PTRIG2  = $027E         ;1-byte paddle trigger 2
PTRIG3  = $027F         ;1-byte paddle trigger 3
PTRIG4  = $0280         ;1-byte paddle trigger 4
PTRIG5  = $0281         ;1-byte paddle trigger 5
PTRIG6  = $0281         ;1-byte paddle trigger 6
PTRIG7  = $0283         ;1-byte paddle trigger 7

STRIG0  = $0284         ;1-byte joystick trigger 0
STRIG1  = $0285         ;1-byte joystick trigger 1
STRIG2  = $0286         ;1-byte joystick trigger 2
STRIG3  = $0287         ;1-byte joystick trigger 3

;CSTAT  = $0288         ;##old## cassette status register
HIBYTE  = $0288         ;##1200xl## 1-byte relocating loader high byte
WMODE   = $0289         ;1-byte cassette WRITE mode
BLIM    = $028A         ;1-byte cassette buffer limit
IMASK   = $028B         ;##rev2## (not used)
JVECK   = $028C         ;2-byte jump vector or temporary
NEWADR  = $028E         ;##1200xl## 2-byte relocating address
TXTROW  = $0290         ;TEXT ROWCRS
TXTCOL  = $0291         ;TEXT COLCRS
TINDEX  = $0293         ;TEXT INDEX
TXTMSC  = $0294         ;FOOLS CONVRT INTO NEW MSC
TXTOLD  = $0296         ;OLDROW & OLDCOL FOR TEXT (AND THEN SOME)
;TMPX1  = $029C         ;##old## 1-byte temporary register
CRETRY  = $029C         ;##1200xl## 1-byte number of command frame retries
HOLD3   = $029D         ;1-byte temporary
SUBTMP  = $029E         ;1-byte temporary
HOLD2   = $029F         ;1-byte (not used)
DMASK   = $02A0         ;1-byte display (pixel location) mask
TMPLBT  = $02A1         ;1-byte (not used)
ESCFLG  = $02A2         ;ESCAPE FLAG
TABMAP  = $02A3         ;15-byte (120 bit) tab stop bit map
LOGMAP  = $02B2         ;LOGICAL LINE START BIT MAP
INVFLG  = $02B6         ;INVERSE VIDEO FLAG (TOGGLED BY ATARI KEY)
FILFLG  = $02B7         ;RIGHT FILL FLAG FOR DRAW
TMPROW  = $02B8         ;1-byte temporary row
TMPCOL  = $02B9         ;2-byte temporary column
SCRFLG  = $02BB         ;SET IF SCROLL OCCURS
HOLD4   = $02BC         ;TEMP CELL USED IN DRAW ONLY
;HOLD5  = $02BD         ;##old## DITTO
DRETRY  = $02BD         ;##1200xl## 1-byte number of device retries
SHFLOK  = $02BE         ;1-byte shift/control lock flags
BOTSCR  = $02BF         ;BOTTOM OF SCREEN   24 NORM 4 SPLIT

PCOLR0  = $02C0         ;1-byte player-missile 0 color/luminance
PCOLR1  = $02C1         ;1-byte player-missile 1 color/luminance
PCOLR2  = $02C2         ;1-byte player-missile 2 color/luminance
PCOLR3  = $02C3         ;1-byte player-missile 3 color/luminance

COLOR0  = $02C4         ;1-byte playfield 0 color/luminance
COLOR1  = $02C5         ;1-byte playfield 1 color/luminance
COLOR2  = $02C6         ;1-byte playfield 2 color/luminance
COLOR3  = $02C7         ;1-byte playfield 3 color/luminance

COLOR4  = $02C8         ;1-byte background color/luminance

PARMBL  = $02C9         ;##rev2## 6-byte relocating loader parameter
RUNADR  = $02C9         ;##1200xl## 2-byte run address
HIUSED  = $02CB         ;##1200xl## 2-byte highest non-zero page address
ZHIUSE  = $02CD         ;##1200xl## 2-byte highest zero page address

OLDPAR  = $02CF         ;##rev2## 6-byte relocating loader parameter
GBYTEA  = $02CF         ;##1200xl## 2-byte GET-BYTE routine address
LOADAD  = $02D1         ;##1200xl## 2-byte non-zero page load address
ZLOADA  = $02D3         ;##1200xl## 2-byte zero page load address

DSCTLN  = $02D5         ;##1200xl## 2-byte disk sector length
ACMISR  = $02D7         ;##1200xl## 2-byte ACMI interrupt service routine
KRPDEL  = $02D9         ;##1200xl## 1-byte auto-repeat delay
KEYREP  = $02DA         ;##1200xl## 1-byte auto-repeat rate
NOCLIK  = $02DB         ;##1200xl## 1-byte key click disable
HELPFG  = $02DC         ;##1200xl## 1-byte HELP key flag (0 = no HELP)
DMASAV  = $02DD         ;##1200xl## 1-byte SDMCTL save/restore
PBPNT   = $02DE         ;##1200xl## 1-byte printer buffer pointer
PBUFSZ  = $02DF         ;##1200xl## 1-byte printer buffer size

GLBABS  = $02E0         ;4-byte global variables for non-DOS users
RUNAD   = $02E0         ;##map## 2-byte binary file run address
INITAD  = $02E2         ;##map## 2-byte binary file initialization address

RAMSIZ  = $02E4         ;RAM SIZE (HI BYTE ONLY)
MEMTOP  = $02E5         ;TOP OF AVAILABLE USER MEMORY
MEMLO   = $02E7         ;BOTTOM OF AVAILABLE USER MEMORY
HNDLOD  = $02E9         ;##1200xl## 1-byte user load flag
DVSTAT  = $02EA         ;STATUS BUFFER
CBAUDL  = $02EE         ;1-byte low cassette baud rate
CBAUDH  = $02EF         ;1-byte high cassette baud rate
CRSINH  = $02F0         ;CURSOR INHIBIT (00 = CURSOR ON)
KEYDEL  = $02F1         ;KEY DELAY
CH1     = $02F2         ;1-byte prior keyboard character
CHACT   = $02F3         ;CHACTL REGISTER RAM
CHBAS   = $02F4         ;CHBAS REGISTER RAM

NEWROW  = $02F5         ;##1200xl## 1-byte draw destination row
NEWCOL  = $02F6         ;##1200xl## 2-byte draw destination column
ROWINC  = $02F8         ;##1200xl## 1-byte draw row increment
COLINC  = $02F9         ;##1200xl## 1-byte draw column increment

CHAR    = $02FA         ;1-byte internal character
ATACHR  = $02FB         ;ATASCII CHARACTER
CH      = $02FC         ;GLOBAL VARIABLE FOR KEYBOARD
FILDAT  = $02FD         ;RIGHT FILL DATA <DRAW>
DSPFLG  = $02FE         ;DISPLAY FLAG   DISPLAY CNTLS IF NON-ZERO
SSFLAG  = $02FF         ;START/STOP FLAG FOR PAGING (CNTL 1). CLEARE

;-------------------------------------------------------------------------
; Page Three Address Equates
;-------------------------------------------------------------------------

DCB     = $0300         ;DEVICE CONTROL BLOCK
DDEVIC  = $0300         ;PERIPHERAL UNIT 1 BUS I.D. NUMBER
DUNIT   = $0301         ;UNIT NUMBER
DCOMND  = $0302         ;BUS COMMAND
DSTATS  = $0303         ;COMMAND TYPE/STATUS RETURN
DBUFLO  = $0304         ;1-byte low data buffer address
DBUFHI  = $0305         ;1-byte high data buffer address
DTIMLO  = $0306         ;DEVICE TIME OUT IN 1 SECOND UNITS
DUNUSE  = $0307         ;UNUSED BYTE
DBYTLO  = $0308         ;1-byte low number of bytes to transfer
DBYTHI  = $0309         ;1-byte high number of bytes to transfer
DAUX1   = $030A         ;1-byte first command auxiliary
DAUX2   = $030B         ;1-byte second command auxiliary

TIMER1  = $030C         ;INITIAL TIMER VALUE
;ADDCOR = $030E         ;##old## ADDITION CORRECTION
JMPERS  = $030E         ;##1200xl## 1-byte jumper options
CASFLG  = $030F         ;CASSETTE MODE WHEN SET
TIMER2  = $0310         ;2-byte final baud rate timer value
TEMP1   = $0312         ;TEMPORARY STORAGE REGISTER
;TEMP2  = $0314         ;##old## TEMPORARY STORAGE REGISTER
TEMP2   = $0313         ;##1200xl## 1-byte temporary
PTIMOT  = $0314         ;##1200xl## 1-byte printer timeout
TEMP3   = $0315         ;TEMPORARY STORAGE REGISTER
SAVIO   = $0316         ;SAVE SERIAL IN DATA PORT
TIMFLG  = $0317         ;TIME OUT FLAG FOR BAUD RATE CORRECTION
STACKP  = $0318         ;SIO STACK POINTER SAVE CELL
TSTAT   = $0319         ;TEMPORARY STATUS HOLDER

HATABS  = $031A         ;35-byte handler address table (was 38 bytes)
PUPBT1  = $033D         ;##1200xl## 1-byte power-up validation byte 1
PUPBT2  = $033E         ;##1200xl## 1-byte power-up validation byte 2
PUPBT3  = $033F         ;##1200xl## 1-byte power-up validation byte 3

IOCB    = $0340         ;I/O CONTROL BLOCKS
ICHID   = $0340         ;HANDLER INDEX NUMBER (FF=IOCB FREE)
ICDNO   = $0341         ;DEVICE NUMBER (DRIVE NUMBER)
ICCOM   = $0342         ;COMMAND CODE
ICSTA   = $0343         ;STATUS OF LAST IOCB ACTION
ICBAL   = $0344         ;1-byte low buffer address
ICBAH   = $0345         ;1-byte high buffer address
ICPTL   = $0346         ;1-byte low PUT-BYTE routine address - 1
ICPTH   = $0347         ;1-byte high PUT-BYTE routine address - 1
ICBLL   = $0348         ;1-byte low buffer length
ICBLH   = $0349         ;1-byte high buffer length
ICAX1   = $034A         ;1-byte first auxiliary information
ICAX2   = $034B         ;1-byte second auxiliary information
ICAX3   = $034C         ;1-byte third auxiliary information
ICAX4   = $034D         ;1-byte fourth auxiliary information
ICAX5   = $034E         ;1-byte fifth auxiliary information
ICSPR   = $034F         ;SPARE BYTE

PRNBUF  = $03C0         ;PRINTER BUFFER
SUPERF  = $03E8         ;##1200xl## 1-byte editor super function flag
CKEY    = $03E9         ;##1200xl## 1-byte cassette boot request flag
CASSBT  = $03EA         ;##1200xl## 1-byte cassette boot flag
CARTCK  = $03EB         ;##1200xl## 1-byte cartridge equivalence check
DERRF   = $03EC         ;##rev2## 1-byte screen OPEN error flag

; Remainder of Page Three Not Cleared upon Reset

ACMVAR  = $03ED         ;##1200xl## 11 bytes reserved for ACMI
BASICF  = $03F8         ;##rev2## 1-byte BASIC switch flag
MINTLK  = $03F9         ;##1200xl## 1-byte ACMI module interlock
GINTLK  = $03FA         ;##1200xl## 1-byte cartridge interlock
CHLINK  = $03FB         ;##1200xl## 2-byte loaded handler chain link
CASBUF  = $03FD         ;CASSETTE BUFFER

;-------------------------------------------------------------------------
; Page Four/Five Address Equates
;-------------------------------------------------------------------------

; USER AREA STARTS HERE AND GOES TO END OF PAGE FIVE
USAREA  = $0480         ;128 bytes reserved for application

LBPR1   = $057E         ;LBUFF PREFIX 1
LBPR2   = $057F         ;LBUFF PREFIX 2
LBUFF   = $0580         ;128-byte line buffer

PLYARG  = $05E0         ;6-byte floating point polynomial argument
FPSCR   = $05E6         ;6-byte floating point temporary
FPSCR1  = $05EC         ;6-byte floating point temporary

;LBFEND = $05FF         ;##old## END OF LBUFF


DOS     = $0700

;-------------------------------------------------------------------------
; SpartaDOS-X Definitions
;-------------------------------------------------------------------------

SDX_FLAG     = DOS              ; 'S' for SpartaDOS
SDX_VERSION  = $0701            ; SD version (e.g. $32 = 3.2, $40 = 4.0)
                                ; address $0702 contains sub-version, e.g.
                                ; 8 in case of SDX 4.48
SDX_KERNEL   = $0703            ; SDX kernel entry point
SDX_BLOCK_IO = $0706            ; block I/O entry point
SDX_MISC     = $0709            ; "misc" entry point
SDX_DEVICE   = $0761
SDX_DATE     = $077B            ; day, month, year (3 bytes)
SDX_TIME     = $077E            ; hour, min, sec (3 bytes)
SDX_DATESET  = $0781
SDX_PATH     = $07A0            ; 64 bytes
SDX_IFSYMBOL = $07EB            ; only valid on SDX 4.40 or newer
SDX_S_LOOKUP = SDX_IFSYMBOL     ; alternative name for SDX_IFSYMBOL

; values for SDX_DEVICE

SDX_CLK_DEV  = $10              ; clock device

; clock device functions

SDX_KD_GETTD = 100              ; get time and date
SDX_KD_SETTD = 101              ; set time and date

;-------------------------------------------------------------------------
; Cartridge Address Equates
;-------------------------------------------------------------------------

CARTCS  = $BFFA         ;##rev2## 2-byte cartridge coldstart address
CART    = $BFFC         ;##rev2## 1-byte cartridge present indicator
                        ;0=Cart Exists
CARTFG  = $BFFD         ;##rev2## 1-byte cartridge flags
                        ;D7  0=Not a Diagnostic Cart
                        ;    1=Is a Diagnostic cart and control is
                        ;      given to cart before any OS is init.
                        ;D2  0=Init but Do not Start Cart
                        ;    1=Init and Start Cart
                        ;D0  0=Do not boot disk
                        ;    1=Boot Disk
CARTAD  = $BFFE         ;##rev2## 2-byte cartridge start vector

;-------------------------------------------------------------------------
; CTIA/GTIA Address Equates
;-------------------------------------------------------------------------

GTIA    = $D000         ;CTIA/GTIA area
.include "atari_gtia.inc"

;-------------------------------------------------------------------------
; PBI Address Equates
;-------------------------------------------------------------------------

PBI     = $D100         ;##rev2## parallel bus interface area

; Read Addresses

PDVI    = $D1FF         ;##rev2## parallel device IRQ status

; Write Addresses

PDVS    = $D1FF         ;##rev2## parallel device select

;-------------------------------------------------------------------------
; POKEY Address Equates
;-------------------------------------------------------------------------

POKEY     = $D200         ;POKEY area
.include  "atari_pokey.inc"

; POKEY KBCODE Values

KEY_NONE    = $FF

KEY_0       = $32
KEY_1       = $1F
KEY_2       = $1E
KEY_3       = $1A
KEY_4       = $18
KEY_5       = $1D
KEY_6       = $1B
KEY_7       = $33
KEY_8       = $35
KEY_9       = $30

KEY_A       = $3F
KEY_B       = $15
KEY_C       = $12
KEY_D       = $3A
KEY_E       = $2A
KEY_F       = $38
KEY_G       = $3D
KEY_H       = $39
KEY_I       = $0D
KEY_J       = $01
KEY_K       = $05
KEY_L       = $00
KEY_M       = $25
KEY_N       = $23
KEY_O       = $08
KEY_P       = $0A
KEY_Q       = $2F
KEY_R       = $28
KEY_S       = $3E
KEY_T       = $2D
KEY_U       = $0B
KEY_V       = $10
KEY_W       = $2E
KEY_X       = $16
KEY_Y       = $2B
KEY_Z       = $17

KEY_COMMA       = $20
KEY_PERIOD      = $22
KEY_SLASH       = $26
KEY_SEMICOLON   = $02
KEY_PLUS        = $06
KEY_ASTERISK    = $07
KEY_DASH        = $0E
KEY_EQUALS      = $0F
KEY_LESSTHAN    = $36
KEY_GREATERTHAN = $37

KEY_ESC     = $1C
KEY_TAB     = $2C
KEY_SPACE   = $21
KEY_RETURN  = $0C
KEY_DELETE  = $34
KEY_CAPS    = $3C
KEY_INVERSE = $27
KEY_HELP    = $11

KEY_F1      = $03
KEY_F2      = $04
KEY_F3      = $13
KEY_F4      = $14

KEY_SHIFT   = $40
KEY_CTRL    = $80

; Composed keys

KEY_EXCLAMATIONMARK = KEY_1 | KEY_SHIFT
KEY_QUOTE           = KEY_2 | KEY_SHIFT
KEY_HASH            = KEY_3 | KEY_SHIFT
KEY_DOLLAR          = KEY_4 | KEY_SHIFT
KEY_PERCENT         = KEY_5 | KEY_SHIFT
KEY_AMPERSAND       = KEY_6 | KEY_SHIFT
KEY_APOSTROPHE      = KEY_7 | KEY_SHIFT
KEY_AT              = KEY_8 | KEY_SHIFT
KEY_OPENINGPARAN    = KEY_9 | KEY_SHIFT
KEY_CLOSINGPARAN    = KEY_0 | KEY_SHIFT
KEY_UNDERLINE       = KEY_DASH | KEY_SHIFT
KEY_BAR             = KEY_EQUALS | KEY_SHIFT
KEY_COLON           = KEY_SEMICOLON | KEY_SHIFT
KEY_BACKSLASH       = KEY_PLUS | KEY_SHIFT
KEY_CIRCUMFLEX      = KEY_ASTERISK | KEY_SHIFT
KEY_OPENINGBRACKET  = KEY_COMMA | KEY_SHIFT
KEY_CLOSINGBRACKET  = KEY_PERIOD | KEY_SHIFT
KEY_QUESTIONMARK    = KEY_SLASH | KEY_SHIFT
KEY_CLEAR           = KEY_LESSTHAN | KEY_SHIFT
KEY_INSERT          = KEY_GREATERTHAN | KEY_SHIFT

KEY_UP              = KEY_DASH | KEY_CTRL
KEY_DOWN            = KEY_EQUALS | KEY_CTRL
KEY_LEFT            = KEY_PLUS | KEY_CTRL
KEY_RIGHT           = KEY_ASTERISK | KEY_CTRL

;-------------------------------------------------------------------------
; ANTIC Address Equates
;-------------------------------------------------------------------------

ANTIC     = $D400         ;ANTIC area
.include  "atari_antic.inc"

; PBI RAM Address Equates

PBIRAM  = $D600         ;##rev2## parallel bus interface RAM area

; Parallel Device Address Equates

PDID1   = $D803         ;##rev2## parallel device ID 1
PDIDV   = $D805         ;##rev2## parallel device I/O vector
PDIRQV  = $D808         ;##rev2## parallel device IRQ vector
PDID2   = $D80B         ;##rev2## parallel device ID 2
PDVV    = $D80D         ;##rev2## parallel device vector table

;-------------------------------------------------------------------------
; PIA Address Equates
;-------------------------------------------------------------------------

PIA     = $D300         ;PIA area

PORTA   = $D300         ;port A direction register or jacks one/two
PORTB   = $D301         ;port B direction register or memory management

PACTL   = $D302         ;port A control
PBCTL   = $D303         ;port B control

;-------------------------------------------------------------------------
; Floating Point Package Address Equates
;-------------------------------------------------------------------------

AFP     = $D800         ;convert ASCII to floating point
FASC    = $D8E6         ;convert floating point to ASCII
IFP     = $D9AA         ;convert integer to floating point
FPI     = $D9D2         ;convert floating point to integer
ZFR0    = $DA44         ;zero FR0
ZF1     = $DA46         ;zero floating point number
FSUB    = $DA60         ;subtract floating point numbers
FADD    = $DA66         ;add floating point numbers
FMUL    = $DADB         ;multiply floating point numbers
FDIV    = $DB28         ;divide floating point numbers
PLYEVL  = $DD40         ;evaluate floating point polynomial
FLD0R   = $DD89         ;load floating point number
FLD0P   = $DD8D         ;load floating point number
FLD1R   = $DD98         ;load floating point number
PLD1P   = $DD9C         ;load floating point number
FST0R   = $DDA7         ;store floating point number
FST0P   = $DDAB         ;store floating point number
FMOVE   = $DDB6         ;move floating point number
LOG     = $DECD         ;calculate floating point logarithm
LOG10   = $DED1         ;calculate floating point base 10 logarithm
EXP     = $DDC0         ;calculate floating point exponential
EXP10   = $DDCC         ;calculate floating point base 10 exponential

;-------------------------------------------------------------------------
; Device Handler Vector Table Address Equates
;-------------------------------------------------------------------------

EDITRV  = $E400         ;editor handler vector table
SCRENV  = $E410         ;screen handler vector table
KEYBDV  = $E420         ;keyboard handler vector table
PRINTV  = $E430         ;printer handler vector table
CASETV  = $E440         ;cassette handler vector table

;-------------------------------------------------------------------------
; Jump Vector Address Equates
;-------------------------------------------------------------------------

DISKIV  = $E450         ;vector to initialize DIO
DSKINV  = $E453         ;vector to DIO
.ifdef __ATARIXL__
.ifndef SHRAM_HANDLERS
.import CIO_handler, SIO_handler, SETVBV_handler
.endif
.define CIOV    CIO_handler
.define SIOV    SIO_handler
.define SETVBV  SETVBV_handler
CIOV_org    = $E456     ;vector to CIO
SIOV_org    = $E459     ;vector to SIO
SETVBV_org  = $E45C     ;vector to set VBLANK parameters
.else
CIOV    = $E456         ;vector to CIO
SIOV    = $E459         ;vector to SIO
SETVBV  = $E45C         ;vector to set VBLANK parameters
; aliases in order not to have to sprinkle common code with .ifdefs
CIOV_org    = CIOV
SIOV_org    = SIOV
SETVBV_org  = SETVBV
.endif
SYSVBV  = $E45F         ;vector to process immediate VBLANK
XITVBV  = $E462         ;vector to process deferred VBLANK
SIOINV  = $E465         ;vector to initialize SIO
SENDEV  = $E468         ;vector to enable SEND
INTINV  = $E46B         ;vector to initialize interrupt handler
CIOINV  = $E46E         ;vector to initialize CIO
BLKBDV  = $E471         ;vector to power-up display
WARMSV  = $E474         ;vector to warmstart
COLDSV  = $E477         ;vector to coldstart
RBLOKV  = $E47A         ;vector to read cassette block
CSOPIV  = $E47D         ;vector to open cassette for input
VCTABL  = $E480         ;RAM vector initial value table
PUPDIV  = $E480         ;##rev2## vector to power-up display
SLFTSV  = $E483         ;##rev2## vector to self-test
PHENTV  = $E486         ;##rev2## vector to enter peripheral handler
PHUNLV  = $E489         ;##rev2## vector to unlink peripheral handler
PHINIV  = $E48C         ;##rev2## vector to initialize peripheral handler
GPDVV   = $E48F         ;##rev2## generic parallel device handler vector

; NOTE: OS rom self-test labels are not included in this file

;-------------------------------------------------------------------------
; Some misc. stuff from the 400/800 rev.B source
;-------------------------------------------------------------------------

; THE FOLLOWING ARE IN BASIC CARTRIDGE:
SIN     = $BD81         ;FR0 <- SIN (FR0) DEGFLG (0=RAD,6=DEG) CARRY
COS     = $BD73         ;FR0 <- COS (FR0) CARRY
ATAN    = $BE43         ;FR0 <- ATAN(FR0) CARRY
SQR     = $BEB1         ;FR0 <- ROOT(FR0) CARRY

RADON   = 0             ;INDICATES RADIANS
DEGON   = 6             ;INDICATES DEGREES

ASCZER  = '0'           ;ASCII ZERO
COLON   = $3A           ;ASCII COLON
CR      = $9B           ;SYSTEM EOL (CARRIAGE RETURN)

;-------------------------------------------------------------------------
; 6502
;-------------------------------------------------------------------------

NMIVEC  = $FFFA
RESVEC  = $FFFC
IRQVEC  = $FFFE

;-------------------------------------------------------------------------
; BASIC
;-------------------------------------------------------------------------

LOMEM   = $80           ;2-byte low memory pointer
VNTP    = $82           ;2-byte variable name table address
VNTD    = $84           ;2-byte variable name table end + 1
VVTP    = $86           ;2-byte variable value table
STMTAB  = $88           ;2-byte statement table address
STMCUR  = $8A           ;2-byte current statement pointer
STARP   = $8C           ;2-byte string and array table pointer
RUNSTK  = $8E           ;2-byte runtime stack address
BMEMTOP = $90           ;2-byte top of memory pointer
STOPLN  = $BA           ;2-byte stopped line number
ERRSAVE = $C3           ;1-byte error code
PTABW   = $C9           ;1-byte tab width

;-------------------------------------------------------------------------
; ATASCII CHARACTER DEFS
;-------------------------------------------------------------------------

ATCLR   = $7D           ;CLEAR SCREEN CHARACTER
ATRUB   = $7E           ;BACK SPACE (RUBOUT)
ATTAB   = $7F           ;TAB
ATEOL   = $9B           ;END-OF-LINE
ATDELL  = $9C           ;delete line
ATINSL  = $9D           ;insert line
ATCTAB  = $9E           ;clear TAB
ATSTAB  = $9F           ;set TAB
ATBEL   = $FD           ;CONSOLE BELL
ATDEL   = $FE           ;delete char.
ATINS   = $FF           ;insert char.
ATURW   = $1C           ;UP-ARROW
ATDRW   = $1D           ;DOWN-ARROW
ATLRW   = $1E           ;LEFT-ARROW
ATRRW   = $1F           ;RIGHT-ARROW
ATESC   = $1B           ;ESCAPE

;-------------------------------------------------------------------------
; OFFSETS INTO SECTSIZETAB (DIO functions)
;-------------------------------------------------------------------------

sst_flag     = 0        ; length 1
sst_sectsize = 1        ;        2
sst_driveno  = 3        ;        1  (drive #)
sst_size     = 4        ; size of one entry
                        ; if changed, adapt diopncls.s

;-------------------------------------------------------------------------
; OFFSETS INTO dio_phys_pos
;-------------------------------------------------------------------------

diopp_head   = 0        ; head
diopp_track  = 1        ; track / cylinder
diopp_sector = 3        ; sector
diopp_size   = 5        ; size of structure

;-------------------------------------------------------------------------
; VALUES for dos_type
;-------------------------------------------------------------------------

SPARTADOS    = 0
REALDOS      = 1
BWDOS        = 2
OSADOS       = 3        ; OS/A+
XDOS         = 4
ATARIDOS     = 5
MYDOS        = 6
NODOS        = 255
; The DOSes with dos_type below or equal MAX_DOS_WITH_CMDLINE do support
; command line arguments.
MAX_DOS_WITH_CMDLINE = XDOS

;-------------------------------------------------------------------------
; XDOS defines (version 2.4, taken from xdos24.pdf)
;-------------------------------------------------------------------------

XOPT         = $070B    ; XDOS options
XCAR         = $070C    ; XDOS cartridge address (+ $70D)
XPAT         = $086F    ; XDOS bugfix and patch number
XVER         = $0870    ; XDOS version number
XFILE        = $087D    ; XDOS filename buffer
XLINE        = $0880    ; XDOS DUP input line
XGLIN        = $0871    ; get line
XSKIP        = $0874    ; skip parameter
.ifdef __ATARIXL__
.ifndef SHRAM_HANDLERS
.import XMOVE_handler
.endif
.define XMOVE XMOVE_handler
XMOVE_org    = $0877    ; move filename
.else
XMOVE        = $0877    ; move filename
.endif
XGNUM        = $087A    ; get number

;-------------------------------------------------------------------------
; End of atari.inc
;-------------------------------------------------------------------------
