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

; Suzy Misc

SPRCTL0     = $FC80
SPRCTL1     = $FC81
SPRCOLL     = $FC82
SPRINIT     = $FC83
SUZYHREV    = $FC88
SUZYSREV    = $FC89
SUZYBUSEN   = $FC90
SPRGO       = $FC91
SPRSYS      = $FC92
JOYSTICK    = $FCB0
SWITCHES    = $FCB1
RCART0      = $FCB2
RCART1      = $FCB3
LEDS        = $FCC0
PARSTATUS   = $FCC2
PARDATA     = $FCC3
HOWIE       = $FCC4


; ***
; *** Mikey Addresses
; ***

; Mikey Timers

TIMER0      = $FD00
TIMER1      = $FD04
TIMER2      = $FD08
TIMER3      = $FD0C
TIMER4      = $FD10
TIMER5      = $FD14
TIMER6      = $FD18
TIMER7      = $FD1C
HTIMER      = $FD00             ; horizontal line timer (timer 0)
VTIMER      = $FD08             ; vertical blank timer (timer 2)
STIMER      = $FD1C             ; sound timer (timer 7)

HTIMBKUP    = $FD00             ; horizontal line timer (timer 0)
HTIMCTLA    = $FD01
HTIMCNT     = $FD02
HTIMCTLB    = $FD03
VTIMBKUP    = $FD08             ; vertical blank timer (timer 2)
VTIMCTLA    = $FD09
VTIMCNT     = $FD0A
VTIMCTLB    = $FD0B
BAUDBKUP    = $FD10             ; serial timer (timer 4)
STIMBKUP    = $FD1C             ; sound timer (timer 7)
STIMCTLA    = $FD1D
STIMCNT     = $FD1E
STIMCTLB    = $FD1F

TIM0BKUP    = $FD00
TIM0CTLA    = $FD01
TIM0CNT     = $FD02
TIM0CTLB    = $FD03 
TIM1BKUP    = $FD04
TIM1CTLA    = $FD05
TIM1CNT     = $FD06
TIM1CTLB    = $FD07 
TIM2BKUP    = $FD08
TIM2CTLA    = $FD09
TIM2CNT     = $FD0A
TIM2CTLB    = $FD0B 
TIM3BKUP    = $FD0C
TIM3CTLA    = $FD0D
TIM3CNT     = $FD0E
TIM3CTLB    = $FD0F 
TIM4BKUP    = $FD10
TIM4CTLA    = $FD11
TIM4CNT     = $FD12
TIM4CTLB    = $FD13 
TIM5BKUP    = $FD14
TIM5CTLA    = $FD15
TIM5CNT     = $FD16
TIM5CTLB    = $FD17 
TIM6BKUP    = $FD18
TIM6CTLA    = $FD19
TIM6CNT     = $FD1A
TIM6CTLB    = $FD1B 
TIM7BKUP    = $FD1C
TIM7CTLA    = $FD1D
TIM7CNT     = $FD1E
TIM7CTLB    = $FD1F 

; Mikey Audio

AUDIO0      = $FD20             ; audio channel 0
AUDIO1      = $FD28             ; audio channel 1
AUDIO2      = $FD30             ; audio channel 2
AUDIO3      = $FD38             ; audio channel 3

AUD0VOL     = $FD20
AUD0FEED    = $FD21
AUD0OUT     = $FD22
AUD0SHIFT   = $FD23
AUD0BKUP    = $FD24
AUD0CTLA    = $FD25
AUD0CNT     = $FD26
AUD0CTLB    = $FD27
AUD1VOL     = $FD28
AUD1FEED    = $FD29
AUD1OUT     = $FD2A
AUD1SHIFT   = $FD2B
AUD1BKUP    = $FD2C
AUD1CTLA    = $FD2D
AUD1CNT     = $FD2E
AUD1CTLB    = $FD2F
AUD2VOL     = $FD30
AUD2FEED    = $FD31
AUD2OUT     = $FD32
AUD2SHIFT   = $FD33
AUD2BKUP    = $FD34
AUD2CTLA    = $FD35
AUD2CNT     = $FD36
AUD2CTLB    = $FD37
AUD3VOL     = $FD38
AUD3FEED    = $FD39
AUD3OUT     = $FD3A
AUD3SHIFT   = $FD3B
AUD3BKUP    = $FD3C
AUD3CTLA    = $FD3D
AUD3CNT     = $FD3E
AUD3CTLB    = $FD3F
MSTEREO     = $FD50

; Mikey Misc

; Interrupt bits in INTRST and INTSET
TIMER0_INTERRUPT = $01
TIMER1_INTERRUPT = $02
TIMER2_INTERRUPT = $04
TIMER3_INTERRUPT = $08
TIMER4_INTERRUPT = $10
TIMER5_INTERRUPT = $20
TIMER6_INTERRUPT = $40
TIMER7_INTERRUPT = $80

HBL_INTERRUPT = TIMER0_INTERRUPT
VBL_INTERRUPT = TIMER2_INTERRUPT
SERIAL_INTERRUPT = TIMER4_INTERRUPT
SND_INTERRUPT = TIMER7_INTERRUPT

INTRST      = $FD80
INTSET      = $FD81
MAGRDY0     = $FD84
MAGRDY1     = $FD85
AUDIN       = $FD86
SYSCTL1     = $FD87
MIKEYHREV   = $FD88
MIKEYSREV   = $FD89
IODIR       = $FD8A
IODAT       = $FD8B
TxIntEnable = %10000000
RxIntEnable = %01000000
TxParEnable = %00010000
ResetErr    = %00001000
TxOpenColl  = %00000100
TxBreak     = %00000010
ParEven     = %00000001
TxReady     = %10000000
RxReady     = %01000000
TxEmpty     = %00100000
RxParityErr = %00010000
RxOverrun   = %00001000
RxFrameErr  = %00000100
RxBreak     = %00000010
ParityBit   = %00000001
SERCTL      = $FD8C
SERDAT      = $FD8D
SDONEACK    = $FD90
CPUSLEEP    = $FD91
DISPCTL     = $FD92
PBKUP       = $FD93
DISPADRL    = $FD94
DISPADRH    = $FD95
MTEST0      = $FD9C
MTEST1      = $FD9D
MTEST2      = $FD9E
PALETTE     = $FDA0                 ; hardware rgb palette
GCOLMAP     = $FDA0                 ; hardware rgb palette (green)
RBCOLMAP    = $FDB0                 ; hardware rgb palette (red-blue)


; ***
; *** Misc Hardware + 6502 vectors
; ***

MAPCTL      = $FFF9
VECTORS     = $FFFB
INTVECTL    = $FFFE
INTVECTH    = $FFFF
RSTVECTL    = $FFFC
RSTVECTH    = $FFFD
NMIVECTL    = $FFFA
NMIVECTH    = $FFFB

