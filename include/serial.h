/*****************************************************************************/
/*                                                                           */
/*                                 serial.h                                  */
/*                                                                           */
/*                         Serial communication API                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2012, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#ifndef _SERIAL_H
#define _SERIAL_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Baudrate settings */
#define SER_BAUD_45_5           0x00
#define SER_BAUD_50             0x01
#define SER_BAUD_75             0x02
#define SER_BAUD_110            0x03
#define SER_BAUD_134_5          0x04
#define SER_BAUD_150            0x05
#define SER_BAUD_300            0x06
#define SER_BAUD_600            0x07
#define SER_BAUD_1200           0x08
#define SER_BAUD_1800           0x09
#define SER_BAUD_2400           0x0A
#define SER_BAUD_3600           0x0B
#define SER_BAUD_4800           0x0C
#define SER_BAUD_7200           0x0D
#define SER_BAUD_9600           0x0E
#define SER_BAUD_19200          0x0F
#define SER_BAUD_38400          0x10
#define SER_BAUD_57600          0x11
#define SER_BAUD_115200         0x12
#define SER_BAUD_230400         0x13
#define SER_BAUD_31250          0x14
#define SER_BAUD_62500          0x15
#define SER_BAUD_56_875         0x16

/* Data bit settings */
#define SER_BITS_5              0x00
#define SER_BITS_6              0x01
#define SER_BITS_7              0x02
#define SER_BITS_8              0x03

/* Stop bit settings */
#define SER_STOP_1              0x00    /* One stop bit */
#define SER_STOP_2              0x01    /* Two stop bits */

/* Parity settings */
#define SER_PAR_NONE            0x00
#define SER_PAR_ODD             0x01
#define SER_PAR_EVEN            0x02
#define SER_PAR_MARK            0x03
#define SER_PAR_SPACE           0x04

/* Handshake settings. The latter two may be combined. */
#define SER_HS_NONE             0x00    /* No handshake */
#define SER_HS_HW               0x01    /* Hardware (RTS/CTS) handshake */
#define SER_HS_SW               0x02    /* Software handshake */

/* Bit masks to mask out things from the status returned by ser_status.
** These are 6551 specific and must be mapped by drivers for other chips.
*/
#define SER_STATUS_PE           0x01    /* Parity error */
#define SER_STATUS_FE           0x02    /* Framing error */
#define SER_STATUS_OE           0x04    /* Overrun error */
#define SER_STATUS_DCD          0x20    /* NOT data carrier detect */
#define SER_STATUS_DSR          0x40    /* NOT data set ready */

/* Error codes returned by all functions */
#define SER_ERR_OK              0x00    /* Not an error - relax */
#define SER_ERR_NO_DRIVER       0x01    /* No driver available */
#define SER_ERR_CANNOT_LOAD     0x02    /* Error loading driver */
#define SER_ERR_INV_DRIVER      0x03    /* Invalid driver */
#define SER_ERR_NO_DEVICE       0x04    /* Device (hardware) not found */
#define SER_ERR_BAUD_UNAVAIL    0x05    /* Baud rate not available */
#define SER_ERR_NO_DATA         0x06    /* Nothing to read */
#define SER_ERR_OVERFLOW        0x07    /* No room in send buffer */
#define SER_ERR_INIT_FAILED     0x08    /* Initialization failed */
#define SER_ERR_INV_IOCTL       0x09    /* IOCTL not supported */
#define SER_ERR_INSTALLED       0x0A    /* A driver is already installed */
#define SER_ERR_NOT_OPEN        0x0B    /* Driver is not open */

/* Struct containing parameters for the serial port */
struct ser_params {
    unsigned char       baudrate;       /* Baudrate */
    unsigned char       databits;       /* Number of data bits */
    unsigned char       stopbits;       /* Number of stop bits */
    unsigned char       parity;         /* Parity setting */
    unsigned char       handshake;      /* Type of handshake to use */
};

/* The name of the standard serial driver for a platform */
extern const char ser_stddrv[];

/* The address of the static standard serial driver for a platform */
extern const void ser_static_stddrv[];



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned char __fastcall__ ser_load_driver (const char* driver);
/* Load and install a serial driver. Return an error code. */

unsigned char ser_unload (void);
/* Uninstall, then unload the currently loaded driver. */

unsigned char __fastcall__ ser_install (const void* driver);
/* Install an already loaded driver. Return an error code. */

unsigned char ser_uninstall (void);
/* Uninstall the currently loaded driver and return an error code.
** Note: This call does not free allocated memory.
*/

unsigned char __fastcall__ ser_open (const struct ser_params* params);
/* "Open" the port by setting the port parameters and enable interrupts. */

unsigned char ser_close (void);
/* "Close" the port. Clear buffers and disable interrupts. */

unsigned char __fastcall__ ser_get (char* b);
/* Get a character from the serial port. If no characters are available, the
** function will return SER_ERR_NO_DATA, so this is not a fatal error.
*/

unsigned char __fastcall__ ser_put (char b);
/* Send a character via the serial port. There is a transmit buffer, but
** transmitting is not done via interrupt. The function returns
** SER_ERR_OVERFLOW if there is no space left in the transmit buffer.
*/

unsigned char __fastcall__ ser_status (unsigned char* status);
/* Return the serial port status. */

unsigned char __fastcall__ ser_ioctl (unsigned char code, void* data);
/* Driver specific entry. */



/* End of serial.h */
#endif
