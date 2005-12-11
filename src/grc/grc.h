
/* I hope that no one will be able to create a .grc bigger than this... */
#define BLOODY_BIG_BUFFER 65000
/* there are no 6MB GEOS binaries... I hope! */
#define THIS_BUFFER_IS_SOOO_HUGE 6000000

struct menuitem {
	char *name;
	char *type;
	char *target;
	struct menuitem *next; };

struct menu {
	char *name;
	int top, left;
	int bot, right;
	char *type;
	struct menuitem *item; };

struct appheader {
	int year,month,day,hour,min;
	int mode;
	int dostype;
	int geostype;
	int structure;
	char *dosname;
	char *classname;
	char *version;
	char *author;
	char *info; };

const char *mainToken[] = {
	"MENU", "HEADER", "ICON", "DIALOG", "VLIR", "" };

const char *hdrFTypes[] = {
	"APPLICATION", "AUTO_EXEC", "DESK_ACC", "ASSEMBLY", "DISK_DEVICE", "PRINTER", "SYSTEM", "" };

const char *hdrFields[] = {
	"author", "info", "date", "dostype", "mode", "structure", "" };

const char *hdrDOSTp[] = {
	"seq", "SEQ", "prg", "PRG", "usr", "USR", "" };

const char *hdrStructTp[] = {
	"seq", "SEQ", "vlir", "VLIR", "" };

const char *hdrModes[] = {
	"any", "40only", "80only", "c64only", "" };

const int BSWTab[] = { 0,
	0x005, 0x007, 0x00b, 0x011, 0x017,
	0x01d, 0x023, 0x025, 0x029, 0x02d, 0x033, 0x039, 0x03c, 0x041, 0x043,
	0x04a, 0x04f, 0x052, 0x056, 0x05a, 0x05f, 0x063, 0x068, 0x06d, 0x072,
	0x077, 0x079, 0x07c, 0x080, 0x084, 0x088, 0x08e, 0x094, 0x09a, 0x09f,
	0x0a4, 0x0a9, 0x0ad, 0x0b1, 0x0b6, 0x0bc, 0x0be, 0x0c2, 0x0c8, 0x0cc,
	0x0d4, 0x0da, 0x0e0, 0x0e5, 0x0eb, 0x0f0, 0x0f5, 0x0f9, 0x0fe, 0x104,
	0x10c, 0x112, 0x118, 0x11e, 0x121, 0x129, 0x12c, 0x132, 0x13a, 0x13e,
	0x143, 0x148, 0x14d, 0x152, 0x157, 0x15a, 0x15f, 0x164, 0x166, 0x168,
	0x16d, 0x16f, 0x177, 0x17c, 0x182, 0x187, 0x18c, 0x18f, 0x193, 0x196,
	0x19b, 0x1a1, 0x1a9, 0x1af, 0x1b4, 0x1ba, 0x1be, 0x1c0, 0x1c4, 0x1ca,
	0x1d2, 0x1dd };

const unsigned char icon1[] = {
	 255, 255, 255, 128, 0, 1, 128, 0, 1, 128, 0, 1, 128, 0, 1, 128, 0, 1,
			128, 0, 1, 128, 0, 1, 128, 0, 1, 128, 0, 1, 128, 0, 1,
			128, 0, 1, 128, 0, 1, 128, 0, 1, 128, 0, 1, 128, 0, 1,
			128, 0, 1, 128, 0, 1, 128, 0, 1, 128, 0, 1, 255, 255, 255 };

char *ProgName;	/* for AbEnd, later remove and use common/cmdline.h */

char *outputCName=NULL, *outputSName=NULL, *outputVName=NULL;
FILE *outputCFile, *outputSFile, *outputVFile;
int CFnum=0, SFnum=0, VFnum=0;
int forceFlag=0;
char outputCMode[2]="w";
char outputSMode[2]="w";
char outputVMode[2]="w";
