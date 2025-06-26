/*****************************************************************************/
/*                                                                           */
/*                                 profile.c                                 */
/*                                                                           */
/*           Function profiling functionality sim65 6502 simulator           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2025, Gorilla Sapiens                                                 */
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#include "6502.h"
#include "memory.h"
#include "trace.h"
#include "peripherals.h"

// map file should be generated with
// bin/cl65 -t sim6502 --mapfile map.txt -vm ~/profileme.c

bool enableProfiling = false;

const char *profileMap = NULL;

static uint16_t resetVector = 0xFFFF;

typedef struct ProfileCallEntry {
    uint16_t callee;
    int count;
} ProfileCallEntry;

typedef struct ProfileTimeEntry {
    uint16_t caller;
    uint64_t totalTicks;
    uint64_t childTicks;

    int children;
    ProfileCallEntry *calls;
} ProfileTimeEntry;

static ProfileTimeEntry *functions = NULL;
static int functionCount = 0;

static int CompareTimeEntry(const void *a, const void *b) {
    const ProfileTimeEntry *pa = (const ProfileTimeEntry *)a;
    const ProfileTimeEntry *pb = (const ProfileTimeEntry *)b;

    return (int)pa->caller - (int)pb->caller;
}

static int CompareCallEntry(const void *a, const void *b) {
    const ProfileCallEntry *pa = (const ProfileCallEntry *)a;
    const ProfileCallEntry *pb = (const ProfileCallEntry *)b;

    return (int)pa->callee - (int)pb->callee;
}

ProfileTimeEntry *EnsureTimeEntry(uint16_t caller) {
    ProfileTimeEntry timeKey = { caller, 0, 0, 0, NULL };

    ProfileTimeEntry *timeEntry =
        bsearch (&timeKey, functions, functionCount,
                 sizeof(ProfileTimeEntry), CompareTimeEntry);

    if (timeEntry == NULL) {
        functions = realloc(functions, (functionCount + 1) * sizeof(ProfileTimeEntry));
        timeEntry = functions + functionCount;
        functionCount++;
        timeEntry->caller = caller;
        timeEntry->totalTicks = 0;
        timeEntry->childTicks = 0;
        timeEntry->children = 0;
        timeEntry->calls = NULL;

        qsort(functions, functionCount, sizeof(ProfileTimeEntry), CompareTimeEntry);

        timeEntry =
            bsearch (&timeKey, functions, functionCount, sizeof(ProfileTimeEntry), CompareTimeEntry);
    }
    return timeEntry;
}

ProfileCallEntry *EnsureCallEntry(ProfileTimeEntry *timeEntry, uint16_t callee) {
    ProfileCallEntry callKey = { callee, 0 };

    ProfileCallEntry *callEntry =
        bsearch (&callKey, timeEntry->calls, timeEntry->children,
                 sizeof(ProfileCallEntry), CompareCallEntry);

    if (!callEntry) {
        timeEntry->calls = realloc(timeEntry->calls,
                                   (timeEntry->children + 1) * sizeof(ProfileCallEntry));
        callEntry = timeEntry->calls + timeEntry->children;
        timeEntry->children++;
        callEntry->callee = callee;
        callEntry->count = 0;

        qsort(timeEntry->calls, timeEntry->children, sizeof(ProfileCallEntry), CompareCallEntry);

        callEntry =
            bsearch (&callKey, timeEntry->calls, timeEntry->children,
                     sizeof(ProfileCallEntry), CompareCallEntry);
    }

    return callEntry;
}

static void ProfileLogCall(uint16_t caller, uint16_t callee) {
    ProfileTimeEntry *timeEntry = EnsureTimeEntry(caller);
    ProfileCallEntry *callEntry = EnsureCallEntry(timeEntry, callee);
    callEntry->count++;
}

static void ProfileLogReturn(uint16_t caller, uint16_t callee, uint64_t ticks) {
    ProfileTimeEntry *calleeEntry = EnsureTimeEntry(callee); /* must be first !!! */
    ProfileTimeEntry *callerEntry = EnsureTimeEntry(caller);
    callerEntry->childTicks += ticks;
    calleeEntry->totalTicks += ticks;
}

typedef struct PC2Name {
    int pc;
    const char *name;
} PC2Name;
PC2Name *pc2name = NULL;
int pc2nameCount = 0;

static int ComparePC2Name(const void *a, const void *b) {
    int pc_a = ((const PC2Name *)a)->pc;
    int pc_b = ((const PC2Name *)b)->pc;
    return pc_a - pc_b;
}

static void AddFunction(int pc, const char *name) {
    pc2name = realloc(pc2name, sizeof(PC2Name) * (pc2nameCount + 1));
    pc2name[pc2nameCount].pc = pc;
    pc2name[pc2nameCount].name = strdup(name);
    pc2nameCount++;
}

static const char *FindFunctionName(int pc) {
    PC2Name key = { pc, NULL };
    PC2Name *result = bsearch(&key, pc2name, pc2nameCount, sizeof(PC2Name), ComparePC2Name);
    if (result) {
        return result->name;
    }
    else {
        return "???";
    }
}

static void ReadMapFile(void) {
    char buf[128];
    char pieces[6][64];
    int mode = 0;
    FILE *f = fopen(profileMap, "r");
    int items;
    if (f) {
        while (fgets(buf, sizeof(buf), f)) {
            switch (mode) {
                case 0:
                    if (strstr(buf, "Exports list by name:")) {
                        mode = 1;
                    }
                    break;
                case 1:
                    mode++;
                    break;
                case 2:
                    items = sscanf(buf, "%63s %63s %63s %63s %63s %63s",
                                   pieces[0], pieces[1], pieces[2],
                                   pieces[3], pieces[4], pieces[5]);
                    if (items >= 3) {
                        int pc0 = strtoul(pieces[1], NULL, 16);
                        AddFunction(pc0, pieces[0]);
                        if (items == 6) {
                            int pc3 = strtoul(pieces[4], NULL, 16);
                            AddFunction(pc3, pieces[3]);
                        }
                    }
                    else {
                        qsort(pc2name, pc2nameCount, sizeof(PC2Name), ComparePC2Name);
                        mode++;
                    }
                default:
                    break;
            }
        }

        fclose(f);
    }
    else {
        printf("Unable to open map file '%s'\n", profileMap);
    }
}

static double percentage(uint64_t num, uint64_t den) {
   int tmp = (int) (num * 1000 / den);
   return tmp / 10.0;
}

static void ProfileDump(void) {
    uint64_t grandTotal, self;
    int i, j, k;
    int gap = 0;

    /* the startup function isn't called by a JSR, so we need to update it here. */
    ProfileTimeEntry *startup = EnsureTimeEntry(resetVector);
    grandTotal = startup->totalTicks = Peripherals.Counter.ClockCycles;

    ReadMapFile();

    for (i = 0; i < functionCount; i++) {
        gap = 0;
        if (i != 0) {
            printf("\n");
            printf("----------\n");
            printf("\n");
        }
        printf("function x%04x %s\n", functions[i].caller, FindFunctionName(functions[i].caller));
        for (k = 0; k < functionCount; k++) {
            for (j = 0; j < functions[k].children; j++) {
                if (functions[k].calls[j].callee == functions[i].caller) {
                    if (!gap) {
                        gap = 1;
                        printf("\n");
                    }
                    printf("\t%8d call%c from x%04x %s\n",
                        functions[k].calls[j].count,
                        (functions[k].calls[j].count == 1) ? ' ' : 's',
                        functions[k].caller,
                        FindFunctionName(functions[k].caller));
                }
            }
        }
        gap = 0;
        printf("\n");
        self = functions[i].totalTicks - functions[i].childTicks;
        printf("\t selfTicks : %12" PRIu64 " (%6.1f%%)\n",
            self, percentage(self, grandTotal));
        printf("\tchildTicks : %12" PRIu64 " (%6.1f%%)\n",
            functions[i].childTicks, percentage(functions[i].childTicks, grandTotal));
        printf("\ttotalTicks : %12" PRIu64 " (%6.1f%%)\n",
            functions[i].totalTicks, percentage(functions[i].totalTicks, grandTotal));
        for (j = 0; j < functions[i].children; j++) {
            if (gap == 0) {
                gap = 1;
                printf("\n");
            }
            printf("\t%8d call%c to x%04x %s\n",
                functions[i].calls[j].count,
                (functions[i].calls[j].count == 1) ? ' ' : 's',
                functions[i].calls[j].callee,
                FindFunctionName(functions[i].calls[j].callee));
        }
    }
}

#define JSR_RTS_DEPTH 128 // the real hardware stack is 256 bytes
typedef struct JsrRtsStackEntry {
    uint16_t pc;
    uint64_t clock;
} JsrRtsStackEntry;

static JsrRtsStackEntry profileStack[JSR_RTS_DEPTH];
static int profilePtr = 0;

void ProfileJSR(uint16_t pc) {
    uint16_t currentFunc = profileStack[profilePtr - 1].pc;
    profileStack[profilePtr].pc = pc;
    profileStack[profilePtr].clock = Peripherals.Counter.ClockCycles;
    profilePtr++;
    ProfileLogCall(currentFunc, pc);
}

void ProfileRTS(void) {
    uint16_t callee = profileStack[profilePtr - 1].pc;
    uint16_t caller = profileStack[profilePtr - 2].pc;
    uint64_t start = profileStack[profilePtr - 1].clock;
    profilePtr--;
    ProfileLogReturn(caller, callee, Peripherals.Counter.ClockCycles - start);
}

void ProfileReset(uint16_t pc) {
    static int init = 0;

    if (!init) {
        init = 1;
        atexit(ProfileDump);
    }

    resetVector = pc;
    profilePtr = 0;
    profileStack[profilePtr].pc = pc;
    profileStack[profilePtr].clock = Peripherals.Counter.ClockCycles;
    profilePtr++;
}
