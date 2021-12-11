; supervision symbols

; supervision 65c02s
; in cc65 up to 2.9.1 65c02 means 65sc02

lcd_addr = $4000
LCD_LINESIZE = $30
LCD_WIDTH = 160
LCD_HEIGHT = 160
; 2 bit per pixel, packed

lcd_width = $2000
lcd_height = $2001
lcd_xpos = $2002 ; in pixel, bit 0+1 not used
lcd_ypos = $2003 ; weird

sv_port_r = $2021
sv_port_w = $2022

sv_timer_count = $2023
; read for quitting
sv_timer_quit = $2024

; bit 0 timer, bit 1 dma
sv_irq_source = $2027
SV_IRQ_REQUEST_TIMER = 1
SV_IRQ_REQUEST_DMA = 2

; bit 5,6,7 select bank at 0x8000
sv_bank = $2026
SV_NMI_ENABLE_ON = 1
SV_IRQ_ENABLE_TIMER = 2
SV_IRQ_ENABLE_DMA = 4
SV_LCD_ON = 8
SV_TIMER_MODE_240Hz = $10 ; else 15360


; low activ/pressed
sv_control = $2020
SV_RIGHT = 1
SV_LEFT = 2
SV_DOWN = 4
SV_UP = 8
SV_BUTTONB = $10
SV_BUTTONA = $20
SV_SELECT = $40
SV_START = $80

; frequency=125000/counter
sv_audio_right_counter = $2010 ;word
sv_audio_left_counter = $2014
SV_AUDIO_ON =$40
;bits 0..3 volume
; bit 4 ?
; bit 5 ?
sv_audio_right_control = $2012
sv_audio_left_control = $2016
; write activates tone for x/60 sec (0 means 256)
sv_audio_right_timer = $2013
sv_audio_left_timer = $2017


;read for irq quitting
sv_dma_quit = $2025
sv_dma_on = $201c
; bit 7 true start, false stop
sv_dma_start = $2018 ; word
sv_dma_size = $201a ; *32 samples
sv_dma_control = $201b
; bit 0,1 speed: 0 15360, 11 15360/4
; bit 2,3 volume: 0 silent, 11 loud

sv_noise_volume = $2028 ; and frequency
sv_noise_timer = $2029
sv_noise_control = $202a
