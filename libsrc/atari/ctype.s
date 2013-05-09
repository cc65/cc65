;
; Ullrich von Bassewitz, 02.06.1998
;
; Character specification table.
;
; adapted to Atari by Christian Groessler, June 2000
;

; The tables are readonly, put them into the rodata segment

.rodata

; The following 256 byte wide table specifies attributes for the isxxx type
; of functions. Doing it by a table means some overhead in space, but it
; has major advantages:
;
;   * It is fast. If it were'nt for the slow parameter passing of cc65, one
;     could even define macros for the isxxx functions (this is usually
;     done on other platforms).
;
;   * It is highly portable. The only unportable part is the table itself,
;     all real code goes into the common library.
;
;   * We save some code in the isxxx functions.
;
;
; Bit assignments:
;
;   0 - Lower case char
;   1 - Upper case char
;   2 - Numeric digit
;   3 - Hex digit (both, lower and upper)
;   4 - Control character
;   5 - The space character itself
;   6 - Other whitespace (that is: '\f', '\n', '\r', '\t' and '\v')
;   7 - Space or tab character

        .export         __ctype

__ctype:
        .byte   $00     ;   0/00 ___heart____
        .byte   $00     ;   1/01 ___l_tee____
        .byte   $00     ;   2/02 ___ctrl_B___
        .byte   $00     ;   3/03 ___ctrl_C___
        .byte   $00     ;   4/04 ___r_tee____
        .byte   $00     ;   5/05 ___ctrl_E___
        .byte   $00     ;   6/06 ___ctrl_F___
        .byte   $00     ;   7/07 ___ctrl_G___
        .byte   $00     ;   8/08 ___ctrl_H___
        .byte   $00     ;   9/09 ___ctrl_I___
        .byte   $00     ;  10/0a ___ctrl_J___
        .byte   $00     ;  11/0b ___ctrl_K___
        .byte   $00     ;  12/0c ___ctrl_L___
        .byte   $00     ;  13/0d ___ctrl_M___
        .byte   $00     ;  14/0e ___ctrl_N___
        .byte   $00     ;  15/0f ___ctrl_O___
        .byte   $00     ;  16/10 ____club____
        .byte   $00     ;  17/11 ___ctrl_Q___
        .byte   $00     ;  18/12 ___h_line___
        .byte   $00     ;  19/13 ___ctrl_S___
        .byte   $00     ;  20/14 ____ball____
        .byte   $00     ;  21/15 ___ctrl_U___
        .byte   $00     ;  22/16 ___ctrl_V___
        .byte   $00     ;  23/17 ___t_tee____
        .byte   $00     ;  24/18 ___b_tee____
        .byte   $00     ;  25/19 ___ctrl_Y___
        .byte   $00     ;  26/1a ___ctrl_Z___
        .byte   $10     ;  27/1b ____ESC_____
        .byte   $10     ;  28/1c ___crsr_up__
        .byte   $10     ;  29/1d ___crsr_dn__
        .byte   $10     ;  30/1e ___crsr_lf__
        .byte   $10     ;  31/1f ___crsr_rg__
        .byte   $A0     ;  32/20 ___SPACE___
        .byte   $00     ;  33/21 _____!_____
        .byte   $00     ;  34/22 _____"_____
        .byte   $00     ;  35/23 _____#_____
        .byte   $00     ;  36/24 _____$_____
        .byte   $00     ;  37/25 _____%_____
        .byte   $00     ;  38/26 _____&_____
        .byte   $00     ;  39/27 _____'_____
        .byte   $00     ;  40/28 _____(_____
        .byte   $00     ;  41/29 _____)_____
        .byte   $00     ;  42/2a _____*_____
        .byte   $00     ;  43/2b _____+_____
        .byte   $00     ;  44/2c _____,_____
        .byte   $00     ;  45/2d _____-_____
        .byte   $00     ;  46/2e _____._____
        .byte   $00     ;  47/2f _____/_____
        .byte   $0C     ;  48/30 _____0_____
        .byte   $0C     ;  49/31 _____1_____
        .byte   $0C     ;  50/32 _____2_____
        .byte   $0C     ;  51/33 _____3_____
        .byte   $0C     ;  52/34 _____4_____
        .byte   $0C     ;  53/35 _____5_____
        .byte   $0C     ;  54/36 _____6_____
        .byte   $0C     ;  55/37 _____7_____
        .byte   $0C     ;  56/38 _____8_____
        .byte   $0C     ;  57/39 _____9_____
        .byte   $00     ;  58/3a _____:_____
        .byte   $00     ;  59/3b _____;_____
        .byte   $00     ;  60/3c _____<_____
        .byte   $00     ;  61/3d _____=_____
        .byte   $00     ;  62/3e _____>_____
        .byte   $00     ;  63/3f _____?_____

        .byte   $00     ;  64/40 _____@_____
        .byte   $0A     ;  65/41 _____A_____
        .byte   $0A     ;  66/42 _____B_____
        .byte   $0A     ;  67/43 _____C_____
        .byte   $0A     ;  68/44 _____D_____
        .byte   $0A     ;  69/45 _____E_____
        .byte   $0A     ;  70/46 _____F_____
        .byte   $02     ;  71/47 _____G_____
        .byte   $02     ;  72/48 _____H_____
        .byte   $02     ;  73/49 _____I_____
        .byte   $02     ;  74/4a _____J_____
        .byte   $02     ;  75/4b _____K_____
        .byte   $02     ;  76/4c _____L_____
        .byte   $02     ;  77/4d _____M_____
        .byte   $02     ;  78/4e _____N_____
        .byte   $02     ;  79/4f _____O_____
        .byte   $02     ;  80/50 _____P_____
        .byte   $02     ;  81/51 _____Q_____
        .byte   $02     ;  82/52 _____R_____
        .byte   $02     ;  83/53 _____S_____
        .byte   $02     ;  84/54 _____T_____
        .byte   $02     ;  85/55 _____U_____
        .byte   $02     ;  86/56 _____V_____
        .byte   $02     ;  87/57 _____W_____
        .byte   $02     ;  88/58 _____X_____
        .byte   $02     ;  89/59 _____Y_____
        .byte   $02     ;  90/5a _____Z_____
        .byte   $00     ;  91/5b _____[_____
        .byte   $00     ;  92/5c _____\_____
        .byte   $00     ;  93/5d _____]_____
        .byte   $00     ;  94/5e _____^_____
        .byte   $00     ;  95/5f _UNDERLINE_
        .byte   $00     ;  96/60 __diamond__
        .byte   $09     ;  97/61 _____a_____
        .byte   $09     ;  98/62 _____b_____
        .byte   $09     ;  99/63 _____c_____
        .byte   $09     ; 100/64 _____d_____
        .byte   $09     ; 101/65 _____e_____
        .byte   $09     ; 102/66 _____f_____
        .byte   $01     ; 103/67 _____g_____
        .byte   $01     ; 104/68 _____h_____
        .byte   $01     ; 105/69 _____i_____
        .byte   $01     ; 106/6a _____j_____
        .byte   $01     ; 107/6b _____k_____
        .byte   $01     ; 108/6c _____l_____
        .byte   $01     ; 109/6d _____m_____
        .byte   $01     ; 110/6e _____n_____
        .byte   $01     ; 111/6f _____o_____
        .byte   $01     ; 112/70 _____p_____
        .byte   $01     ; 113/71 _____q_____
        .byte   $01     ; 114/72 _____r_____
        .byte   $01     ; 115/73 _____s_____
        .byte   $01     ; 116/74 _____t_____
        .byte   $01     ; 117/75 _____u_____
        .byte   $01     ; 118/76 _____v_____
        .byte   $01     ; 119/77 _____w_____
        .byte   $01     ; 120/78 _____x_____
        .byte   $01     ; 121/79 _____y_____
        .byte   $01     ; 122/7a _____z_____
        .byte   $00     ; 123/7b ___spade___
        .byte   $00     ; 124/7c __v_line___
        .byte   $10     ; 125/7d __CLRSCR___
        .byte   $D0     ; 126/7e __backtab__
        .byte   $D0     ; 127/7f ____tab____

        .byte   $00     ; 128/80 _inv_heart___
        .byte   $00     ; 129/81 _inv_l_tee___
        .byte   $00     ; 130/82 _inv_ctrl_B__
        .byte   $00     ; 131/83 _inv_ctrl_C__
        .byte   $00     ; 132/84 _inv_r_tee___
        .byte   $00     ; 133/85 _inv_ctrl_E__
        .byte   $00     ; 134/86 _inv_ctrl_F__
        .byte   $00     ; 135/87 _inv_ctrl_G__
        .byte   $00     ; 136/88 _inv_ctrl_H__
        .byte   $00     ; 137/89 _inv_ctrl_I__
        .byte   $00     ; 138/8a _inv_ctrl_J__
        .byte   $00     ; 139/8b _inv_ctrl_K__
        .byte   $00     ; 140/8c _inv_ctrl_L__
        .byte   $00     ; 141/8d _inv_ctrl_M__
        .byte   $00     ; 142/8e _inv_ctrl_N__
        .byte   $00     ; 143/8f _inv_ctrl_O__
        .byte   $00     ; 144/90 __inv__club__
        .byte   $00     ; 145/91 _inv_ctrl_Q__
        .byte   $00     ; 146/92 _inv_h_line__
        .byte   $00     ; 147/93 _inv_ctrl_S__
        .byte   $00     ; 148/94 __inv__ball__
        .byte   $00     ; 149/95 _inv_ctrl_U__
        .byte   $00     ; 150/96 _inv_ctrl_V__
        .byte   $00     ; 151/97 __inv_t_tee__
        .byte   $00     ; 152/98 __inv_b_tee__
        .byte   $00     ; 153/99 _inv_ctrl_Y__
        .byte   $00     ; 154/9a _inv_ctrl_Z__
        .byte   $50     ; 155/9b _____EOL_____
        .byte   $10     ; 156/9c ___CLRLINE___
        .byte   $10     ; 157/9d ___INSLINE___
        .byte   $10     ; 158/9e ____CLRTAB___
        .byte   $10     ; 159/9f ____INSTAB___
        .byte   $A0     ; 160/a0 __inv_SPACE__
        .byte   $00     ; 161/a1 ___inv_!_____
        .byte   $00     ; 162/a2 ___inv_"_____
        .byte   $00     ; 163/a3 ___inv_#_____
        .byte   $00     ; 164/a4 ___inv_$_____
        .byte   $00     ; 165/a5 ___inv_%_____
        .byte   $00     ; 166/a6 ___inv_&_____
        .byte   $00     ; 167/a7 ___inv_'_____
        .byte   $00     ; 168/a8 ___inv_(_____
        .byte   $00     ; 169/a9 ___inv_)_____
        .byte   $00     ; 170/aa ___inv_*_____
        .byte   $00     ; 171/ab ___inv_+_____
        .byte   $00     ; 172/ac ___inv_,_____
        .byte   $00     ; 173/ad ___inv_-_____
        .byte   $00     ; 174/ae ___inv_._____
        .byte   $00     ; 175/af ___inv_/_____
        .byte   $0C     ; 176/b0 ___inv_0_____
        .byte   $0C     ; 177/b1 ___inv_1_____
        .byte   $0C     ; 178/b2 ___inv_2_____
        .byte   $0C     ; 179/b3 ___inv_3_____
        .byte   $0C     ; 180/b4 ___inv_4_____
        .byte   $0C     ; 181/b5 ___inv_5_____
        .byte   $0C     ; 182/b6 ___inv_6_____
        .byte   $0C     ; 183/b7 ___inv_7_____
        .byte   $0C     ; 184/b8 ___inv_8_____
        .byte   $0C     ; 185/b9 ___inv_9_____
        .byte   $00     ; 186/ba ___inv_:_____
        .byte   $00     ; 187/bb ___inv_;_____
        .byte   $00     ; 188/bc ___inv_<_____
        .byte   $00     ; 189/bd ___inv_=_____
        .byte   $00     ; 190/be ___inv_>_____
        .byte   $00     ; 191/bf ___inv_?_____

        .byte   $00     ; 192/c0 ___inv_@_____
        .byte   $0A     ; 193/c1 ___inv_A_____
        .byte   $0A     ; 194/c2 ___inv_B_____
        .byte   $0A     ; 195/c3 ___inv_C_____
        .byte   $0A     ; 196/c4 ___inv_D_____
        .byte   $0A     ; 197/c5 ___inv_E_____
        .byte   $0A     ; 198/c6 ___inv_F_____
        .byte   $02     ; 199/c7 ___inv_G_____
        .byte   $02     ; 200/c8 ___inv_H_____
        .byte   $02     ; 201/c9 ___inv_I_____
        .byte   $02     ; 202/ca ___inv_J_____
        .byte   $02     ; 203/cb ___inv_K_____
        .byte   $02     ; 204/cc ___inv_L_____
        .byte   $02     ; 205/cd ___inv_M_____
        .byte   $02     ; 206/ce ___inv_N_____
        .byte   $02     ; 207/cf ___inv_O_____
        .byte   $02     ; 208/d0 ___inv_P_____
        .byte   $02     ; 209/d1 ___inv_Q_____
        .byte   $02     ; 210/d2 ___inv_R_____
        .byte   $02     ; 211/d3 ___inv_S_____
        .byte   $02     ; 212/d4 ___inv_T_____
        .byte   $02     ; 213/d5 ___inv_U_____
        .byte   $02     ; 214/d6 ___inv_V_____
        .byte   $02     ; 215/d7 ___inv_W_____
        .byte   $02     ; 216/d8 ___inv_X_____
        .byte   $02     ; 217/d9 ___inv_Y_____
        .byte   $02     ; 218/da ___inv_Z_____
        .byte   $00     ; 219/db ___inv_[_____
        .byte   $00     ; 220/dc ___inv_\_____
        .byte   $00     ; 221/dd ___inv_]_____
        .byte   $00     ; 222/de ___inv_^_____
        .byte   $00     ; 223/df _inv_UNDRLIN_
        .byte   $00     ; 224/e0 _inv_diamond_
        .byte   $09     ; 225/e1 ___inv_a_____
        .byte   $09     ; 226/e2 ___inv_b_____
        .byte   $09     ; 227/e3 ___inv_c_____
        .byte   $09     ; 228/e4 ___inv_d_____
        .byte   $09     ; 229/e5 ___inv_e_____
        .byte   $09     ; 230/e6 ___inv_f_____
        .byte   $01     ; 231/e7 ___inv_g_____
        .byte   $01     ; 232/e8 ___inv_h_____
        .byte   $01     ; 233/e9 ___inv_i_____
        .byte   $01     ; 234/ea ___inv_j_____
        .byte   $01     ; 235/eb ___inv_k_____
        .byte   $01     ; 236/ec ___inv_l_____
        .byte   $01     ; 237/ed ___inv_m_____
        .byte   $01     ; 238/ee ___inv_n_____
        .byte   $01     ; 239/ef ___inv_o_____
        .byte   $01     ; 240/f0 ___inv_p_____
        .byte   $01     ; 241/f1 ___inv_q_____
        .byte   $01     ; 242/f2 ___inv_r_____
        .byte   $01     ; 243/f3 ___inv_s_____
        .byte   $01     ; 244/f4 ___inv_t_____
        .byte   $01     ; 245/f5 ___inv_u_____
        .byte   $01     ; 246/f6 ___inv_v_____
        .byte   $01     ; 247/f7 ___inv_w_____
        .byte   $01     ; 248/f8 ___inv_x_____
        .byte   $01     ; 249/f9 ___inv_y_____
        .byte   $01     ; 250/fa ___inv_z_____
        .byte   $00     ; 251/fb __inv_spade__
        .byte   $00     ; 252/fc __inv_v_line_
        .byte   $10     ; 253/fd ____BEEP_____
        .byte   $10     ; 254/fe ____DELBS____
        .byte   $10     ; 255/ff ___INSERT____


