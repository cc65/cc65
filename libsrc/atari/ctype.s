; ctype.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; Atari character specification table.
;

        .include        "ctypetable.inc"
        .export         __ctypeidx
        
; The tables are readonly, put them into the rodata segment

.rodata

__ctypeidx:
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;   0/00 ___heart____, 1/01 ___l_tee____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;   2/02 ___ctrl_B___, 3/03 ___ctrl_C___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;   4/04 ___r_tee____, 5/05 ___ctrl_E___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;   6/06 ___ctrl_F___, 7/07 ___ctrl_G___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;   8/08 ___ctrl_H___, 9/09 ___ctrl_I___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  10/0a ___ctrl_J___, 11/0b ___ctrl_K___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  12/0c ___ctrl_L___, 13/0d ___ctrl_M___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  14/0e ___ctrl_N___, 15/0f ___ctrl_O___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  16/10 ____club____, 17/11 ___ctrl_Q___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  18/12 ___h_line___, 19/13 ___ctrl_S___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  20/14 ____ball____, 21/15 ___ctrl_U___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  22/16 ___ctrl_V___, 23/17 ___t_tee____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  24/18 ___b_tee____, 25/19 ___ctrl_Y___
        ct_mix  CT_NONE_IDX, CT_CTRL_IDX                 ;  26/1a ___ctrl_Z___, 27/1b ____ESC_____
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  28/1c ___crsr_up__, 29/1d ___crsr_dn__
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  30/1e ___crsr_lf__, 31/1f ___crsr_rg__

        ct_mix  CT_SPACE_SPACETAB_IDX, CT_NONE_IDX       ;  32/20 ___SPACE___, 33/21 _____!_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  34/22 _____"_____, 35/23 _____#_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  36/24 _____$_____, 37/25 _____%_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  38/26 _____&_____, 39/27 _____'_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  40/28 _____(_____, 41/29 _____)_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  42/2a _____*_____, 43/2b _____+_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  44/2c _____,_____, 45/2d _____-_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  46/2e _____._____, 47/2f _____/_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ;  48/30 _____0_____, 49/31 _____1_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ;  50/32 _____2_____, 51/33 _____3_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ;  52/34 _____4_____, 53/35 _____5_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ;  54/36 _____6_____, 55/37 _____7_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ;  56/38 _____8_____, 57/39 _____9_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  58/3a _____:_____, 59/3b _____;_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  60/3c _____<_____, 61/3d _____=_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  62/3e _____>_____, 63/3f _____?_____

        ct_mix  CT_NONE_IDX, CT_UPPER_XDIGIT_IDX         ;  64/40 _____@_____, 65/41 _____A_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_XDIGIT_IDX ;  66/42 _____B_____, 67/43 _____C_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_XDIGIT_IDX ;  68/44 _____D_____, 69/45 _____E_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_IDX        ;  70/46 _____F_____, 71/47 _____G_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  72/48 _____H_____, 73/49 _____I_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  74/4a _____J_____, 75/4b _____K_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  76/4c _____L_____, 77/4d _____M_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  78/4e _____N_____, 79/4f _____O_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  80/50 _____P_____, 81/51 _____Q_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  82/52 _____R_____, 83/53 _____S_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  84/54 _____T_____, 85/55 _____U_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  86/56 _____V_____, 87/57 _____W_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ;  88/58 _____X_____, 89/59 _____Y_____
        ct_mix  CT_UPPER_IDX, CT_NONE_IDX                ;  90/5a _____Z_____, 91/5b _____[_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  92/5c _____\_____, 93/5d _____]_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  94/5e _____^_____, 95/5f _UNDERLINE_

        ct_mix  CT_NONE_IDX, CT_LOWER_XDIGIT_IDX         ;  96/60 __diamond__, 97/61 _____a_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_XDIGIT_IDX ;  98/62 _____b_____, 99/63 _____c_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_XDIGIT_IDX ; 100/64 _____d_____, 101/65 _____e_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_IDX        ; 102/66 _____f_____, 103/67 _____g_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 104/68 _____h_____, 105/69 _____i_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 106/6a _____j_____, 107/6b _____k_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 108/6c _____l_____, 109/6d _____m_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 110/6e _____n_____, 111/6f _____o_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 112/70 _____p_____, 113/71 _____q_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 114/72 _____r_____, 115/73 _____s_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 116/74 _____t_____, 117/75 _____u_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 118/76 _____v_____, 119/77 _____w_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 120/78 _____x_____, 121/79 _____y_____
        ct_mix  CT_LOWER_IDX, CT_NONE_IDX                ; 122/7a _____z_____, 123/7b ___spade___
        ct_mix  CT_NONE_IDX, CT_CTRL_IDX                 ; 124/7c __v_line___, 125/7d __CLRSCR___
        ct_mix  CT_CTRL_WS_SPACETAB_IDX, CT_CTRL_WS_SPACETAB_IDX
                                                         ; 126/7e __backtab__, 127/7f ____tab____

        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 128/80 _inv_heart___, 129/81 _inv_l_tee___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 130/82 _inv_ctrl_B__, 131/83 _inv_ctrl_C__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 132/84 _inv_r_tee___, 133/85 _inv_ctrl_E__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 134/86 _inv_ctrl_F__, 135/87 _inv_ctrl_G__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 136/88 _inv_ctrl_H__, 137/89 _inv_ctrl_I__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 138/8a _inv_ctrl_J__, 139/8b _inv_ctrl_K__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 140/8c _inv_ctrl_L__, 141/8d _inv_ctrl_M__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 142/8e _inv_ctrl_N__, 143/8f _inv_ctrl_O__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 144/90 __inv__club__, 145/91 _inv_ctrl_Q__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 146/92 _inv_h_line__, 147/93 _inv_ctrl_S__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 148/94 __inv__ball__, 149/95 _inv_ctrl_U__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 150/96 _inv_ctrl_V__, 151/97 __inv_t_tee__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 152/98 __inv_b_tee__, 153/99 _inv_ctrl_Y__
        ct_mix  CT_NONE_IDX, CT_CTRL_WS_IDX              ; 154/9a _inv_ctrl_Z__, 155/9b _____EOL_____
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 156/9c ___CLRLINE___, 157/9d ___INSLINE___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 158/9e ____CLRTAB___, 159/9f ____INSTAB___

        ct_mix  CT_SPACE_SPACETAB_IDX, CT_NONE_IDX       ; 160/a0 __inv_SPACE__, 161/a1 ___inv_!_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 162/a2 ___inv_"_____, 163/a3 ___inv_#_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 164/a4 ___inv_$_____, 165/a5 ___inv_%_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 166/a6 ___inv_&_____, 167/a7 ___inv_'_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 168/a8 ___inv_(_____, 169/a9 ___inv_)_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 170/aa ___inv_*_____, 171/ab ___inv_+_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 172/ac ___inv_,_____, 173/ad ___inv_-_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 174/ae ___inv_._____, 175/af ___inv_/_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ; 176/b0 ___inv_0_____, 177/b1 ___inv_1_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ; 178/b2 ___inv_2_____, 179/b3 ___inv_3_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ; 180/b4 ___inv_4_____, 181/b5 ___inv_5_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ; 182/b6 ___inv_6_____, 183/b7 ___inv_7_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ; 184/b8 ___inv_8_____, 185/b9 ___inv_9_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 186/ba ___inv_:_____, 187/bb ___inv_;_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 188/bc ___inv_<_____, 189/bd ___inv_=_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 190/be ___inv_>_____, 191/bf ___inv_?_____

        ct_mix  CT_NONE_IDX, CT_UPPER_XDIGIT_IDX         ; 192/c0 ___inv_@_____, 193/c1 ___inv_A_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_XDIGIT_IDX ; 194/c2 ___inv_B_____, 195/c3 ___inv_C_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_XDIGIT_IDX ; 196/c4 ___inv_D_____, 197/c5 ___inv_E_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_IDX        ; 198/c6 ___inv_F_____,199/c7 ___inv_G_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 200/c8 ___inv_H_____, 201/c9 ___inv_I_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 202/ca ___inv_J_____, 203/cb ___inv_K_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 204/cc ___inv_L_____, 205/cd ___inv_M_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 206/ce ___inv_N_____, 207/cf ___inv_O_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 208/d0 ___inv_P_____, 209/d1 ___inv_Q_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 210/d2 ___inv_R_____, 211/d3 ___inv_S_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 212/d4 ___inv_T_____, 213/d5 ___inv_U_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 214/d6 ___inv_V_____, 215/d7 ___inv_W_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 216/d8 ___inv_X_____, 217/d9 ___inv_Y_____
        ct_mix  CT_UPPER_IDX, CT_NONE_IDX                ; 218/da ___inv_Z_____, 219/db ___inv_[_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 220/dc ___inv_\_____, 221/dd ___inv_]_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 222/de ___inv_^_____, 223/df _inv_UNDRLIN_

        ct_mix  CT_NONE_IDX, CT_LOWER_XDIGIT_IDX         ; 224/e0 _inv_diamond_, 225/e1 ___inv_a_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_XDIGIT_IDX ; 226/e2 ___inv_b_____, 227/e3 ___inv_c_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_XDIGIT_IDX ; 228/e4 ___inv_d_____, 229/e5 ___inv_e_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_IDX        ; 230/e6 ___inv_f_____, 231/e7 ___inv_g_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 232/e8 ___inv_h_____, 233/e9 ___inv_i_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 234/ea ___inv_j_____, 235/eb ___inv_k_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 236/ec ___inv_l_____, 237/ed ___inv_m_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 238/ee ___inv_n_____, 239/ef ___inv_o_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 240/f0 ___inv_p_____, 241/f1 ___inv_q_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 242/f2 ___inv_r_____, 243/f3 ___inv_s_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 244/f4 ___inv_t_____, 245/f5 ___inv_u_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 246/f6 ___inv_v_____, 247/f7 ___inv_w_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 248/f8 ___inv_x_____, 249/f9 ___inv_y_____
        ct_mix  CT_LOWER_IDX, CT_NONE_IDX                ; 250/fa ___inv_z_____, 251/fb __inv_spade__
        ct_mix  CT_NONE_IDX, CT_CTRL_IDX                 ; 252/fc __inv_v_line_, 253/fd ____BEEP_____
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 254/fe ____DELBS____, 255/ff ___INSERT____
