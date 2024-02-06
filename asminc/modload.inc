;*****************************************************************************/
;*                                                                           */
;*                                  modload.inc                              */
;*                                                                           */
;*                     o65 module loader interface for cc65                  */
;*                                                                           */
;*                                                                           */
;*                                                                           */
;* (C) 2002      Ullrich von Bassewitz                                       */
;*               Wacholderweg 14                                             */
;*               D-70597 Stuttgart                                           */
;* EMail:        uz@musoftware.de                                            */
;*                                                                           */
;*                                                                           */
;* This software is provided 'as-is', without any expressed or implied       */
;* warranty.  In no event will the authors be held liable for any damages    */
;* arising from the use of this software.                                    */
;*                                                                           */
;* Permission is granted to anyone to use this software for any purpose,     */
;* including commercial applications, and to alter it and redistribute it    */
;* freely, subject to the following restrictions:                            */
;*                                                                           */
;* 1. The origin of this software must not be misrepresented; you must not   */
;*    claim that you wrote the original software. If you use this software   */
;*    in a product, an acknowledgment in the product documentation would be  */
;*    appreciated but is not required.                                       */
;* 2. Altered source versions must be plainly marked as such, and must not   */
;*    be misrepresented as being the original software.                      */
;* 3. This notice may not be removed or altered from any source              */
;*    distribution.                                                          */
;*                                                                           */
;*****************************************************************************/



; Exports structures and functions to load relocatable o65 modules at
; runtime.



; Offsets for the mod_ctrl struct. This struct is passed to the module loader.
; It contains stuff, the loader needs to work, and another area where the
; loader will place informational data if it was successful. You will have to
; check the return code of mod_load before accessing any of these additional
; struct members.
.struct MOD_CTRL
        READ            .addr
        CALLERDATA      .word
        MODULE          .addr           ; Pointer to module data
        MODULE_SIZE     .word           ; Total size of loaded module
        MODULE_ID       .word
.endstruct

; unsigned char mod_load (struct mod_ctrl* ctrl);
; /* Load a module into memory and relocate it. The function will return an
;  * error code (see below). If MLOAD_OK is returned, the outgoing fields in
;  * the passed mod_ctrl struct contain information about the module just
;  * loaded.
;  */
.global         _mod_load

; void mod_free (void* module);
; /* Free a loaded module. Note: The given pointer is the pointer to the
;  * module memory, not a pointer to a control structure.
;  */
.global         _mod_free

; Errors
.enum
        MLOAD_OK        ; Module load successful
        MLOAD_ERR_READ  ; Read error
        MLOAD_ERR_HDR   ; Header error
        MLOAD_ERR_OS    ; Wrong OS
        MLOAD_ERR_FMT   ; Data format error
        MLOAD_ERR_MEM   ; Not enough memory
.endenum


