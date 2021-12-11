; ctype.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; CBM character specification table.
;
; This table is taken from Craig S. Bruce's technical docs. for the ACE OS.

        .include        "ctypetable.inc"
        .export         __ctypeidx
        
; The tables are readonly, put them into the rodata segment

.rodata

__ctypeidx:
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;   0/00 ___rvs_@___, 1/01 ___rvs_a___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;   2/02 ___rvs_b___, 3/03 ___rvs_c___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;   4/04 ___rvs_d___, 5/05 ___rvs_e___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;   6/06 ___rvs_f___, 7/07 _BEL/rvs_g_
        ct_mix  CT_CTRL_IDX, CT_CTRL_WS_SPACETAB_IDX     ;   8/08 ___rvs_h___, 9/09 _TAB/rvs_i_
        ct_mix  CT_CTRL_WS_IDX, CT_CTRL_IDX              ;  10/0a _BOL/rvs_j_, 11/0b ___rvs_k___
        ct_mix  CT_CTRL_IDX, CT_CTRL_WS_IDX              ;  12/0c ___rvs_l___, 13/0d _CR_/rvs_m_
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  14/0e ___rvs_n___, 15/0f ___rvs_o___
        ct_mix  CT_CTRL_IDX, CT_CTRL_WS_IDX              ;  16/10 ___rvs_p___, 17/11 _VT_/rvs_q_
        ct_mix  CT_CTRL_IDX, CT_CTRL_WS_IDX              ;  18/12 ___rvs_r___, 19/13 HOME/rvs_s_
        ct_mix  CT_CTRL_WS_IDX, CT_CTRL_IDX              ;  20/14 _BS_/rvs_t_, 21/15 ___rvs_u___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  22/16 ___rvs_v___, 23/17 ___rvs_w___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  24/18 ___rvs_x___, 25/19 ___rvs_y___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  26/1a ___rvs_z___, 27/1b ___rvs_[___
        ct_mix  CT_CTRL_IDX, CT_CTRL_WS_IDX              ;  28/1c ___rvs_\___, 29/1d cursr-right
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ;  30/1e ___rvs_^___, 31/1f _rvs_under_

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

        ct_mix  CT_NONE_IDX, CT_LOWER_XDIGIT_IDX         ;  64/40 _____@_____, 65/41 _____a_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_XDIGIT_IDX ;  66/42 _____b_____, 67/43 _____c_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_XDIGIT_IDX ;  68/44 _____d_____, 69/45 _____e_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_IDX        ;  70/46 _____f_____, 71/47 _____g_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ;  72/48 _____h_____, 73/49 _____i_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ;  74/4a _____j_____, 75/4b _____k_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ;  76/4c _____l_____, 77/4d _____m_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ;  78/4e _____n_____, 79/4f _____o_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ;  80/50 _____p_____, 81/51 _____q_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ;  82/52 _____r_____, 83/53 _____s_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ;  84/54 _____t_____, 85/55 _____u_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ;  86/56 _____v_____, 87/57 _____w_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ;  88/58 _____x_____, 89/59 _____y_____
        ct_mix  CT_LOWER_IDX, CT_NONE_IDX                ;  90/5a _____z_____, 91/5b _____[_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  92/5c _____\_____, 93/5d _____]_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  94/5e _____^_____, 95/5f _UNDERLINE_

        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  96/60 _A`_grave__, 97/61 _A'_acute__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ;  98/62 _A^_circum_, 99/63 _A~_tilde__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 100/64 _A"_dieres_, 101/65 _A__ring___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 102/66 _AE________, 103/67 _C,cedilla_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 104/68 _E`_grave__, 105/69 _E'_acute__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 106/6a _E^_circum_, 107/6b _E"_dieres_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 108/6c _I`_grave__, 109/6d _I'_acute__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 110/6e _I^_circum_, 111/6f _I"_dieres_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 112/70 _D-_Eth_lr_, 113/71 _N~_tilde__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 114/72 _O`_grave__, 115/73 _O'_acute__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 116/74 _O^_circum_, 117/75 _O~_tilde__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 118/76 _O"_dieres_, 119/77 __multiply_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 120/78 _O/_slash__, 121/79 _U`_grave__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 122/7a _U'_acute__, 123/7b _U^_circum_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 124/7c _U"_dieres_, 125/7d _Y'_acute__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 126/7e _cap_thorn_, 127/7f _Es-sed_B__

        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 128/80 __bullet___, 129/81 __v_line___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 130/82 __h_line___, 131/83 ___cross___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 132/84 _tl_corner_, 133/85 _tr_corner_
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 134/86 _bl_corner_, 135/87 _br_corner_
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 136/88 ___l_tee___, 137/89 ___r_tee___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 138/8a ___t_tee___, 139/8b ___b_tee___
        ct_mix  CT_CTRL_IDX, CT_CTRL_WS_IDX              ; 140/8c ___heart___, 141/8d _CR/diamond
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 142/8e ___club____, 143/8f ___spade___
        ct_mix  CT_CTRL_IDX, CT_CTRL_WS_IDX              ; 144/90 _s_circle__, 145/91 _cursor-up_
        ct_mix  CT_CTRL_IDX, CT_CTRL_WS_IDX              ; 146/92 ___pound___, 147/93 _CLS/check_
        ct_mix  CT_CTRL_WS_IDX, CT_CTRL_IDX              ; 148/94 __INSert___, 149/95 ____+/-____
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 150/96 __divide___, 151/97 __degree___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 152/98 _c_checker_, 153/99 _f_checker_
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 154/9a _solid_sq__, 155/9b __cr_char__
        ct_mix  CT_CTRL_IDX, CT_CTRL_WS_IDX              ; 156/9c _up_arrow__, 157/9d cursor-left
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 158/9e _left_arro_, 159/9f _right_arr_

        ct_mix  CT_SPACE_SPACETAB_IDX, CT_NONE_IDX       ; 160/a0 _req space_, 161/a1 _!_invertd_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 162/a2 ___cent____, 163/a3 ___pound___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 164/a4 __currency_, 165/a5 ____yen____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 166/a6 _|_broken__, 167/a7 __section__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 168/a8 __umulaut__, 169/a9 _copyright_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 170/aa __fem_ord__, 171/ab _l_ang_quo_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 172/ac ____not____, 173/ad _syl_hyphn_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 174/ae _registerd_, 175/af _overline__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 176/b0 __degrees__, 177/b1 ____+/-____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 178/b2 _2_supersc_, 179/b3 _3_supersc_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 180/b4 ___acute___, 181/b5 ____mu_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 182/b6 _paragraph_, 183/b7 __mid_dot__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 184/b8 __cedilla__, 185/b9 _1_supersc_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 186/ba __mas_ord__, 187/bb _r_ang_quo_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 188/bc ____1/4____, 189/bd ____1/2____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 190/be ____3/4____, 191/bf _?_invertd_

        ct_mix  CT_NONE_IDX, CT_UPPER_XDIGIT_IDX         ; 192/c0 _____`_____, 193/c1 _____A_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_XDIGIT_IDX ; 194/c2 _____B_____, 195/c3 _____C_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_XDIGIT_IDX ; 196/c4 _____D_____, 197/c5 _____E_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_IDX        ; 198/c6 _____F_____, 199/c7 _____G_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 200/c8 _____H_____, 201/c9 _____I_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 202/ca _____J_____, 203/cb _____K_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 204/cc _____L_____, 205/cd _____M_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 206/ce _____N_____, 207/cf _____O_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 208/d0 _____P_____, 209/d1 _____Q_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 210/d2 _____R_____, 211/d3 _____S_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 212/d4 _____T_____, 213/d5 _____U_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 214/d6 _____V_____, 215/d7 _____W_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 216/d8 _____X_____, 217/d9 _____Y_____
        ct_mix  CT_UPPER_IDX, CT_NONE_IDX                ; 218/da _____Z_____, 219/db _____{_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 220/dc _____|_____, 221/dd _____}_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 222/de _____~_____, 223/df ___HOUSE___

        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 224/e0 _a`_grave__, 225/e1 _a'_acute__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 226/e2 _a^_circum_, 227/e3 _a~_tilde__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 228/e4 _a"_dieres_, 229/e5 _a__ring___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 230/e6 _ae________, 231/e7 _c,cedilla_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 232/e8 _e`_grave__, 233/e9 _e'_acute__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 234/ea _e^_circum_, 235/eb _e"_dieres_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 236/ec _i`_grave__, 237/ed _i'_acute__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 238/ee _i^_circum_, 239/ef _i"_dieres_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 240/f0 _o^x_Eth_s_, 241/f1 _n~_tilda__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 242/f2 _o`_grave__, 243/f3 _o'_acute__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 244/f4 _o^_circum_, 245/f5 _o~_tilde__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 246/f6 _o"_dieres_, 247/f7 __divide___
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 248/f8 _o/_slash__, 249/f9 _u`_grave__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 250/fa _u'_acute__, 251/fb _u^_circum_
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 252/fc _u"_dieres_, 253/fd _y'_acute__
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 254/fe _sm_thorn__, 255/ff _y"_dieres_
