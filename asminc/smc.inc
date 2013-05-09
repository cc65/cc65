; smc.mac
; ca65 Macro-Pack for Self Modifying Code (SMC)
;
; (c) Christian Krüger, latest change: 09-Nov-2011
;
; This software is provided 'as-is', without any expressed or implied
; warranty.  In no event will the authors be held liable for any damages
; arising from the use of this software.
;
; Permission is granted to anyone to use this software for any purpose,
; including commercial applications, and to alter it and redistribute it
; freely, subject to the following restrictions:
;
; 1. The origin of this software must not be misrepresented; you must not
;    claim that you wrote the original software. If you use this software
;    in a product, an acknowledgment in the product documentation would be
;    appreciated but is not required.
; 2. Altered source versions must be plainly marked as such, and must not
;    be misrepresented as being the original software.
; 3. This notice may not be removed or altered from any source
;    distribution.
;

.define _SMCDesignator  .mid(0, .tcount(label) - 1, label) .ident(.concat(.string(.right(1, label)), "_SMC"))
.define _SMCAlias       .mid(0, .tcount(alias) - 1, alias) .ident(.concat(.string(.right(1, alias)), "_SMC"))
.define SMC_AbsAdr      $FADE
.define SMC_ZpAdr       $00
.define SMC_Opcode      nop
.define SMC_Value       $42

.macro SMC_OperateOnValue opcode, label
        opcode _SMCDesignator+1
.endmacro

.macro SMC_OperateOnLowByte opcode, label
        SMC_OperateOnValue opcode, label
.endmacro

.macro SMC_OperateOnHighByte opcode, label
        opcode _SMCDesignator + 2
.endmacro

.macro SMC_Import alias
.import _SMCAlias
.endmacro

.macro SMC_Export alias, label
.export _SMCAlias := _SMCDesignator
.endmacro

.macro  SMC     label, statement
_SMCDesignator: statement
.endmacro

.macro SMC_TransferOpcode       label, opcode, register
.if .paramcount = 2 .or .match ({register}, a)
        lda #opcode
        sta _SMCDesignator
.elseif .match ({register}, x)
        ldx #opcode
        stx _SMCDesignator
.elseif .match ({register}, y)
        ldy #opcode
        sty _SMCDesignator
.endif
.endmacro

.macro SMC_LoadOpcode   label, register
.if .paramcount = 1 .or .match ({register}, a)
        lda _SMCDesignator
.elseif .match ({register}, x)
        ldx _SMCDesignator
.elseif .match ({register}, y)
        ldy _SMCDesignator
.endif
.endmacro

.macro SMC_StoreOpcode  label, register
.if .paramcount = 1 .or .match ({register}, a)
        sta _SMCDesignator
.elseif .match ({register}, x)
        stx _SMCDesignator
.elseif .match ({register}, y)
        sty _SMCDesignator
.endif
.endmacro

.macro SMC_ChangeBranch         label, destination, register
.if .paramcount = 2 .or .match ({register}, a)
        lda #(destination - _SMCDesignator -2)
        sta _SMCDesignator+1
.elseif .match ({register}, x)
        ldx #(destination - _SMCDesignator - 2)
        stx _SMCDesignator+1
.elseif .match ({register}, y)
        ldy #(destination - _SMCDesignator - 2)
        sty _SMCDesignator+1
.endif
.endmacro

.macro SMC_TransferValue        label, value, register
.if .paramcount = 2 .or .match ({register}, a)
        lda value
        sta _SMCDesignator+1
.elseif .match ({register}, x)
        ldx value
        stx _SMCDesignator+1
.elseif .match ({register}, y)
        ldy value
        sty _SMCDesignator+1
.endif
.endmacro

.macro SMC_LoadValue    label, register
.if .paramcount = 1 .or .match ({register}, a)
        lda _SMCDesignator+1
.elseif .match ({register}, x)
        ldx _SMCDesignator+1
.elseif .match ({register}, y)
        ldy _SMCDesignator+1
.endif
.endmacro

.macro SMC_StoreValue   label, register
.if .paramcount = 1 .or .match ({register}, a)
        sta _SMCDesignator+1
.elseif .match ({register}, x)
        stx _SMCDesignator+1
.elseif .match ({register}, y)
        sty _SMCDesignator+1
.endif
.endmacro


.macro SMC_TransferLowByte      label, value, register
SMC_TransferValue label, value, register
.endmacro

.macro SMC_LoadLowByte  label, register
SMC_LoadValue label, register
.endmacro

.macro SMC_StoreLowByte label, register
SMC_StoreValue label, register
.endmacro

.macro SMC_TransferHighByte     label, value, register
.if .paramcount = 2 .or .match ({register}, a)
        lda value
        sta _SMCDesignator+2
.elseif .match ({register}, x)
        ldx value
        stx _SMCDesignator+2
.elseif .match ({register}, y)
        ldy value
        sty _SMCDesignator+2
.endif
.endmacro

.macro SMC_LoadHighByte label, register
.if .paramcount = 1 .or .match ({register}, a)
        lda _SMCDesignator+2
.elseif .match ({register}, x)
        ldx _SMCDesignator+2
.elseif .match ({register}, y)
        ldy _SMCDesignator+2
.endif
.endmacro

.macro SMC_StoreHighByte        label, register
.if .paramcount = 1 .or .match ({register}, a)
        sta _SMCDesignator+2
.elseif .match ({register}, x)
        stx _SMCDesignator+2
.elseif .match ({register}, y)
        sty _SMCDesignator+2
.endif
.endmacro

.macro SMC_TransferAddressSingle        label, address, register
.if .paramcount = 2 .or .match ((register), a)
        .if (.match (.left (1, {address}), #))
                ; immediate mode
                lda #<(.right (.tcount ({address})-1, {address}))
                sta _SMCDesignator+1
                lda #>(.right (.tcount ({address})-1, {address}))
                sta _SMCDesignator+2
        .else
                ; assume absolute or zero page
                lda address
                sta _SMCDesignator+1
                lda 1+(address)
                sta _SMCDesignator+2
        .endif
.elseif .match ((register), x)
        .if (.match (.left (1, {address}), #))
                ; immediate mode
                ldx #<(.right (.tcount ({address})-1, {address}))
                stx _SMCDesignator+1
                ldx #>(.right (.tcount ({address})-1, {address}))
                stx _SMCDesignator+2
        .else
                ; assume absolute or zero page
                ldx address
                stx _SMCDesignator+1
                ldx 1+(address)
                stx _SMCDesignator+2
        .endif
.elseif .match ((register), y)
        .if (.match (.left (1, {address}), #))
                ; immediate mode
                ldy #<(.right (.tcount ({address})-1, {address}))
                sty _SMCDesignator+1
                ldy #>(.right (.tcount ({address})-1, {address}))
                sty _SMCDesignator+2
        .else
                ; assume absolute or zero page
                ldy address
                sty _SMCDesignator+1
                ldy 1+(address)
                sty _SMCDesignator+2
        .endif
.endif
.endmacro

.macro SMC_TransferAddress      label, address
.if (.match (.left (1, {address}), #))
        ; immediate mode
        lda #<(.right (.tcount ({address})-1, {address}))
        sta _SMCDesignator+1
        ldx #>(.right (.tcount ({address})-1, {address}))
        stx _SMCDesignator+2
.else
        ; assume absolute or zero page
        lda {address}
        sta _SMCDesignator+1
        ldx 1+{address}
        stx _SMCDesignator)+2
.endif
.endmacro

.macro SMC_StoreAddress label
        sta _SMCDesignator+1
        stx _SMCDesignator+2
.endmacro
