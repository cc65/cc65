;
; Oliver Schmidt, 2013-05-16
;
; int open (const char* name, int flags, ...);
; int __fastcall__ close (int fd);
; int __fastcall__ read (int fd, void* buf, unsigned count);
; int __fastcall__ write (int fd, const void* buf, unsigned count);
;

        .export         exit, _open, _close, _read, _write

exit            := $FFF0
_open           := $FFF1
_close          := $FFF2
_read           := $FFF3
_write          := $FFF4
