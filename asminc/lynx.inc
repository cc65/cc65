; Lynx system hardware includes
; Shawn Jefferson
; June 18th, 2004
;
; Reference:
;  Bastian Schick's Lynx Documentation
;  http://www.geocities.ws/SiliconValley/Byte/4242/lynx/
;

; ***
; *** Suzy Addresses
; ***

; Sprite Control Block

TMPADRL     = $FC00
TMPADRH     = $FC01
TILTACUML   = $FC02
TILTACUMH   = $FC03
HOFFL       = $FC04
HOFFH       = $FC05
VOFFL       = $FC06
VOFFH       = $FC07
VIDBASL     = $FC08
VIDBASH     = $FC09
COLLBASL    = $FC0A
COLLBASH    = $FC0B
VIDADRL     = $FC0C
VIDADRH     = $FC0D
COLLADRL    = $FC0E
COLLADRH    = $FC0F
SCBNEXTL    = $FC10
SCBNEXTH    = $FC11
SPRDLINEL   = $FC12
SPRDLINEH   = $FC13
HPOSSTRTL   = $FC14
HPOSSTRTH   = $FC15
VPOSSTRTL   = $FC16
VPOSSTRTH   = $FC17
SPRHSIZL    = $FC18
SPRHSIZH    = $FC19
SPRVSIZL    = $FC1A
SPRVSIZH    = $FC1B
STRETCHL    = $FC1C
STRETCHH    = $FC1D
TILTL       = $FC1E
TILTH       = $FC1F
SPRDOFFL    = $FC20
SPRDOFFH    = $FC21
SPRVPOSL    = $FC22
SPRVPOSH    = $FC23
COLLOFFL    = $FC24
COLLOFFH    = $FC25
VSIZACUML   = $FC26
VSIZACUMH   = $FC27
HSIZOFFL    = $FC28
HSIZOFFH    = $FC29
VSIZOFFL    = $FC2A
VSIZOFFH    = $FC2B
SCBADRL     = $FC2C
SCBADRH     = $FC2D
PROCADRL    = $FC2E
PROCADRH    = $FC2F

; Suzy Math

MATHD       = $FC52
MATHC       = $FC53
MATHB       = $FC54
MATHA       = $FC55
MATHP       = $FC56
MATHN       = $FC57
MATHH       = $FC60
MATHG       = $FC61
MATHF       = $FC62
MATHE       = $FC63
MATHM       = $FC6C
MATHL       = $FC6D
MATHK       = $FC6E
MATHJ       = $FC6F

; Suzy sprite engine

SPRCTL0          = $FC80
; Sprite bits-per-pixel definitions
BPP_MASK         = %11000000  ; Mask for settings bits per pixel
BPP_1            = %00000000
BPP_2            = %01000000
BPP_3            = %10000000
BPP_4            = %11000000
; More sprite control 0 bit definitions
HFLIP            = %00100000
VFLIP            = %00010000
; Sprite types - redefined to reflect the reality caused by the shadow error
TYPE_SHADOW      = %00000111
TYPE_XOR         = %00000110
TYPE_NONCOLL     = %00000101  ; Non-colliding
TYPE_NORMAL      = %00000100
TYPE_BOUNDARY    = %00000011
TYPE_BSHADOW     = %00000010  ; Background shadow
TYPE_BACKNONCOLL = %00000001  ; Background non-colliding
TYPE_BACKGROUND  = %00000000

SPRCTL1     = $FC81
LITERAL     = %10000000
PACKED      = %00000000
ALGO3       = %01000000  ; Broken, do not set this bit!
; Sprite reload mask definitions
RELOAD_MASK = %00110000
RENONE      = %00000000  ; Reload nothing
REHV        = %00010000  ; Reload hsize, vsize
REHVS       = %00100000  ; Reload hsize, vsize, stretch
REHVST      = %00110000  ; Reload hsize, vsize, stretch, tilt
; More sprite control 1 bit definitions
REUSEPAL    = %00001000
SKIP        = %00000100
DRAWUP      = %00000010
DRAWLEFT    = %00000001

SPRCOLL   = $FC82
SPRINIT   = $FC83
SUZYHREV  = $FC88
SUZYSREV  = $FC89
SUZYBUSEN = $FC90

SPRGO = $FC91
; SPRGO bit definitions
SPRITE_GO = %00000001  ; sprite process start bit
EVER_ON   = %00000100  ; everon detector enable

SPRSYS = $FC92
; SPRSYS bit definitions for write operations
SIGNMATH      = %10000000  ; signed math
ACCUMULATE    = %01000000  ; accumulate multiplication results
NO_COLLIDE    = %00100000  ; do not collide with any sprites (also SPRCOLL bit definition)
VSTRETCH      = %00010000  ; stretch v
LEFTHAND      = %00001000
CLR_UNSAFE    = %00000100  ; unsafe access reset
SPRITESTOP    = %00000010  ; request to stop sprite process
; SPRSYS bit definitions for read operations
MATHWORKING   = %10000000  ; math operation in progress
MATHWARNING   = %01000000  ; accumulator overflow on multiple or divide by zero
MATHCARRY     = %00100000  ; last carry bit
VSTRETCHING   = %00010000
LEFTHANDED    = %00001000
UNSAFE_ACCESS = %00000100  ; unsafe access performed
SPRITETOSTOP  = %00000010  ; requested to stop
SPRITEWORKING = %00000001  ; sprite process is active

JOYSTICK = $FCB0
; JOYSTICK bit definitions
JOYPAD_UP      = %10000000
JOYPAD_DOWN    = %01000000
JOYPAD_LEFT    = %00100000
JOYPAD_RIGHT   = %00010000
BUTTON_OPTION1 = %00001000
BUTTON_OPTION2 = %00000100
BUTTON_INNER   = %00000010
BUTTON_OUTER   = %00000001

SWITCHES    = $FCB1
; SWITCHES bit definitions
CART1_IO_INACTIVE = %00000100
CART0_IO_INACTIVE = %00000010
BUTTON_PAUSE      = %00000001

RCART0      = $FCB2
RCART1      = $FCB3
LEDS        = $FCC0
PARSTATUS   = $FCC2
PARDATA     = $FCC3
HOWIE       = $FCC4

;
; *** Mikey Addresses
; ***

; Mikey timers

; Logical timer names
TIMER0 = $FD00
TIMER1 = $FD04
TIMER2 = $FD08
TIMER3 = $FD0C
TIMER4 = $FD10
TIMER5 = $FD14
TIMER6 = $FD18
TIMER7 = $FD1C
HTIMER = TIMER0  ; horizontal line timer (timer 0)
VTIMER = TIMER2  ; vertical blank timer (timer 2)
STIMER = TIMER7  ; sound timer (timer 7)

HTIMBKUP = $FD00  ; horizontal line timer (timer 0)
HTIMCTLA = $FD01
HTIMCNT  = $FD02
HTIMCTLB = $FD03
VTIMBKUP = $FD08  ; vertical blank timer (timer 2)
VTIMCTLA = $FD09
VTIMCNT  = $FD0A
VTIMCTLB = $FD0B
BAUDBKUP = $FD10  ; serial timer (timer 4)
STIMBKUP = $FD1C  ; sound timer (timer 7)
STIMCTLA = $FD1D
STIMCNT  = $FD1E
STIMCTLB = $FD1F

TIM0BKUP = $FD00
TIM0CTLA = $FD01
TIM0CNT  = $FD02
TIM0CTLB = $FD03
TIM1BKUP = $FD04
TIM1CTLA = $FD05
TIM1CNT  = $FD06
TIM1CTLB = $FD07
TIM2BKUP = $FD08
TIM2CTLA = $FD09
TIM2CNT  = $FD0A
TIM2CTLB = $FD0B
TIM3BKUP = $FD0C
TIM3CTLA = $FD0D
TIM3CNT  = $FD0E
TIM3CTLB = $FD0F
TIM4BKUP = $FD10
TIM4CTLA = $FD11
TIM4CNT  = $FD12
TIM4CTLB = $FD13
TIM5BKUP = $FD14
TIM5CTLA = $FD15
TIM5CNT  = $FD16
TIM5CTLB = $FD17
TIM6BKUP = $FD18
TIM6CTLA = $FD19
TIM6CNT  = $FD1A
TIM6CTLB = $FD1B
TIM7BKUP = $FD1C
TIM7CTLA = $FD1D
TIM7CNT  = $FD1E
TIM7CTLB = $FD1F

; Timer offsets
TIM_BACKUP   = 0
TIM_CONTROLA = 1
TIM_COUNT    = 2
TIM_CONTROLB = 3

; TIM_CONTROLA control bits
ENABLE_INT     = %10000000
RESET_DONE     = %01000000
ENABLE_RELOAD  = %00010000
ENABLE_COUNT   = %00001000
AUD_CLOCK_MASK = %00000111
; Clock settings
AUD_LINKING = %00000111
AUD_64      = %00000110
AUD_32      = %00000101
AUD_16      = %00000100
AUD_8       = %00000011
AUD_4       = %00000010
AUD_2       = %00000001
AUD_1       = %00000000

; TIM_CONTROLB control bits
TIMER_DONE = %00001000
LAST_CLOCK = %00000100
BORROW_IN  = %00000010
BORROW_OUT = %00000001

; Mikey Audio

AUDIO0 = $FD20  ; audio channel 0
AUDIO1 = $FD28  ; audio channel 1
AUDIO2 = $FD30  ; audio channel 2
AUDIO3 = $FD38  ; audio channel 3

AUD0VOL   = $FD20
AUD0FEED  = $FD21
AUD0OUT   = $FD22
AUD0SHIFT = $FD23
AUD0BKUP  = $FD24
AUD0CTLA  = $FD25
AUD0CNT   = $FD26
AUD0CTLB  = $FD27
AUD1VOL   = $FD28
AUD1FEED  = $FD29
AUD1OUT   = $FD2A
AUD1SHIFT = $FD2B
AUD1BKUP  = $FD2C
AUD1CTLA  = $FD2D
AUD1CNT   = $FD2E
AUD1CTLB  = $FD2F
AUD2VOL   = $FD30
AUD2FEED  = $FD31
AUD2OUT   = $FD32
AUD2SHIFT = $FD33
AUD2BKUP  = $FD34
AUD2CTLA  = $FD35
AUD2CNT   = $FD36
AUD2CTLB  = $FD37
AUD3VOL   = $FD38
AUD3FEED  = $FD39
AUD3OUT   = $FD3A
AUD3SHIFT = $FD3B
AUD3BKUP  = $FD3C
AUD3CTLA  = $FD3D
AUD3CNT   = $FD3E
AUD3CTLB  = $FD3F

; AUD_CONTROL bits are almost identical to TIM_CONTROLA bits.
; See TIM_CONTROLA above for the other definitions
FEEDBACK_7       = %10000000
ENABLE_INTEGRATE = %00100000

; Stereo control registers follow
; Stereo capability does not exist in all Lynxes
; Left and right may be reversed, and if so will be corrected in a later
; release
ATTENREG0 = $FD40 ; Stereo attenuation registers
ATTENREG1 = $FD41
ATTENREG2 = $FD42
ATTENREG3 = $FD43

MPAN    = $FD44
MSTEREO = $FD50
; Bit definitions for MPAN and MSTEREO registers
LEFT_ATTENMASK  = %11110000
RIGHT_ATTENMASK = %00001111
LEFT3_SELECT  = %10000000
LEFT2_SELECT  = %01000000
LEFT1_SELECT  = %00100000
LEFT0_SELECT  = %00010000
RIGHT3_SELECT = %00001000
RIGHT2_SELECT = %00000100
RIGHT1_SELECT = %00000010
RIGHT0_SELECT = %00000001

; Mikey interrupts

INTRST = $FD80
INTSET = $FD81

; Interrupt bits in INTRST and INTSET
TIMER0_INTERRUPT = %00000001
TIMER1_INTERRUPT = %00000010
TIMER2_INTERRUPT = %00000100
TIMER3_INTERRUPT = %00001000
TIMER4_INTERRUPT = %00010000
TIMER5_INTERRUPT = %00100000
TIMER6_INTERRUPT = %01000000
TIMER7_INTERRUPT = %10000000

HBL_INTERRUPT    = TIMER0_INTERRUPT
VBL_INTERRUPT    = TIMER2_INTERRUPT
SERIAL_INTERRUPT = TIMER4_INTERRUPT
SND_INTERRUPT    = TIMER7_INTERRUPT

MAGRDY0 = $FD84
MAGRDY1 = $FD85
AUDIN   = $FD86
SYSCTL1 = $FD87
; SYSCTL1 bit definitions
POWERON          = %00000010
CART_ADDR_STROBE = %00000001

MIKEYHREV = $FD88
MIKEYSREV = $FD89

IODIR = $FD8A
IODAT = $FD8B
; IODIR and IODAT bit definitions
AUDIN_BIT      = %00010000  ; Note that there is also the address AUDIN
READ_ENABLE    = %00010000  ; Same bit for AUDIN_BIT
RESTLESS       = %00001000
NOEXP          = %00000100  ; If set, redeye is not connected
CART_ADDR_DATA = %00000010
CART_POWER_OFF = %00000010  ; Same bit for CART_ADDR_DATA
EXTERNAL_POWER = %00000001

SERCTL = $FD8C
; SERCTL bit definitions for write operations
TXINTEN  = %10000000
RXINTEN  = %01000000
PAREN    = %00010000
RESETERR = %00001000
TXOPEN   = %00000100
TXBRK    = %00000010
PAREVEN  = %00000001
; SERCTL bit definitions for read operations
TXRDY    = %10000000
RXRDY    = %01000000
TXEMPTY  = %00100000
PARERR   = %00010000
OVERRUN  = %00001000
FRAMERR  = %00000100
RXBRK    = %00000010
PARBIT   = %00000001

SERDAT   = $FD8D
SDONEACK = $FD90
CPUSLEEP = $FD91
DISPCTL  = $FD92
; DISPCTL bit definitions
DISP_COLOR   = %10000000  ; must be set to 1
DISP_FOURBIT = %01000000  ; must be set to 1
DISP_FLIP    = %00100000
DMA_ENABLE   = %00010000  ; must be set to 1

PBKUP    = $FD93
DISPADRL = $FD94
DISPADRH = $FD95

MTEST0 = $FD9C
; MTEST0 bit definitions
AT_CNT16   = %10000000
AT_TEST    = %01000000
XCLKEN     = %00100000
UART_TURBO = %00010000
ROM_SEL    = %00001000
ROM_TEST   = %00000100
M_TEST     = %00000010
CPU_TEST   = %00000001

MTEST1 = $FD9D
; MTEST1 bit definitions
P_CNT16     = %01000000
REF_CNT16   = %00100000
VID_TRIG    = %00010000
REF_TRIG    = %00001000
VID_DMA_DIS = %00000100
REF_FAST    = %00000010
REF_DIS     = %00000001

MTEST2 = $FD9E
; MTEST2 bit definitions
V_STROBE  = %00010000
V_ZERO    = %00001000
H_120     = %00000100
H_ZERO    = %00000010
V_BLANKEF = %00000001

PALETTE  = $FDA0  ; hardware rgb palette
GCOLMAP  = $FDA0  ; hardware rgb palette (green)
RBCOLMAP = $FDB0  ; hardware rgb palette (red-blue)

; Memory mapping control and 6502 vectors

MAPCTL        = $FFF9
; MAPCTL bit definitions
TURBO_DISABLE = %10000000
VECTOR_SPACE  = %00001000  ; 1 maps RAM into specified space
ROM_SPACE     = %00000100
MIKEY_SPACE   = %00000010
SUZY_SPACE    = %00000001

VECTORS  = $FFFB
INTVECTL = $FFFE
INTVECTH = $FFFF
RSTVECTL = $FFFC
RSTVECTH = $FFFD
NMIVECTL = $FFFA
NMIVECTH = $FFFB
