;*****************************************************************************/
;*                                                                           */
;*                                 stdio.inc                                 */
;*                                                                           */
;*                      Mirror definitions for stdio.h                       */
;*                                                                           */
;*                                                                           */
;*                                                                           */
;* (C) 2003      Ullrich von Bassewitz                                       */
;*               Römerstrasse 52                                             */
;*               D-70794 Filderstadt                                         */
;* EMail:        uz@cc65.org                                                 */
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



;----------------------------------------------------------------------------
; Constants

_IOFBF		= 0
_IOLBF		= 1
_IONBF		= 2
BUFSIZ		= 256
EOF 	      	= -1
FILENAME_MAX	= 16
FOPEN_MAX	= 8
L_tmpnam	= (FILENAME_MAX + 1)
SEEK_CUR	= 0
SEEK_END	= 1
SEEK_SET	= 2
TMP_MAX		= 256

;----------------------------------------------------------------------------
; External variables

        .global _stdin
        .global _stdout
        .global _stderr



