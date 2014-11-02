;-----------------------------------------------------------
; Y2K FIX by Alan Davis, Dennis Debro, and Ronen Habot
;-----------------------------------------------------------
Y2K     LDY     #$00            ; Copy BIOS opening screen to RAM
        LDA     #$FD
        STA     TEMPH
        LDA     #$58            ; Assume 2 port system
        LDX     $FD32
        CPX     #$E8            ; Is this a 4 port?
        BNE     Y2K0            ; Jump if not
        LDA     #$42            ; Yes, 4 port system
Y2K0    STA     TEMPL           
Y2K1    LDA     (TEMPL),Y       
        STA     $0600,Y
        INY     
        BNE     Y2K1
        LDY     #$50
        INC     TEMPH
Y2K2    LDA     (TEMPL),Y       
        STA     $0700,Y
        DEY     
        BPL     Y2K2
        LDA     #$D4            ; Point to copyright string
        STA     $0724
        LDA     #$BF
        STA     $0725
        LDX     #$0B            ; Store NOP's @ end
        LDA     #$EA
Y2K3    STA     $0732,X         
        DEX     
        BPL     Y2K3
        LDA     #$60            ; Store RTS opcode @ end
        STA     $0750
        JSR     $0600           ; Show title screen
        LDY     #$00            ; Clear RAM from $0600-$3FFF
        STY     $80
        LDA     #$06
        STA     $81
        JSR     CLRRAM
        RTS     

