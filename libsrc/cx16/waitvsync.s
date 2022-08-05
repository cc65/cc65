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

.proc _waitvsync: near
      RDTIM = $FFDE  ; Kernal API for reading the jiffy timer
      jsr RDTIM
      sta lastjiffy
keep_waiting:
      jsr RDTIM
      cmp #$FF       ; self-mod the value returned by RDTIM to save memory
      lastjiffy=(*-1)
      beq keep_waiting
      rts
.endproc
