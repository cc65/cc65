/*****************************************************************************/
/*                                                                           */
/* Atari VCS 2600 TIA registers addresses                                    */
/*                                                                           */
/* Source: DASM - vcs.h                                                      */
/*                                                                           */
/* Florent Flament (contact@florentflament.com), 2017                        */
/*                                                                           */
/*****************************************************************************/

/* TIA write / read registers */
struct __tia {
    union {
        unsigned char vsync;
        unsigned char cxm0p;
    };
    union {
        unsigned char vblank;
        unsigned char cxm1p;
    };
    union {
        unsigned char wsync;
        unsigned char cxp0fb;
    };
    union {
        unsigned char rsync;
        unsigned char cxp1fb;
    };
    union {
        unsigned char nusiz0;
        unsigned char cxm0fb;
    };
    union {
        unsigned char nusiz1;
        unsigned char cxm1fb;
    };
    union {
        unsigned char colup0;
        unsigned char cxblpf;
    };
    union {
        unsigned char colup1;
        unsigned char cxppmm;
    };
    union {
        unsigned char colupf;
        unsigned char inpt0;
    };
    union {
        unsigned char colubk;
        unsigned char inpt1;
    };
    union {
        unsigned char ctrlpf;
        unsigned char inpt2;
    };
    union {
        unsigned char refp0;
        unsigned char inpt3;
    };
    union {
        unsigned char refp1;
        unsigned char inpt4;
    };
    union {
        unsigned char pf0;
        unsigned char inpt5;
    };
    unsigned char pf1;
    unsigned char pf2;
    unsigned char resp0;
    unsigned char resp1;
    unsigned char resm0;
    unsigned char resm1;
    unsigned char resbl;
    unsigned char audc0;
    unsigned char audc1;
    unsigned char audf0;
    unsigned char audf1;
    unsigned char audv0;
    unsigned char audv1;
    unsigned char grp0;
    unsigned char grp1;
    unsigned char enam0;
    unsigned char enam1;
    unsigned char enabl;
    unsigned char hmp0;
    unsigned char hmp1;
    unsigned char hmm0;
    unsigned char hmm1;
    unsigned char hmbl;
    unsigned char vdelp0;
    unsigned char vdelp1;
    unsigned char vdelbl;
    unsigned char resmp0;
    unsigned char resmp1;
    unsigned char hmove;
    unsigned char hmclr;
    unsigned char cxclr;
};
