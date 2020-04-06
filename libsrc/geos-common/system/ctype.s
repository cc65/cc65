; ctype.s
;
; This file is part of
; cc65 - a freeware C compiler for 6502 based systems
;
; https://cc65.github.io
;
; See "LICENSE" file for legal information.
;
; GEOS character specification table.
;
; Source: The Hitchhiker's Guide To GEOS
; http://lyonlabs.org/commodore/onrequest/geos-manuals/The_Hitchhikers_Guide_to_GEOS.pdf

        .include        "ctypetable.inc"
        .export         __ctypeidx

; The tables are readonly, put them into the rodata segment

.rodata

__ctypeidx:
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 0/00 ____NULL___, 1/01 ____N/A____
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 2/02 ____N/A____, 3/03 ____N/A____
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 4/04 ____N/A____, 5/05 ____N/A____
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 6/06 ____N/A____, 7/07 ____N/A____
        ct_mix  CT_CTRL_IDX, CT_CTRL_WS_SPACETAB_IDX     ; 8/08 __BAKSPACE_, 9/09 __FWDSPACE_
        ct_mix  CT_CTRL_WS_IDX, CT_CTRL_IDX              ; 10/0a _____LF____, 11/0b ____HOME___
        ct_mix  CT_CTRL_IDX, CT_CTRL_WS_IDX              ; 12/0c ___UPLINE__, 13/0d _____CR____
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 14/0e __ULINEON__, 15/0f __ULINEOFF_
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 16/10 _ESC_GRAPH_, 17/11 ____N/A____
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 18/12 ___REVON___, 19/13 ___REVOFF__
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 20/14 ___GOTOX___, 21/15 ___GOTOY___
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 22/16 ___GOTOXY__, 23/17 _NEWCRDSET_
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 24/18 ___BOLDON__, 25/19 __ITALICON_
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 26/1a _OUTLINEON_, 27/1b _PLAINTEXT_
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 28/1c ____N/A____, 29/1d ____N/A____
        ct_mix  CT_CTRL_IDX, CT_CTRL_IDX                 ; 30/1e ____N/A____, 31/1f ____N/A____

        ct_mix  CT_SPACE_SPACETAB_IDX, CT_NONE_IDX       ; 32/20 ___SPACE___, 33/21 _____!_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 34/22 _____"_____, 35/23 _____#_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 36/24 _____$_____, 37/25 _____%_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 38/26 _____&_____, 39/27 _____'_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 40/28 _____(_____, 41/29 _____)_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 42/2a _____*_____, 43/2b _____+_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 44/2c _____,_____, 45/2d _____-_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 46/2e _____._____, 47/2f _____/_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ; 48/30 _____0_____, 49/31 _____1_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ; 50/32 _____2_____, 51/33 _____3_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ; 52/34 _____4_____, 53/35 _____5_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ; 54/36 _____6_____, 55/37 _____7_____
        ct_mix  CT_DIGIT_XDIGIT_IDX, CT_DIGIT_XDIGIT_IDX ; 56/38 _____8_____, 57/39 _____9_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 58/3a _____:_____, 59/3b _____;_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 60/3c _____<_____, 61/3d _____=_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 62/3e _____>_____, 63/3f _____?_____

        ct_mix  CT_NONE_IDX, CT_UPPER_XDIGIT_IDX         ; 64/40 _____@_____, 65/41 _____A_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_XDIGIT_IDX ; 66/42 _____B_____, 67/43 _____C_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_XDIGIT_IDX ; 68/44 _____D_____, 69/45 _____E_____
        ct_mix  CT_UPPER_XDIGIT_IDX, CT_UPPER_IDX        ; 70/46 _____F_____, 71/47 _____G_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 72/48 _____H_____, 73/49 _____I_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 74/4a _____J_____, 75/4b _____K_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 76/4c _____L_____, 77/4d _____M_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 78/4e _____N_____, 79/4f _____O_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 80/50 _____P_____, 81/51 _____Q_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 82/52 _____R_____, 83/53 _____S_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 84/54 _____T_____, 85/55 _____U_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 86/56 _____V_____, 87/57 _____W_____
        ct_mix  CT_UPPER_IDX, CT_UPPER_IDX               ; 88/58 _____X_____, 89/59 _____Y_____
        ct_mix  CT_UPPER_IDX, CT_NONE_IDX                ; 90/5a _____Z_____, 91/5b _____[_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 92/5c _____\_____, 93/5d _____]_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 94/5e _____^_____, 95/5f _UNDERLINE_

        ct_mix  CT_NONE_IDX, CT_LOWER_XDIGIT_IDX         ; 96/60 _____`_____, 97/61 _____a_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_XDIGIT_IDX ; 98/62 _____b_____, 99/63 _____c_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_XDIGIT_IDX ; 100/64 _____d_____, 101/65 _____e_____
        ct_mix  CT_LOWER_XDIGIT_IDX, CT_LOWER_IDX        ; 102/66 _____f_____, 103/67 _____g_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 104/68 _____h_____, 105/69 _____i_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 106/6a _____j_____, 107/6b _____k_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 108/6c _____l_____, 109/6d _____m_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 110/6e _____n_____, 111/6f _____o_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 112/70 _____p_____, 113/71 _____q_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 114/72 _____r_____, 15/73 _____s_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 116/74 _____t_____, 117/75 _____u_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 118/76 _____v_____, 119/77 _____w_____
        ct_mix  CT_LOWER_IDX, CT_LOWER_IDX               ; 120/78 _____x_____, 121/79 _____y_____
        ct_mix  CT_LOWER_IDX, CT_NONE_IDX                ; 122/7a _____z_____, 123/7b _____{_____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 124/7c _____|_____, 125/7d _____}_____
        ct_mix  CT_NONE_IDX, CT_CTRL_IDX                 ; 126/7e _____~_____, 127/7f __USELAST__

        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 128/80 __SHORTCUT_ ,129/81 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 130/82 ____N/A____ ,131/83 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 132/84 ____N/A____ ,133/85 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 134/86 ____N/A____, 135/87 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 136/88 ____N/A____ ,137/89 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 138/8a ____N/A____ ,139/8b ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 140/8c ____N/A____, 141/8d ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 142/8e ____N/A____, 143/8f ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 144/90 ____N/A____, 145/91 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 146/92 ____N/A____, 147/93 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 148/94 ____N/A____, 149/95 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 150/96 ____N/A____, 151/97 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 152/98 ____N/A____, 153/99 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 154/9a ____N/A____, 155/9b ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 156/9c ____N/A____, 157/9d ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 158/9e ____N/A____, 159/9f ____N/A____

        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 160/a0 ____N/A____, 161/a1 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 162/a2 ____N/A____, 163/a3 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 164/a4 ____N/A____, 165/a5 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 166/a6 ____N/A____, 167/a7 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 168/a8 ____N/A____, 169/a9 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 170/aa ____N/A____, 171/ab ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 172/ac ____N/A____, 173/ad ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 174/ae ____N/A____, 175/af ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 176/b0 ____N/A____, 177/b1 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 178/b2 ____N/A____, 179/b3 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 180/b4 ____N/A____, 181/b5 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 182/b6 ____N/A____, 183/b7 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 184/b8 ____N/A____, 185/b9 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 186/ba ____N/A____, 187/bb ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 188/bc ____N/A____, 189/bd ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 190/be ____N/A____, 191/bf ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 192/c0 ____N/A____, 193/c1 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 194/c2 ____N/A____, 195/c3 ____N/A____

        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 196/c4 ____N/A____, 197/c5 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 198/c6 ____N/A____, 199/c7 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 200/c8 ____N/A____, 201/c9 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 202/ca ____N/A____, 203/cb ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 204/cc ____N/A____, 205/cd ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 206/ce ____N/A____, 207/cf ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 208/d0 ____N/A____, 209/d1 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 210/d2 ____N/A____, 211/d3 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 212/d4 ____N/A____, 213/d5 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 214/d6 ____N/A____, 215/d7 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 216/d8 ____N/A____, 217/d9 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 218/da ____N/A____, 219/db ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 220/dc ____N/A____, 221/dd ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 222/de ____N/A____, 223/df ____N/A____

        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 224/e0 ____N/A____, 225/e1 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 226/e2 ____N/A____, 227/e3 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 228/e4 ____N/A____, 229/e5 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 230/e6 ____N/A____, 231/e7 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 232/e8 ____N/A____, 233/e9 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 234/ea ____N/A____, 235/eb ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 236/ec ____N/A____, 237/ed ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 238/ee ____N/A____, 239/ef ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 240/f0 ____N/A____, 241/f1 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 242/f2 ____N/A____, 243/f3 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 244/f4 ____N/A____, 245/f5 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 246/f6 ____N/A____, 247/f7 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 248/f8 ____N/A____, 249/f9 ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 250/fa ____N/A____, 251/fb ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 252/fc ____N/A____, 253/fd ____N/A____
        ct_mix  CT_NONE_IDX, CT_NONE_IDX                 ; 254/fe ____N/A____, 255/ff ____N/A____
