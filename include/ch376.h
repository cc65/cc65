

/* /// "Portable types" */




/* /// "CH376 interface commands and constants" */

// Chip version
#define CH376_DATA_IC_VER 3

// Commands
#define CH376_CMD_NONE        0x00
#define CH376_CMD_GET_IC_VER  0x01
#define CH376_CMD_CHECK_EXIST   0x06
#define CH376_CMD_SET_USB_MODE  0x15
#define CH376_CMD_GET_STATUS    0x22
#define CH376_CMD_RD_USB_DATA0  0x27
#define CH376_CMD_WR_REQ_DATA   0x2d
#define CH376_CMD_SET_FILE_NAME 0x2f
#define CH376_CMD_DISK_MOUNT    0x31
#define CH376_CMD_FILE_OPEN     0x32
#define CH376_CMD_FILE_ENUM_GO  0x33
#define CH376_CMD_FILE_CREATE   0x34
#define CH376_CMD_FILE_CLOSE  0x36
#define CH376_CMD_BYTE_LOCATE   0x39
#define CH376_CMD_BYTE_READ     0x3a
#define CH376_CMD_BYTE_RD_GO    0x3b
#define CH376_CMD_BYTE_WRITE    0x3c
#define CH376_CMD_BYTE_WR_GO    0x3d
#define CH376_CMD_DISK_QUERY    0x3f
#define CH376_CMD_DISK_RD_GO    0x55

#define CH376_ARG_SET_USB_MODE_INVALID  0x00
#define CH376_ARG_SET_USB_MODE_SD_HOST  0x03
#define CH376_ARG_SET_USB_MODE_USB_HOST 0x06

// Status & errors
#define CH376_ERR_OPEN_DIR  0x41
#define CH376_ERR_MISS_FILE 0x42

#define CH376_RET_SUCCESS 0x51
#define CH376_RET_ABORT   0x5f

#define CH376_INT_SUCCESS    0x14
#define CH376_INT_DISK_READ  0x1d
#define CH376_INT_DISK_WRITE 0x1e


unsigned char	ch376_check_exist(unsigned char value);
unsigned char 	ch376_ic_get_version(void);
void 			ch376_set_usb_mode(unsigned char value);
unsigned char 	ch376_disk_mount(void);

