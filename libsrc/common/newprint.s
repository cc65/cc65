;========================================================================
;    core printf engine, used by printf, sprintf, fprintf, snprintf...
;
;    rewrite by pm100
;    original code by Ullrich von Bassewitz
;========================================================================

        .include        "zeropage.inc"
        .export         __printf

        .import         popax, pushax, pusheax, decsp6, push1, axlong, axulong
        .import         _ltoa, _ultoa
        .import         _strlower, _strlen

        .macpack        generic
        .macpack        longbranch
        .macpack        uint16
; ----------------------------------------------------------------------------
; We will store variables into the register bank in the zeropage. Define
; equates for these variables.

ArgList         = regbank+0             ; Argument list pointer
Format          = regbank+2             ; Format string
OutData         = regbank+4             ; Function parameters

; ----------------------------------------------------------------------------
; Other zero page cells

FSave           = ptr4
FCount          = ptr3
FChar           = tmp1


;==============================================================
;                       Process one field
;  Enter here with the format string pointer pointing to the
;  the char after % character, FChar contains '%'
;==============================================================
DoField:
        ; clear 'locals'
        lda     #0
        ldx     #FormatVarSize-1
@L1:    sta     FormatVars,x
        dex
        bpl     @L1
; set up the result string pointer.
; alway points to the numeric conversion buffer  
; except when we process a %s field
        p16_asg Str, Buf        


;===================
; Flags characters
;===================
        ; while(1)
        ;       fchar = *format_ptr++;
NextFlag:
        jsr     NextFChar
        ;       switch (fchar) {
        ;       case '-':
        cmp    #'-'
        bne    @L2                
        ;               left_align = 1;
        lda    #$ff
        sta    LeftJust
        jmp    NextFlag
        ;               break;
        ;       case '+':
 @L2:   cmp    #'+'
        bne    @L3
        ;               add_sign = 1;
        lda    #$ff
        sta    AddSign
        jmp    NextFlag
        ;               break;
        ;       case '0':
 @L3:   cmp    #'0'
        ;               pad_zero = 1;
        bne    @L4
        lda    #$ff
        sta    PadZero
        jmp    NextFlag
        ;               break;
        ;       case ' ':
@L4:    cmp    #' '
        ;               space_for_plus = 1;
        bne    @L5
        lda    #$ff
        sta    AddBlank
        jmp    NextFlag
        ;               break;
        ;       case '#':altform = 1;
@L5:    cmp    #'#'
        bne    @L6
        lda    #$ff
        sta    AltForm
        jmp    NextFlag
        ;               break;
        ;       default:
        ;               --format_ptr;
@L6:    
        ;u16_dec Format
        ;               goto width;
        ;}

;=======================
;       Width 
;=======================
        
        ;if (*format_ptr == '*') {
        lda     FChar
        cmp     #'*'
        bne     InlineWidth
        ;       width = va_arg(*args, int);
        jsr     GetIntArg
        sta     Width
        stx     Width+1
        ;       if (width < 0) {
        bit     Width +1
        bpl     @L1   
        ;               left_align = 1;
        lda     #1
        sta     LeftJust
        ;               width = -width;
        u16_asg Pad, #0 ;; borrow Pad for negation
        u16_sub Pad, Width
        u16_asg Width, Pad
        ;       }
        ;       ++format_ptr;
@L1:    jsr     NextFChar
        jmp GetPrecision
        ; }
InlineWidth:
        ;else {
        ;       width = strtol(format_ptr, (char**)&format_ptr, 10);
        ;}
        
        jsr     ReadInt
        sta     Width
        stx     Width+1 
  


;====================        
;      precision
;====================
GetPrecision:
        lda     FChar
        ;if (fchar == '.') {
        cmp     #'.'
        bne     SizeSkip
        ;       ++format_ptr;
        jsr     NextFChar
        ;       precision_set = 1;
        lda     #1
        sta     PrecisionSet
        ;       if (*format_ptr == '*') {
        lda     FChar
        cmp     #'*'
        bne     @L2
        ;               precision = va_arg(*args, int);
        jsr     GetIntArg
        sta     Precision
        stx     Precision+1
        bit     Precision+1
        bpl     @L3
        ;               if (precision < 0) {
        ;                       precision_set = 0;
        ;               }
        lda     #0
        sta     PrecisionSet
        ;               ++format_ptr;
        @L3:    jsr     NextFChar
        jmp     SizeSkip
        ;       }
        ;       else {
               ;                precision = strtol(format_ptr, (char**)&format_ptr, 10);
@L2:    jsr     ReadInt
        sta     Precision
        stx     Precision+1
        lda     FChar
        jmp     SizeSkip
        ;       }
        ; }  
;====================
;   size qualifiers 
;====================
ReadSize:    
        ; while (1) {
        ;       fchar = *format_ptr;
        ;       ++format_ptr;
        jsr     NextFChar
        ;       switch (fchar) {
        ;               case 'h':
SizeSkip: ; entry when we already have next format char loaded
        cmp     #'h'
        bne     @Ll
        ;                       if (hmod) {
        ;                               hhmod = 1;
        ;                       }
        ;                       hmod = 1;
        ;                       break;
        ldx     #$ff
        lda     HMod
        beq     @Lh
        stx     HHMod
@Lh: 
        stx     HMod
        jmp     ReadSize
        ;               case 'l':
@Ll:    cmp     #'l'
        beq     @Ll2
        ;               case 'L':
               ;                        if (lmod) {
        ;                               llmod = 1;
        ;                       }
        ;                       lmod = 1;
        ;                       break;
        cmp     #'L'
        bne     @Lj
@Ll2:
        ldx     #$ff
        lda     LMod
        beq     @Ll3
        stx     LLMod
@Ll3: 
        stx     LMod
        stx     IsLong
        jmp     ReadSize
        ;               case 'j':
@Lj:    cmp #'j'
        bne     @Lz
        ;                       jmod = 1;
        ;                       break;
        ldx     #$ff
        stx     JMod
        stx     IsLong
        jmp     ReadSize
        ;               case 'z':
@Lz:
        cmp     #'z'
        bne     @Lt
        ;                       zmod = 1;
        ;                       break;
        ldx     #1
        stx     ZMod
        jmp     ReadSize
        ;               case 't':
@Lt:
        cmp     #'t'
        bne     @Ldef
        ldx     #1
        ;                       tmod = 1;
        ;                       break;
        stx     TMod
        jmp     ReadSize
        ;               default:
@Ldef:  ;
        ;                       --format_ptr;
        ;                       goto type;
        ;       }
        ; }

;================================
;       Types (d,i,u,o,x....)
;================================

        lda     FChar
        ; fchar = *format_ptr;
        ; switch (fchar) {
        ;       case 'd':
        cmp     #'d'
        beq      @Ld
        ;       case 'i': {
        cmp     #'i'
        bne     @Lu
@Ld:
        ;               do_number(args, 10);
        lda     #10 
        sta     Base
        jsr     DoNumber
        jmp     DoFormat
        ;               break;
        ;       }
        ;       case 'u': {
@Lu:    cmp     #'u'
        bne     @Lo
        ;               do_unumber(args, 10);
        lda     #10 
        sta     Base
        jsr     DoUNumber
        jmp     DoFormat
        ;               break;
        ;       }
        ;       case 'o': {
@Lo:    cmp     #'o'
        bne     @Lx
        ;               do_unumber(args, 8);
        lda     #8
        sta     Base
        jsr     DoUNumber
        ;               break;
        jmp     DoFormat
        ;       }
        ;       case 'x': {
@Lx:    cmp     #'x'
        bne     @LX
        lda     #16
        sta     Base
        ;               if (do_unumber(args, 16) && altform) first_char = 'x';
        jsr     DoUNumber
        ;  ultoa returns upper case hex, lower case it
        lda     Str
        ldx     Str+1
        jsr     _strlower  
        lda     NotZero ; set in DoUNumber
        jeq     DoFormat
        lda     AltForm
        jeq     DoFormat
        lda     #'x'
        sta     FirstChar
        sta     IsHex ; remember we are doing hex
        jmp     DoFormat
        ;               break;
        ;       }
        ;       case 'X': {
@LX:    cmp     #'X'
        bne     @Lc
@Ptr:   lda     #16
        sta     Base
        ;               int i;
        ;               if (do_unumber(args, 16) && altform) first_char = 'X';
        jsr     DoUNumber
        lda     NotZero
        jeq     DoFormat
        lda     AltForm
        jeq     DoFormat
        lda     #'X'
        sta     FirstChar
        sta     IsHex
        ;               for (i = 0; conv_buff[i]; i++) {
        ;                       conv_buff[i] = toupper(conv_buff[i]);
        ;               }
 
        ;               break;
        jmp     DoFormat
        ;       }
;=====================================
;  floating point not (yet) suported
;=====================================
        ;       case 'f':
        ;               break;
        ;       case 'F':
        ;               break;
        ;       case 'e':
        ;               break;
        ;       case 'E':
        ;               break;
        ;       case 'g':
        ;               break;
        ;       case 'G':
        ;               break;
        ;       case 'a':
        ;               break;
        ;       case 'A':
        ;               break;
        ;       case 'c':
@Lc:    cmp     #'c'
        bne     @Ls
        ;               add_sign = 0;
        lda     #0
        sta     AddSign
        ;               space_for_plus = 0;
        sta     AddBlank
        ;               precision_set = 0;
        sta     PrecisionSet
        ;               conv_buff[0] = va_arg(*args, int);
        ;               conv_buff[1] = 0;
        jsr     GetIntArg               ; Get the argument (promoted to int)
        sta     Buf   
        lda     #0
        sta     Buf+1
        ;               break;
        jmp     DoFormat


        ;       case 's':
@Ls:    cmp     #'s'
        bne     @Lp
        ;               add_sign = 0;
        lda     #0
        sta     AddSign
        ;               space_for_plus = 0;
        sta     AddBlank        
        jsr     GetIntArg
        sta     Str
        stx     Str+1
        jmp     DoFormat
        ;               str = va_arg(*args, char*);
        ;               break;
        ;       case 'p': {
@Lp:    cmp     #'p'
        bne     @Ln
        ;               void* ptr = va_arg(*args, char*);
       
        ;               add_sign = 0;
        ldx     #0
        stx     AddSign
        ;               space_for_plus = 0;
        stx     AddBlank
                lda     #16
        sta     Base
        jsr     DoUNumber
        jmp     DoFormat
                ;               precision_set = 1;
        ;               precision = 16;

        ;               break;
        ;       }
        ;       case 'n':
@Ln:    cmp     #'n'
        bne     @Lpct
        lda     #0
        tay
        ; current outchar count is at OutData[0..1]
        lda     (OutData),y
        sta     Width ; borrow width to temp store count
        iny
        lda     (OutData),y
        sta     Width+1
        lda     #0
        sta     IsLong ; clear IsLong, we are reading a pointer
        jsr     GetUnsignedArg
        sta     ptr2     ; pointer to variable
        stx     ptr2+1   ; to receive the count
      
        lda #0
        tay
        lda     Width   ; always store lsb first
        sta     (ptr2),y
        bit     HHMod
        bmi     @Endn       ; thats all folks
        iny
        lda     Width+1   ; now either 2 or 4 bytes
        sta     (ptr2),y  ; store byte 2
        bit     LMod      ; 4 bytes?
        bpl     @Endn

        lda     #0         ; 4 bytes, store 0 into bytes 3 & 4
        iny
        sta     (ptr2),y
        iny
        sta     (ptr2),y

@Endn:  rts
        ;               break;
        ;       case '%':
        @Lpct:  cmp     #'%'
        bne     DoFormat
        ;               conv_buff[0] = '%';
        sta     Buf
        ;               conv_buff[1] = 0;
        lda     #0
        sta     Buf+1
        jmp     DoFormat

        ;               break;
        ;       default:
        ;               break;
        ; }

;==============================================================
; when we get here Str points to the output string
; Str points at Buf which has the converted number
; except for %s, in that case Str is loaded with the
; the pointer passed as argument to sprintf
;==============================================================

DoFormat:

; ====================
; determine first char
;=====================
        ;if (first_char == '-') {
        lda     FirstChar
        cmp     #'-'
        beq     StrLen
        ;       // taken care of already
        ;}
        ;else if (add_sign) {
        lda     AddSign
        beq     @L1
        ;       first_char = '+';
        lda     #'+'
        sta     FirstChar
        jmp     StrLen
        ;}
        ;else if (space_for_plus) {
@L1:    lda     AddBlank
        beq     StrLen
        ;               first_char = ' ';
        lda     #' '
        sta     FirstChar
        ;}
        ; slen = (int)strlen(str);


StrLen:
        lda     Str
        ldx     Str+1
        jsr     _strlen
        sta     SLen
        stx     SLen+1
; specifc case. if string length is zero and we are 
; printing a number output one \0 character
        ; if (slen == 0 && str == conv_buff) {
        lda     Str
        cmp     #<Buf
        bne     td111
        lda     Str+1
        cmp     #>Buf
        bne     td111
        u16_isz SLen
        bne     td111
        ;       slen = 1;
        lda     #1
        sta     SLen
td111:
        ; }

        ;arglen = slen;
        u16_asg ArgLen, SLen
        ; if (!is_number) {
        lda     IsNumber
        bne     @L1
        ;       if (precision_set) {
        lda     PrecisionSet
        jeq     CalcPadding
        ;               if (precision < slen) {
        u16_gt  SLen, Precision
        jeq     CalcPadding
        ;                       slen = precision;
        u16_asg SLen, Precision
        ;                       arglen = slen;
        u16_asg ArgLen, SLen
        jmp     CalcPadding
        ;               }
        ;       }
        ; }
        ; else {
@L1:   
       
        ;       //If precision is specified as 0, and the value to be converted is 0, the result is no characters output, as shown in this example:
        ;       //printf( "%.0d", 0 ); /* No characters output */

        ;       if (slen == 1 && str[0] == '0' && precision_set && precision == 0) {
        u16_eq  SLen, #1
        bne     @L2
        lda     Buf
        cmp     #'0'
        bne     @L2
        lda     PrecisionSet    
        beq     @L2
        u16_isz Precision
        bne     @L2
        u16_asg SLen, #0
        u16_asg ArgLen, #0
        jmp     CalcPadding

        ;               slen = 0;
        ;               arglen = 0;
        ;       }
        ;       else {
        ;               arglen = MAX(slen, (int)precision);
@L2:
        u16_gt  SLen, Precision
        beq     @L3
        u16_asg ArgLen, SLen
        jmp     CalcPadding
@L3:    u16_asg ArgLen, Precision

        ;               zpad = precision - slen;
        u16_asg ZPad, Precision
        u16_sub ZPad, SLen
        ;       }
        ; }
CalcPadding:

        ; pad = arglen < width ? width - arglen : 0;
        u16_gt  Width, ArgLen
        beq     @L1
        u16_asg Pad, Width
        u16_sub Pad, ArgLen
        jmp     @L2
@L1:    u16_asg Pad, #0
        ; pad_char = pad_zero ? '0' : ' ';
@L2:    lda     #' '
        sta     PadChar
        lda     PadZero
        beq     @L3
        lda     #'0'
        sta     PadChar
@L3:
        ; if (left_align && !altform) {
        lda     LeftJust
        beq     EmitFirstCharLeft
        lda     AltForm
        bne     EmitFirstCharLeft
        ;       pad_char = ' ';
        lda     #' '
        sta     PadChar

        ; }
EmitFirstCharLeft:
        ; if (first_char) {
        lda     FirstChar
        beq     NoFirstChar
        ;       if (pad_char == '0') {
        ldx     PadChar
        cpx     #'0'
        bne     Not0
        ;  
        ;               if (first_char == 'x' || first_char == 'X') {
        lda     IsHex
        beq     @L1
        ;                       *outb_ptr = '0';
        ;                       outb_ptr++;
        lda    #'0'
        jsr     Output1
        ;                       pad--
        u16_dec Pad          ; one less pad char for the xor X
        ;       }
        ;else{
        ;       *outb_ptr = first_char;
        ;       outb_ptr++;
@L1:    lda     FirstChar
        jsr     Output1
        lda     #0
        sta     FirstChar
        u16_dec Pad
        jmp     NoFirstChar
        ; }
Not0:
        ; else if (first_char == 'x' || first_char == 'X') {
        lda     IsHex
        beq     @L1
        ;               pad--;
        ;               pad--;
        u16_dec Pad
        ;       }
        ;       else
        ;               pad--;
@L1:
        u16_dec Pad

        ; }
NoFirstChar:

;==============================
;  emit left padding
;  spaces first
;==============================
        ;if (!left_align) {
        lda     LeftJust
        ;       for ( i = 0; i < pad; i++) {
        bne     @L1
        u16_asg I, Pad
        jsr     PadOut

@L1:
;=================
;  now 0 or 0x
;=================
        ; if (first_char) {
        lda     FirstChar
        beq     NoFC
        ;       if (first_char == 'x' || first_char == 'X') {
        lda     IsHex
        beq     @L2
        lda     #'0'
        jsr     Output1
        ;               *outb_ptr = '0';
        ;               outb_ptr++;

        ;       }
        ;       *outb_ptr = first_char;
        ;       outb_ptr++;
@L2:    lda     FirstChar
        jsr     Output1
NoFC:
        ; }

        ;========================
        ; now zero padding
        ;========================
        
        lda     PadChar ; PadOut reads PadChar, so we need to set it    
        pha             ; remeber old PadChar
        ; for (i = 0; i < zpad; i++) {
        u16_asg I, ZPad
        lda     #'0'
        sta     PadChar
        jsr     PadOut
        ;       *outb_ptr = '0';
        ;       outb_ptr++;
        ; }
        pla             ; restore padchar
        sta     PadChar

; ==============================
;  now the actual field
; ==============================
        ; for (i = 0; i < slen; i++) {
        ;       *outb_ptr = *str;
        ;       outb_ptr++;
        ;       str++;
        ; }
        jsr     PushOutData
        lda     Str
        ldx     Str+1
        jsr     pushax
        lda     SLen
        ldx     SLen+1
        jsr     pushax
        jsr     CallOutFunc  
        
;==============================
; now , finally, right padding
;==============================        
        ; if (left_align)
        lda     LeftJust
        beq     AllDone
        u16_asg I, Pad
        jsr     PadOut
AllDone:
        ;       for (i = 0; i < pad; i++) {
        ;               *outb_ptr = pad_char;
        ;               outb_ptr++;
        ;       }
       rts


;==============================================================
;   Process one signed number
;==============================================================


DoNumber:
;       is_number = 1;
        lda     #$ff
        sta     IsNumber
;       if (pad_zero) pad_zero = !precision_set;
        lda     PadZero
        beq     @L1
        lda     PrecisionSet
        beq     @L1
        lda     #0
        sta     PadZero
@L1:
;       intmax_t v = va_arg(*args, intmax_t);
        jsr     GetSignedArg

;       if (hhmod) {
        bit     HHMod   ; truncate if 1 byte arg
        beq     @L2
        ldx     #0      ; either set high bytes to 0
        bit     sreg+1  ; or 0xff if negative
        bpl     @Pos
        dex 
@Pos:   
        stx     sreg
        stx     sreg+1
             
        
;               char cv = (char)v;
;               itoa(cv, conv_buff, base);
;       }
;       else if (hmod) {
;               short cv = (short)v;
;               itoa(cv, conv_buff, base);
;       }
;       else if (llmod) {
;               //long long cv = (long long)v;
;               //i64toa(cv, conv_buff, base); // TODO
;       }
;       else if (lmod) {
;               long cv = (long)v;
;               ltoa(cv, conv_buff, base);
;       }

;       else {
;               itoa(v, conv_buff, base);
  @L2:
        jsr     ltoa
;       }
;       if (str[0] == '-') {
;--------------------------------------
; if we get a minus sign we need some
; juggling. Move it to FirstChar
; same as for leading '+' sign
; then shunt the Str pointer up by one 
; char to omit the '-' the we might
; not want to output before the first 
; digit
;--------------------------------------
        ldx     Buf
        cpx     #'-'
        bne     @L3
        u16_inc Str
        lda     #'-'
        sta    FirstChar
;               str++;
;               first_char = '-';
;       }
@L3:        
        rts
; }

;--------------------------------------------------------
; Process one unsigned number
;--------------------------------------------------------

DoUNumber:
;       uintmax_t v;
;       is_number = 1;
        lda     #$ff
        sta     IsNumber
        lda     #0
;       add_sign = 0;
        sta     AddSign
;       space_for_plus = 0;
        sta     AddBlank
;       if (pad_zero) pad_zero = !precision_set;
        lda     PadZero
        beq     @L1
        lda     PrecisionSet
        beq     @L1
        lda     #0
        sta     PadZero
@L1:

;       v= va_arg(*args, uintmax_t);
        jsr GetUnsignedArg
;============================
; big special case for octal
;============================
;       if (v > 0 && altform && base == 8) {
        cmp     #0
        jne     @L2
        cpx     #0
        jne     @L2
        bit     sreg
        bne     @L2
        bit     sreg+1
        bne     @L2
        jmp     NotOctal
@L2:
; here v > 0 (unsigned)
        ldy     #1
        sty     NotZero ; remeber !=0
        bit     AltForm
        bpl     NotOctal
        ldy     Base
        cpy     #8
        bne     NotOctal
        pha
        lda     #'0'
        sta     Buf
        u16_inc Str     ; we want ultoa output starting at second byte
        pla
        jsr     ultoa
        u16_dec Str     ; restore Str
        rts

;               str[0] = '0';
;               str++;
;       }
;       if (hhmod) {
NotOctal:
        bit     HHMod
        beq     @L2
        ldx     #0    ; simple truncate (no sign)
        stx     sreg
        stx     sreg+1
;               unsigned char cv = (unsigned char)v;
;               ultoa(cv, str, base);
;       }
;       else if (hmod) {
;               unsigned short cv = (unsigned short)v;
;               ultoa(cv, str, base);
;       }
;       else if (llmod) {
;               //unsigned long long cv = (unsigned long long)v;
;               //ui64toa(cv, str, base); // TODO
;       }
;       else if (lmod) {
;               unsigned long cv = (unsigned long)v;
;               ultoa(cv, str, base);
;       }
;       else {
;               ultoa(v, str, base);
 @L2:
        jsr    ultoa
;       }
;       str = conv_buff;
;       return v > 0;
        rts
; }
;==================================================================
;                         Entry point
;==================================================================

__printf:
        jsr     Initialize
        u16_asg FSave, Format           ; remeber start of non format chars
FindNextField:



        jsr     NextFChar
        beq     Out
        cmp     #'%'
        bne     FindNextField
        jsr     Flush                   ; flush what we have so far
        jsr     DoField                 ; process the field
        u16_asg FSave, Format           ; start the next run of non format chars
        jmp     FindNextField
Out:
        ; we are done
        jsr     Flush
        jmp     CleanAndExit

;=======================================================
;      Helper routines
;=======================================================


; FChar contains the current format string character
; Format points at the next character
;  ie NextFChar does FChar = *Format++

NextFChar:
        ldy     #0
        lda     (Format),y
        sta     FChar
        u16_inc Format  
        lda     FChar
        rts

;=====================================
; output padding characters
; count is in I
; character is in PadChar
; checks for count > 0
;=====================================

PadOut:
        u16_isz I
        beq     PadDone
        bit     I
        bmi     PadDone
PadLoop:
        lda     PadChar
        jsr     Output1
        u16_dec I
        bne     PadLoop
PadDone:
        rts

;=======================================================
; flush what we have so far from non field chars     
;  start output is FSave
;  count = FSave - Format - 1 (format points at next char)
;=======================================================
Flush:
        u16_asg FCount, Format
        u16_sub FCount, FSave
        u16_dec FCount ; we dont want the last char read (% or \0)
        u16_isz FCount
        beq     FlushDone
        jsr     PushOutData

        lda     FSave
        ldx     FSave+1
        jsr     pushax

        lda     FCount
        ldx     FCount+1
        jsr     pushax
        jsr     CallOutFunc  
FlushDone:
        rts


;=======================================================
;  Original helper functions are all here
;=======================================================

;
; Ullrich von Bassewitz, 2000-10-21
;


;---------------
; Initlialize 
;---------------
Initialize:
        ; Save the register bank variables into the save area

        pha                             ; Save low byte of ap
        ldy     #5
Save:   lda     regbank,y
        sta     RegSave,y
        dey
        bpl     Save
        pla                             ; Restore low byte of ap

; Get the parameters from the stack

        sta     ArgList                 ; Argument list pointer
        stx     ArgList+1

        jsr     popax                   ; Format string
        sta     Format
        stx     Format+1

        jsr     popax                   ; Output descriptor
        sta     OutData
        stx     OutData+1

; Initialize the output counter in the output descriptor to zero

        lda     #0
        tay
        sta     (OutData),y
        iny
        sta     (OutData),y

; Get the output function from the output descriptor and remember it

        iny
        lda     (OutData),y
        sta     CallOutFunc+1
        iny
        lda     (OutData),y
        sta     CallOutFunc+2
        rts

;=========================================

PushOutData:
        lda     OutData
        ldx     OutData+1
        jmp     pushax
;--------------------
; Clean up and return
;--------------------
CleanAndExit:
        ldx     #5
Rest:   lda     RegSave,x
        sta     regbank,x
        dex
        bpl     Rest
        rts

; ----------------------------------------------------------------------------
; Call the output function with one character in A

Output1:
        sta     CharArg
        jsr     PushOutData
        lda     #<CharArg
        ldx     #>CharArg
        jsr     pushax
        jsr     push1
        jmp     CallOutFunc     ; fout (OutData, &CharArg, 1)
; ----------------------------------------------------------------------------
; Read an integer from the format string. Will return zero in .Y.

ReadInt:
        u16_dec Format
        ldy     #0
        sty     ptr1
        sty     ptr1+1                  ; Start with zero
@Loop:  
        jsr     NextFChar
        sub     #'0'                    ; Make number from ascii digit
        bcc     @L9                     ; Jump if done
        cmp     #9+1
        bcs     @L9                     ; Jump if done

; Skip the digit character
     ;   pha
      ;  jsr     NextFChar
      ;  pla

; Add the digit to the value we have in ptr1
        pha
        lda     ptr1
        ldx     ptr1+1
        asl     ptr1
        rol     ptr1+1                  ; * 2
        asl     ptr1
        rol     ptr1+1                  ; * 4, assume carry clear
        adc     ptr1
        sta     ptr1
        txa
        adc     ptr1+1
        sta     ptr1+1                  ; * 5
        asl     ptr1
        rol     ptr1+1                  ; * 10, assume carry clear
        pla
        adc     ptr1                    ; Add digit value
        sta     ptr1
        bcc     @Loop
        inc     ptr1+1
        bcs     @Loop                   ; Branch always

; We're done converting

@L9:    lda     ptr1
        ldx     ptr1+1                  ; Load result
        rts
; ----------------------------------------------------------------------------

; ----------------------------------------------------------------------------
; Get an unsigned int or long argument depending on the IsLong flag.

GetUnsignedArg:
        lda     IsLong                  ; Check flag
        bne     GetLongArg              ; Long sets all
        jsr     GetIntArg               ; Get an integer argument
        jmp     axulong                 ; Convert to unsigned long

; ----------------------------------------------------------------------------
; Get an signed int or long argument depending on the IsLong flag.

GetSignedArg:
        lda     IsLong                  ; Check flag
        bne     GetLongArg              ; Long sets all
        jsr     GetIntArg               ; Get an integer argument
        jmp     axlong                  ; Convert to long

; ----------------------------------------------------------------------------
; Get a long argument from the argument list. Returns 0 in .Y.
 
GetLongArg:
        jsr     GetIntArg               ; Get high word
        sta     sreg
        stx     sreg+1

; Run into GetIntArg fetching the low word

; ----------------------------------------------------------------------------
; Get an integer argument from the argument list. Returns 0 in .Y.

GetIntArg:
        u16_sub ArgList, #2
        ldy     #1
        lda     (ArgList),y
        tax
        dey
        lda     (ArgList),y
        rts

; ltoa: Wrapper for _ltoa that pushes all arguments

ltoa:   
        jsr     pusheax                 ; Push value
        lda     #<Buf              ; Push the buffer pointer...
        ldx     #>Buf
        jsr     pushax
        lda     Base                    ; Restore base
        jmp     _ltoa                   ; ultoa (l, s, base);


; ----------------------------------------------------------------------------
; ultoa: Wrapper for _ultoa that pushes all arguments

ultoa: 
        jsr     pusheax                 ; Push value
        lda Str
        ldx Str+1          ; Push the buffer pointer...
        jsr     pushax
        lda     Base                    ; Restore base
        jmp     _ultoa                  ; ultoa (l, s, base);


; ----------------------------------------------------------------------------
;

; Local data (all static)

.bss

; Save area for the zero page registers
RegSave:        .res    regbanksize

; One character argument for OutFunc
CharArg:        .byte   0

; Format variables
FormatVars:
LeftJust:       .byte   0
Base:           .byte   0
AddSign:        .byte   0
AddBlank:       .byte   0
PadZero:        .byte   0
Pad:            .word   0
ZPad:           .word   0
AltForm:        .byte   0
PadChar:        .byte   0
Width:          .word   0
Precision:      .word   0
PrecisionSet:   .byte   0
IsNumber:       .byte   0
IsLong:         .byte   0
HMod:           .byte   0
HHMod:          .byte   0
LMod:           .byte   0
LLMod:          .byte   0
JMod:           .byte   0
ZMod:           .byte   0
TMod:           .byte   0
IsHex:          .byte   0
NotZero:        .byte   0       
FirstChar:      .byte   0
FormatVarSize   = * - FormatVars

; Argument buffer and pointer
Buf:            .res    20
Str:            .word   0
ArgLen:         .res    2
SLen:           .res    2
I:              .word   0

.data
CallOutFunc:    jmp     $0000