
void print (char *);

void hires();
void text();
void oups();
void ping();
void zap();
void shoot();
void explode();

void paper(char color);
void ink(char color);

void kbdclick1();



void curset(char x,char y);
void circle(char rayon);

char key(void);


/* PEEK, POKE, DEEK, DOKE */

#define POKE(addr,val)     (*(unsigned char*) (addr) = (val))


#define PCHN_1   001
#define PCHN_2   002
#define PCHN_12  003
#define PCHN_3   004
#define PCHN_13  005
#define PCHN_23  006
#define PCHN_123 007

#define ENV_DECAY       001  /* \_________ envelope */
#define ENV_ATTACK_CUT  002  /* /_________ envelope */
#define ENV_SAW_DOWN    003  /* \\\\\\\\\\ envelope */
#define ENV_WAVE        004  /* /\/\/\/\/\ envelope */
#define ENV_DECAY_CONT  005  /* \~~~~~~~~~ envelope */
#define ENV_SAW_UP      006  /* ////////// envelope */
#define ENV_ATTACK_CONT 007  /* /~~~~~~~~~ envelope */

#define VOL_ENVELOPE  0x0
#define VOL_QUIETEST  0x1
#define VOL_LOUDEST   0xe

extern int play(int soundchanels,int noisechanels,int envelop,int volume);


/* Play a musical tone through the selected channel. */

#define CHAN_1   1
#define CHAN_2   2
#define CHAN_3   3

