/*****************************************************************************/
/*                                                                           */
/*                               bsearchcheck.h                              */
/*                                                                           */
/*                  Used to check sorting of various tables                  */
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

#ifndef _INCLUDE_BSEARCHCHECK_H_
#define _INCLUDE_BSEARCHCHECK_H_

#define BSEARCH_CHECK_HELPER(name, count, pre, post)                                         \
    static void Check ## name ## Sorted(void) {                                              \
        size_t I;                                                                            \
        for (I= 1; I < count; ++I) {                                                         \
            if (strcmp(pre[I-1]post, pre[I]post) >= 0) {                                     \
                fprintf(stderr,                                                              \
                    "%s:%d :: " #name " not sorted at index %zu\n", __FILE__, __LINE__, I);  \
                fprintf(stderr,                                                              \
                    "    %s < %s\n",                                                         \
                    pre[I-1]post, pre[I]post);                                               \
                abort();                                                                     \
            }                                                                                \
        }                                                                                    \
    }

#if defined(__GNUC__) || defined(__clang__)

#define BSEARCH_CHECK(name, count, pre, post) \
    __attribute__((constructor)) BSEARCH_CHECK_HELPER(name,count,pre,post)

#elif defined(_MSC_VER)

#pragma section(".CRT$XCU", read)
#define BSEARCH_CHECK(name, count, pre, post) \
    BSEARCH_CHECK_HELPER(name,count,pre,post) \
    __declspec(allocate(".CRT$XCU")) static void (*p_ ## name)(void) = Check ## name ## Sorted;

#else

#define BSEARCH_CHECK(name, count, pre, post)

#endif

#endif // _INCLUDE_BSEARCHCHECK_H_
