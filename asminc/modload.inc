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
MODCTRL_READ            =       0
MODCTRL_CALLERDATA      =       2
MODCTRL_MODULE          =       4       ; Pointer to module data
MODCTRL_MODULE_SIZE     =       6       ; Total size of loaded module
MODCTRL_CODE            =       8       ; Pointer to code segment
MODCTRL_CODE_SIZE       =      10       ; Size of code segment
MODCTRL_DATA            =      12       ; Pointer to data segment
MODCTRL_DATA_SIZE       =      14       ; Size of data segment
MODCTRL_BSS             =      16       ; Pointer to bss segment
MODCTRL_BSS_SIZE        =      18       ; Size of bss segment
MODCTRL_SIZE            =      20       ; Total size of struct


; unsigned char mod_load (struct mod_ctrl* ctrl);
; /* Load a module into memory and relocate it. The function will return an
;  * error code (see below). If MLOAD_OK is returned, the outgoing fields in
;  * the passed mod_ctrl struct contain information about the module just
;  * loaded.
;  */
.global         _mod_load



; Errors
MLOAD_OK                =       0       ; Module load successful
MLOAD_ERR_READ          =       1       ; Read error
MLOAD_ERR_HDR           =       2       ; Header error
MLOAD_ERR_OS            =       3       ; Wrong OS
MLOAD_ERR_FMT           =       4       ; Data format error
MLOAD_ERR_MEM           =       5       ; Not enough memory



