;
;GeosConst - various system constans sorted by function
;reassembled by Maciej 'YTM/Elysium' Witkowiak
;4-2-99, 18-3-99

NULL                    =       0
FALSE                   =       NULL
TRUE                    =       $ff

MOUSE_SPRNUM            =       0

DISK_DRV_LGH            =       $0d80

;
;filetypes
;       GEOS
NOT_GEOS                =       0
BASIC                   =       1
ASSEMBLY                =       2
DATA                    =       3
SYSTEM                  =       4
DESK_ACC                =       5
APPLICATION             =       6
APPL_DATA               =       7
FONT                    =       8
PRINTER                 =       9
INPUT_DEVICE            =       10
DISK_DEVICE             =       11
SYSTEM_BOOT             =       12
TEMPORARY               =       13
AUTO_EXEC               =       14
INPUT_128               =       15
NUMFILETYPES            =       16
;       structure
SEQUENTIAL              =       0
VLIR                    =       1
;       DOS
DEL                     =       0
SEQ                     =       1
PRG                     =       2
USR                     =       3
REL                     =       4
CBM                     =       5

;drivetypes
DRV_NULL                =       0
DRV_1541                =       1
DRV_1571                =       2
DRV_1581                =       3
DRV_NETWORK             =       15

;various disk
REL_FILE_NUM            =       9
CMND_FILE_NUM           =       15
MAX_CMND_STR            =       32
DIR_1581_TRACK          =       40
DIR_ACC_CHAN            =       13
DIR_TRACK               =       18
N_TRACKS                =       35
DK_NM_ID_LEN            =       18
TRACK                   =       9
SECTOR                  =       12
TOTAL_BLOCKS            =       664

;colours
BLACK                   =       0
WHITE                   =       1
RED                     =       2
CYAN                    =       3
PURPLE                  =       4
GREEN                   =       5
BLUE                    =       6
YELLOW                  =       7
ORANGE                  =       8
BROWN                   =       9
LTRED                   =       10
DKGREY                  =       11
GREY                    =       12
MEDGREY                 =       12
LTGREEN                 =       13
LTBLUE                  =       14
LTGREY                  =       15

;vic memory banks
GRBANK0                 =       %11
GRBANK1                 =       %10
GRBANK2                 =       %01
GRBANK3                 =       %00

;screen
VIC_X_POS_OFF           =       24
VIC_Y_POS_OFF           =       50
SC_BYTE_WIDTH           =       40
.ifdef __GEOS_CBM__
SC_PIX_HEIGHT           =       200
SC_PIX_WIDTH            =       320
.else
SC_PIX_HEIGHT           =       192
SC_PIX_WIDTH            =       560
.endif
SC_SIZE                 =       8000
;128 screen size constants
SCREENBYTEWIDTH         =       80
SCREENPIXELWIDTH        =       640


;control characters
EOF                     =       0
BACKSPACE               =       8
FORWARDSPACE            =       9
TAB                     =       9
LF                      =       10
HOME                    =       11
PAGE_BREAK              =       12
UPLINE                  =       12
CR                      =       13
ULINEON                 =       14
ULINEOFF                =       15
ESC_GRAPHICS            =       16
ESC_RULER               =       17
REV_ON                  =       18
REV_OFF                 =       19
GOTOX                   =       20
GOTOY                   =       21
GOTOXY                  =       22
NEWCARDSET              =       23
BOLDON                  =       24
ITALICON                =       25
OUTLINEON               =       26
PLAINTEXT               =       27

;keyboard
KEY_F1                  =       1
KEY_F2                  =       2
KEY_F3                  =       3
KEY_F4                  =       4
KEY_F5                  =       5
KEY_F6                  =       6
KEY_NOSCRL              =       7
KEY_ENTER               =       11
KEY_F7                  =       14
KEY_F8                  =       15
KEY_UP                  =       16
KEY_DOWN                =       17
KEY_HOME                =       18
KEY_CLEAR               =       19
KEY_LARROW              =       20
KEY_UPARROR             =       21
KEY_STOP                =       22
KEY_RUN                 =       23
KEY_BPS                 =       24
KEY_HELP                =       25
KEY_ALT                 =       26
KEY_ESC                 =       27
KEY_INSERT              =       28
KEY_DELETE              =       29
KEY_RIGHT               =       30
KEY_INVALID             =       31
KEY_LEFT                =       BACKSPACE

;DialogBox
;       icons
OK                      =       1
CANCEL                  =       2
YES                     =       3
NO                      =       4
OPEN                    =       5
DISK                    =       6
;       commands
DBTXTSTR                =       11
DBVARSTR                =       12
DBGETSTRING             =       13
DBSYSOPV                =       14
DBGRPHSTR               =       15
DBGETFILES              =       16
DBOPVEC                 =       17
DBUSRICON               =       18
DB_USR_ROUT             =       19
;       tabulation in standard window
DBI_X_0                 =       1
DBI_X_1                 =       9
DBI_X_2                 =       17
DBI_Y_0                 =       8
DBI_Y_1                 =       40
DBI_Y_2                 =       72
;       standard window
SET_DB_POS              =       0
DEF_DB_POS              =       $80
DEF_DB_TOP              =       32
DEF_DB_BOT              =       127
DEF_DB_LEFT             =       64
DEF_DB_RIGHT            =       255
;       text tabulation
TXT_LN_1_Y              =       16
TXT_LN_2_Y              =       32
TXT_LN_3_Y              =       48
TXT_LN_4_Y              =       64
TXT_LN_5_Y              =       80
TXT_LN_X                =       16
;       ???
SYSDBI_HEIGHT           =       16
SYSDBI_WIDTH            =       6

;GraphicsString - commands
MOVEPENTO               =       1
LINETO                  =       2
RECTANGLETO             =       3
NEWPATTERN              =       5
ESC_PUTSTRING           =       6
FRAME_RECTO             =       7
PEN_X_DELTA             =       8
PEN_Y_DELTA             =       9
PEN_XY_DELTA            =       10


;DoMenu - menutypes
MENU_ACTION             =       $00
DYN_SUB_MENU            =       $40
SUB_MENU                =       $80
HORIZONTAL              =       %00000000
VERTICAL                =       %10000000

;Errors
ANY_FAULT               =       %11110000
NO_BLOCKS               =       1
INV_TRACK               =       2
INSUFF_SPACE            =       3
FULL_DIRECTORY          =       4
FILE_NOT_FOUND          =       5
BAD_BAM                 =       6
UNOPENED_VLIR           =       7
INV_RECORD              =       8
OUT_OF_RECORDS          =       9
STRUCT_MISMAT           =       10
BFR_OVERFLOW            =       11
CANCEL_ERR              =       12
DEV_NOT_FOUND           =       13
INCOMPATIBLE            =       14
HDR_NOT_THERE           =       $20
NO_SYNC                 =       $21
DBLK_NOT_THERE          =       $22
DAT_CHKSUM_ERR          =       $23
WR_VER_ERR              =       $25
WR_PR_ON                =       $26
HDR_CHKSUM_ERR          =       $27
DSK_ID_MISMAT           =       $29
BYTE_DEC_ERR            =       $2e
DOS_MISMATCH            =       $73

;Offsets
;       ???
OFF_INDEX_PTR           =       1
;       icons
OFF_NM_ICNS             =       0
OFF_IC_XMOUSE           =       1
OFF_IC_YMOUSE           =       3
OFF_PIC_ICON            =       0
OFF_X_ICON_POS          =       2
OFF_Y_ICON_POS          =       3
OFF_WDTH_ICON           =       4
OFF_HEIGHT_ICON         =       5
OFF_SRV_RT_ICON         =       6
OFF_NX_ICON             =       8
;       menu
OFF_MY_TOP              =       0
OFF_MY_BOT              =       1
OFF_MX_LEFT             =       2
OFF_MX_RIGHT            =       4
OFF_NUM_M_ITEMS         =       6
OFF_1ST_M_ITEM          =       7
;       dialog box
OFF_DB_FORM             =       0
OFF_DB_TOP              =       1
OFF_DB_BOT              =       2
OFF_DB_LEFT             =       3
OFF_DB_RIGHT            =       5
OFF_DB_1STCMD           =       7
;       directory
;               disk header
OFF_TO_BAM              =       4
OFF_DISK_NAME           =       144
OFF_GS_DTYPE            =       189
OFF_OP_TR_SC            =       171
OFF_GS_ID               =       173
;               dir entry
.ifdef __GEOS_CBM__
FRST_FILE_ENTRY         =       2
OFF_CFILE_TYPE          =       0
OFF_DE_TR_SC            =       1
OFF_FNAME               =       3
OFF_GHDR_PTR            =       19
OFF_GSTRUC_TYPE         =       21
OFF_GFILE_TYPE          =       22
OFF_YEAR                =       23
OFF_SIZE                =       28
OFF_NXT_FILE            =       32
.else
FRST_FILE_ENTRY         =       43
OFF_GSTRUC_TYPE         =       0
OFF_FNAME               =       1
OFF_GFILE_TYPE          =       16
OFF_DE_TR_SC            =       17
OFF_SIZE                =       21
OFF_YEAR                =       25
OFF_GHDR_PTR            =       31
OFF_NXT_FILE            =       39
.endif
;               file header
O_GHIC_WIDTH            =       2
O_GHIC_HEIGHT           =       3
O_GHIC_PIC              =       4
O_GHCMDR_TYPE           =       68
O_GHGEOS_TYPE           =       69
O_GHSTR_TYPE            =       70
O_GHST_ADDR             =       71
O_GHEND_ADDR            =       73
O_GHST_VEC              =       75
O_GHFNAME               =       77
O_128_FLAGS             =       96
O_GH_AUTHOR             =       97
O_GHP_DISK              =       97
O_GHP_FNAME             =       117
O_GHINFO_TXT            =       $a0

; values for CPU_DATA - C64 memory config
RAM_64K                 =       $30     ;64K RAM
IO_IN                   =       $35     ;60K RAM, 4K I/O space in
KRNL_IO_IN              =       $36     ;kernal and I/O mapped into memory
KRNL_BAS_IO_IN          =       $37     ;kernal, I/O and basic mapped into memory

;values for config - C128 mmu
CIOIN                   =       $7E     ;60K RAM, 4K I/O space in
CRAM64K                 =       $7F     ;64K RAM
CKRNLBASIOIN            =       $40     ;kernal, I/O and basic ROM's mapped into memory
CKRNLIOIN               =       $4E     ;Kernal ROM and I/O space mapped in

;values of faultData - pointer position vs. mouseTop/Bottom/Left/Right
;       bit numbers
OFFTOP_BIT              =       7
OFFBOTTOM_BIT           =       6
OFFLEFT_BIT             =       5
OFFRIGHT_BIT            =       4
OFFMENU_BIT             =       3
;       masks
SET_OFFTOP              =       %10000000
SET_OFFBOTTOM           =       %01000000
SET_OFFLEFT             =       %00100000
SET_OFFRIGHT            =       %00010000
SET_OFFMENU             =       %00001000

;values of currentMode
;       bit numbers
UNDERLINE_BIT           =       7
BOLD_BIT                =       6
REVERSE_BIT             =       5
ITALIC_BIT              =       4
OUTLINE_BIT             =       3
SUPERSCRIPT_BIT         =       2
SUBSCRIPT_BIT           =       1
;       masks
SET_UNDERLINE           =       %10000000
SET_BOLD                =       %01000000
SET_REVERSE             =       %00100000
SET_ITALIC              =       %00010000
SET_OUTLINE             =       %00001000
SET_SUPERSCRIPT         =       %00000100
SET_SUBSCRIPT           =       %00000010
SET_PLAINTEXT           =       %00000000

;Process control variable
;       bit numbers
RUNABLE_BIT             =       7
BLOCKED_BIT             =       6
FROZEN_BIT              =       5
NOTIMER_BIT             =       4
;       masks
SET_RUNABLE             =       %10000000
SET_BLOCKED             =       %01000000
SET_FROZEN              =       %00100000
SET_NOTIMER             =       %00010000

;mouseOn
;       bit numbers
MOUSEON_BIT             =       7
MENUON_BIT              =       6
ICONSON_BIT             =       5
;       masks
SET_MSE_ON              =       %10000000
SET_MENUON              =       %01000000
SET_ICONSON             =       %00100000

;pressFlag
;       bit numbers
KEYPRESS_BIT            =       7
INPUT_BIT               =       6
MOUSE_BIT               =       5
;       masks
SET_KEYPRESS            =       %10000000
SET_INPUTCHG            =       %01000000
SET_MOUSE               =       %00100000

;dispBufferOn
ST_WRGS_FORE            =       $20
ST_WR_BACK              =       $40
ST_WR_FORE              =       $80

;alarmSetFlag
ALARMMASK               =       %00000100

;PutDecimal
    ;leading zeroes
SET_NOSURPRESS          =       %00000000
SET_SURPRESS            =       %01000000
    ;justification
SET_RIGHTJUST           =       %00000000
SET_LEFTJUST            =       %10000000

;icons, menus status flags
ST_FLASH                =       $80
ST_INVERT               =       $40
ST_LD_AT_ADDR           =       $01
ST_LD_DATA              =       $80
ST_PR_DATA              =       $40
ST_WR_PR                =       $40

;???
ADD1_W                  =       $2000
DOUBLE_B                =       $80
DOUBLE_W                =       $8000

CLR_SAVE                =       %01000000
CONSTRAINED             =       %01000000
UN_CONSTRAINED          =       %00000000
FG_SAVE                 =       %10000000

FUTURE1                 =       7
FUTURE2                 =       8
FUTURE3                 =       9
FUTURE4                 =       10
USELAST                 =       127
SHORTCUT                =       128
