;
; Ullrich von Bassewitz, 16.05.2000
;

; errno variable

        .global         __errno

; Error codes, must match the values in the C headers
ENOENT		=	1	; No such file or directory
ENOMEM		=	2	; Out of memory
EACCES		=	3	; Permission denied
ENODEV		=	4	; No such device
EMFILE		=	5	; Too many open files
EBUSY		=	6	; Device or resource busy
EINVAL		=	7	; Invalid argument
ENOSPC		=	8	; No space left on device
EEXIST		=	9	; File exists
EAGAIN		=	10	; Try again
EIO		=	11	; I/O error
EINTR		=	12	; Interrupted system call
ENOSYS		=	13	; Function not implemented
ESPIPE		=	14	; Illegal seek
ERANGE          =       15      ; Range error
EUNKNOWN       	=      	16	; Unknown OS specific error - must be last!

EMAX		=	16	; Highest error code

