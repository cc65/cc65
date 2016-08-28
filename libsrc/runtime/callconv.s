; callconv.s                                    
; Calling convertor, fastcall to register call               
;                                                                           
; (c) Christian Krüger, latest change: 18-Jul-2013                          
;

.importzp ptr1,ptr2,ptr3,ptr4
.import popax

; int __fastcall__
; RegCallPcPcRetN(RegCallFuncRetN func, char* param1, char* param2);
.export _RegCallPcPcRetN

; size_t __fastcall__
; RegCallPcPcRetSt(RegCallFuncRetSt func, char* param1, char* param2);
.export _RegCallPcPcRetSt

; void __fastcall__
; RegCallPvPvRetV(RegCallFuncRetPv func, void* param1, void* param2);
.export _RegCallPvPvRetV

; void __fastcall__
; RegCallPvStRetV(RegCallFuncRetV func, void* param1, size_t param2);
.export _RegCallPvStRetV

; void* __fastcall__
; RegCallPvPvRetPv(RegCallFuncRetPv func, void* param1, void* param2);
.export _RegCallPvPvRetPv

; void* __fastcall__
; RegCallPvStRetpV(RegCallFuncRetPv func, void* param1, size_t param2);
.export _RegCallPvStRetPv

; char* __fastcall__
; RegCallPcPcRetPc(RegCallFunc func, char* param1, char* param2);
.export _RegCallPcPcRetPc

; char* __fastcall__
; RegCallPcNRetPc(RegCallFuncRetPc func, char* param1, int param2);
.export _RegCallPcNRetPc

; int __fastcall__
; RegCallPvPvStRetN(RegCallFuncRetN func, void* param1, void* param2, size_t param3);
.export _RegCallPvPvStRetN

; void* __fastcall__
; RegCallPvNStRetPv(RegCallFuncRetPv func, void* param1, int param2, size_t param3);
.export _RegCallPvNStRetPv

; void* __fastcall__
; RegCallPvStNRetPv(RegCallFuncRetPv func, void* param1, size_t param2, int param3);
.export _RegCallPvStNRetPv

; void* __fastcall__
; RegCallPvPvStRetPv(RegCallFuncRetPv func, void* param1, void* param2, size_t param3);
.export _RegCallPvPvStRetPv

; int __fastcall__
; RegCallPcPcStRetN(RegCallFuncRetN func, char* param1, char* param2, size_t param3);
.export _RegCallPcPcStRetN

; size_t __fastcall__
; RegCallPcPcStRetSt(RegCallFuncRetSt func, char* param1, char* param2, size_t param3);
.export _RegCallPcPcStRetSt

; char* __fastcall__
; RegCallPcPcStRetPc(RegCallFuncRetPc func, char* param1, char* param2, size_t param3);
.export _RegCallPcPcStRetPc


_RegCallPvNStRetPv:
_RegCallPvStNRetPv:
_RegCallPvPvStRetN:
_RegCallPvPvStRetPv:
_RegCallPcPcStRetN:
_RegCallPcPcStRetSt:
_RegCallPcPcStRetPc:
    sta ptr3
    stx ptr3+1
    jsr popax
    
_RegCallPcPcRetN:
_RegCallPcPcRetSt:
_RegCallPvPvRetV:
_RegCallPvStRetV:
_RegCallPvPvRetPv:
_RegCallPvStRetPv:
_RegCallPcPcRetPc:    
_RegCallPcNRetPc:    
    sta ptr2
    stx ptr2+1
    jsr popax
    
_RegCall1P:    
    sta ptr1
    stx ptr1+1
    jsr popax
    sta ptr4
    stx ptr4+1    
    jmp (ptr4)
    
