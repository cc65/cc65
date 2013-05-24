;
; Oliver Schmidt, 2013-05-16
;
; int open (const char* name, int flags, ...);
; int __fastcall__ close (int fd);
; int __fastcall__ read (int fd, void* buf, unsigned count);
; int __fastcall__ write (int fd, const void* buf, unsigned count);
;

        .export         args, exit, _open, _close, _read, _write

args            := $FFF0
exit            := $FFF1
_open           := $FFF2
_close          := $FFF3
_read           := $FFF4
_write          := $FFF5
