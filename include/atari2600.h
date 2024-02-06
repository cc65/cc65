/*****************************************************************************/
/*                                                                           */
/* Atari VCS 2600 TIA & RIOT registers addresses                             */
/*                                                                           */
/* Source: DASM Version 1.05 - vcs.h                                         */
/*                                                                           */
/* Florent Flament (contact@florentflament.com), 2017                        */
/*                                                                           */
/*****************************************************************************/



#ifndef _ATARI2600_H
#define _ATARI2600_H



/* Check for errors */
#if !defined(__ATARI2600__)
#  error This module may only be used when compiling for the Atari 2600!
#endif

#include <_tia.h>
#define TIA (*(struct __tia*)0x0000)

#include <_riot.h>
#define RIOT (*(struct __riot*)0x0280)



/* End of atari2600.h */
#endif
