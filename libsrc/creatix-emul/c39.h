#ifndef C39_H
#define C39_H 1

typedef char i8;
typedef int i16;
typedef long i32;

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

#define TIMER_VECT_A_RAM_LO 0x0102
#define TIMER_VECT_A_RAM_HI 0x0103      

#define TIMER_VECT_B_RAM_LO 0x0106
#define TIMER_VECT_B_RAM_HI 0x0107

#define P_C      0x0002

#define P_TAMODE 0x0010
#define P_TALC   0x0011
#define P_TASC   0x0013

#define P_TBMODE 0x0014
#define P_TBLC   0x0015
#define P_TBSC   0x0017

#define TIMER_DIV32                   0x04
#define TIMER_IRQ_RAM_VECTOR_ENABLE   0x20
#define TIMER_IRQ_ENABLE              0x40

#define P_SSR    0x003C

#define BIT_SIN_BUFFER_FULL    0
#define BIT_SIN_OVERRUN_ERROR  1
#define BIT_SIN_PARITY_ERROR   2
#define BIT_SIN_FRAMING_ERROR  3
#define BIT_SIN_BREAK_DETECTED 4
#define BIT_SOUT_BUFFER_EMPTY  5
#define BIT_SOUT_UNDERRUN      6
#define BIT_SIN_PARIITY_BIT    7

#define SET_BYTE_ZP(ADDR, VAL) \
  __asm__ ("lda #%w", VAL); \
  __asm__ ("sta %w", ADDR);

#define GET_BYTE_ZP(ADDR, VAR)  \
  __asm__ ("lda %w", ADDR); \
  __asm__ ("sta %v", VAR);

// asm funcs

// does not work, use inline asm..
//void __fastcall__  set_byte_zp(unsigned char addr, unsigned char val);
//unsigned char __fastcall__ get_byte_zp(unsigned char addr);

void install_timer_a_irq_handler(void);
void install_timer_b_irq_handler(void);
void install_serial_status_irq_handler(void);

void __fastcall__ serial_putc(u8 c);

u8 serial_getc(void);
u8 serial_getc_echo(void);
u8 serial_check_break(void);

void serial_enable_status_irq(void);
void serial_disable_status_irq(void);

void serial_crlf(void);

void cts_led_on(void);
void cts_led_off(void);

void dsr_led_on(void);
void dsr_led_off(void);

void rlsd_led_on(void);
void rlsd_led_off(void);

void R6746_led_on(void);
void R6746_led_off(void);

#endif
