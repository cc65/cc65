;
; Definitions for CBM file types. From cbm.h
;
; Ullrich von Bassewitz, 2012-06-03
;

; Define bitmapped constants for the table entries

.enum
        CBM_T_REG     = $10     ; Bit set for regular files
        CBM_T_SEQ     = $10     ; Sequential file
        CBM_T_PRG     = $11     ; Program file
        CBM_T_USR     = $12     ; User file
        CBM_T_REL     = $13     ; Relative file
        CBM_T_VRP     = $14     ; Vorpal fast-loadable format
        CBM_T_DEL     = $00     ; Deleted file
        CBM_T_CBM     = $01     ; 1581 sub-partition
        CBM_T_DIR     = $02     ; IDE64 and CMD sub-directory
        CBM_T_LNK     = $03     ; IDE64 soft-link
        CBM_T_OTHER   = $04     ; File-type not recognized
        CBM_T_HEADER  = $05     ; Disk header / title
.endenum


; The following function maps the start character for a file type to
; one of the file types above. Note: 'd' will always mapped to CBM_T_DEL.
; The calling function has to look at the following character to determine
; if the file type is actually CBM_T_DIR.
;
; unsigned char __fastcall__ _cbm_filetype (unsigned char c);

        .global __cbm_filetype


