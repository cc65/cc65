/*
 * rs232.h
 *
 * Ullrich von Bassewitz, 19.3.1999
 *
 * This module is based upon the public domain swiftlink module written by
 * Craig Bruce. Thanks a lot!
 *
 */



#ifndef _RS232_H
#define _RS232_h



/*****************************************************************************/
/*			  	     Data				     */
/*****************************************************************************/


#if defined(__C64__) || defined(__C128__)

/* Baudrate settings */
#define RS_BAUD_50     	       		0x00
#define RS_BAUD_110    	       		0x01
#define RS_BAUD_134_5  	       	  	0x02
#define RS_BAUD_300    	       		0x03
#define RS_BAUD_600    	       		0x04
#define RS_BAUD_1200   	       		0x05
#define RS_BAUD_2400   	       		0x06
#define RS_BAUD_4800   	       		0x07
#define RS_BAUD_9600   	       		0x08
#define RS_BAUD_19200  	       		0x09
#define RS_BAUD_38400  	       		0x0A
#define RS_BAUD_57600  	       		0x0B
#define RS_BAUD_115200 	       		0x0C
#define RS_BAUD_230400 	       		0x0D

/* Data bit settings */
#define RS_BITS_5      	       		0x60
#define RS_BITS_6      	       		0x40
#define RS_BITS_7      	       		0x20
#define RS_BITS_8      	       		0x00

/* Parity settings */
#define RS_PAR_NONE    	       		0x00
#define RS_PAR_ODD     	       		0x20
#define RS_PAR_EVEN    	       		0x60
#define RS_PAR_MARK    	       		0xA0
#define RS_PAR_SPACE   	       		0xE0

/* Bit masks to mask out things from the status returned by rs232_status */
#define RS_STATUS_PE   	       		0x01	/* Parity error */
#define RS_STATUS_FE			0x02	/* Framing error */
#define RS_STATUS_OVERRUN		0x04	/* Overrun error */
#define RS_STATUS_RDRF			0x08	/* Receiver data register full */
#define RS_STATUS_THRE			0x10	/* Transmit holding reg. empty */
#define RS_STATUS_DCD			0x20	/* NOT data carrier detect */
#define RS_STATUS_DSR			0x40	/* NOT data set ready */
#define RS_STATUS_IRQ  	       		0x80	/* IRQ condition */

#elif defined(__ATARI__)

/* Baudrate settings */
#define RS_BAUD_300    	       		0x00
#define RS_BAUD_45_5   	       		0x01
#define RS_BAUD_50     	       		0x02
#define RS_BAUD_56_875 	       		0x03
#define RS_BAUD_75 	       		0x04
#define RS_BAUD_110    	       		0x05
#define RS_BAUD_134_5  	       	  	0x06
#define RS_BAUD_150    	       		0x07
/*#define RS_BAUD_300    	       	0x08  alternative */
#define RS_BAUD_600    	       		0x09
#define RS_BAUD_1200   	       		0x0A
#define RS_BAUD_1800   	       		0x0B
#define RS_BAUD_2400   	       		0x0C
#define RS_BAUD_4800   	       		0x0D
#define RS_BAUD_9600   	       		0x0E

/* Data bit settings */
#define RS_BITS_5      	       		0x30
#define RS_BITS_6      	       		0x20
#define RS_BITS_7      	       		0x10
#define RS_BITS_8      	       		0x00

/* Parity settings */
#define RS_PAR_NONE    	       		0x00
#define RS_PAR_ODD     	       		0x05
#define RS_PAR_EVEN    	       		0x0A
#define RS_PAR_MARK    	       		0x03
#define RS_PAR_SPACE   	       		0x0C

/* Bit masks to mask out things from the status returned by rs232_status */
#define RS_STATUS_PE   	       		0x20	/* Parity error */
#define RS_STATUS_FE			0x80	/* Framing error */
#define RS_STATUS_OVERRUN		0x40	/* Overrun error */
#define RS_STATUS_RDRF			0x10	/* Receiver data register full */

#endif /* __ATARI__ section */

/* Stop bit settings */
#define RS_STOP_1      	       		0x00
#define RS_STOP_2      	       		0x80

/* Error codes returned by all functions */
#define RS_ERR_OK     	       		0x00	/* Not an error - relax */
#define RS_ERR_NOT_INITIALIZED 		0x01   	/* Module not initialized */
#define RS_ERR_BAUD_TOO_FAST		0x02   	/* Cannot handle baud rate */
#define RS_ERR_BAUD_NOT_AVAIL		0x03   	/* Baud rate not available */
#define RS_ERR_NO_DATA		  	0x04   	/* Nothing to read */
#define RS_ERR_OVERFLOW       	       	0x05   	/* No room in send buffer */
#define RS_ERR_INIT_FAILED     	       	0x06   	/* Initialization of RS232 routines failed */



/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



unsigned char __fastcall__ rs232_init (char hacked);
/* Initialize the serial port, install the interrupt handler. The parameter
 * must be true (non zero) for a hacked swiftlink and false (zero) otherwise.
 */

unsigned char __fastcall__ rs232_params (unsigned char params, unsigned char parity);
/* Set the port parameters. Use a combination of the #defined values above. */

unsigned char __fastcall__ rs232_done (void);
/* Close the port, deinstall the interrupt hander. You MUST call this function
 * before terminating the program, otherwise the machine may crash later. If
 * in doubt, install an exit handler using atexit(). The function will do
 * nothing, if it was already called.
 */

unsigned char __fastcall__ rs232_get (char* b);
/* Get a character from the serial port. If no characters are available, the
 * function will return RS_ERR_NO_DATA, so this is not a fatal error.
 */

unsigned char __fastcall__ rs232_put (char b);
/* Send a character via the serial port. There is a transmit buffer, but
 * transmitting is not done via interrupt. The function returns
 * RS_ERR_OVERFLOW if there is no space left in the transmit buffer.
 */

unsigned char __fastcall__ rs232_pause (void);
/* Assert flow control and disable interrupts. */

unsigned char __fastcall__ rs232_unpause (void);
/* Re-enable interrupts and release flow control */

unsigned char __fastcall__ rs232_status (unsigned char* status,
					 unsigned char* errors);
/* Return the serial port status. */



/* End of rs232.h */
#endif



