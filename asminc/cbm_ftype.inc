;
; Definitions for CBM file types. From cbm.h
;
; Ullrich von Bassewitz, 2012-06-03
;

; Define bitmapped constants for the table entries

.enum
        CBM_T_DEL               ; Deleted file
        CBM_T_SEQ               ; Sequential file
        CBM_T_PRG               ; Program file
        CBM_T_USR               ; User file
        CBM_T_REL               ; Relative file
        CBM_T_CBM               ; 1581 sub-partition
        CBM_T_DIR               ; IDE64 and CMD sub-directory
        CBM_T_LNK               ; IDE64 soft-link
        CBM_T_VRP               ; Vorpal fast-loadable format
        CBM_T_OTHER             ; File-type not recognized
        CBM_T_HEADER            ; Disk header / title 
.endenum


; The following function maps the start character for a file type to 
; one of the file types above. Note: 'd' will always mapped to CBM_T_DEL.
; The calling function has to look at the following character to determine
; if the file type is actually CBM_T_DIR.
;
; unsigned char __fastcall__ _cbm_filetype (unsigned char c);

        .global __cbm_filetype


