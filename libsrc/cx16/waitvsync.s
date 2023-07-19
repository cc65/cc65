;
; 2021-04-01, Greg King
;
; void waitvsync (void);
; /* Wait for the start of the next video field. */
;
; VERA's vertical sync causes IRQs which increment the jiffy timer.
;
; Updated by ZeroByteOrg to use Kernal API RDTIM to retreive the TIMER variable
;

        .export         _waitvsync
        .importzp       tmp1
        .import         RDTIM

.proc _waitvsync: near
      jsr RDTIM
      sta tmp1
keep_waiting:
      jsr RDTIM
      cmp tmp1
      beq keep_waiting
      rts
.endproc
