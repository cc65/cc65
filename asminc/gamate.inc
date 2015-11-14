; gamate symbols

.p02

LCD_WIDTH = 160
LCD_HEIGHT = 152

lcd_base = $5000
lcd_mode = $5001
lcd_mode_y_increment = $40
lcd_x_pos = $5002
lcd_x_pos_plane2_flag =$80
lcd_y_pos = $5003
lcd_x = $5004
lcd_y = $5005
lcd_read = $5006
lcd_data = $5007

audio_base =$4000

controls = $4400
; active low
up=1
down=2
left=4
right=8
button_a=$10
button_b=$20
start=$40
select=$80

cart_main = $6020
;cart_nmi = $6023
cart_irq = $6026 ; 135 hz
cart_font = $6029

nmi_enable = $c
nmi_4800 = $a

_f009 = $f009 ; wait 1/60s
_f00c = $f00c ; install font
_f012 = $f012 ; write screen

