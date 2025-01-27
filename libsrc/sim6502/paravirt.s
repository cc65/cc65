;
; Oliver Schmidt, 2013-05-16
;
; int open (const char* name, int flags, ...);
; int __fastcall__ close (int fd);
; int __fastcall__ read (int fd, void* buf, unsigned count);
; int __fastcall__ write (int fd, const void* buf, unsigned count);
;

        .export         exit, args, _open, _close, _read, _write, _lseek
        .export         __sysremove, ___osmaperrno

_lseek          := $FFF1
__sysremove     := $FFF2
___osmaperrno   := $FFF3
_open           := $FFF4
_close          := $FFF5
_read           := $FFF6
_write          := $FFF7
args            := $FFF8
exit            := $FFF9

                ; $FFFA-FFFF are hardware vectors, extend before not after!
