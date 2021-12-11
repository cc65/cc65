/*
  !!DESCRIPTION!! print word frequencies; uses structures
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXWORDS 250

FILE *in;
#define getchar() fgetc(in)

struct node
{
        int count;              /* frequency count */
        struct node *left;      /* left subtree */
        struct node *right;     /* right subtree */
        char *word;             /* word itself */
} words[MAXWORDS];
int next;               /* index of next free entry in words */

/*struct node *lookup();*/

int err(char *s);
int getword(char *buf);
void tprint(struct node *tree);
struct node *lookup(char *word, struct node **p);

int isletter(char c);

/* err - print error message s and die  */
err(s) char *s; {
        printf("? %s\n", s);
        exit(EXIT_FAILURE);
}

/* getword - get next input word into buf, return 0 on EOF */
int getword(buf) char *buf;
{
        char *s;
        int c;

        while (((c = getchar()) != -1) && (isletter(c) == 0))
                ;
        for (s = buf; (c = isletter(c)); c = getchar())
                *s++ = c;
        *s = 0;
        if (s > buf)
                return 1;
        return 0;
}

/* isletter - return folded version of c if it is a letter, 0 otherwise */
int isletter(char c)
{
        if ((c >= 'A') && (c <= 'Z')) c += 'a' - 'A';
        if ((c >= 'a') && (c <= 'z')) return c;
        return 0;
}

/* lookup - lookup word in tree; install if necessary */
struct node *lookup(word, p)
char *word; struct node **p;
{
        int cond;
/*      char *malloc(); */

        if (*p) {
                cond = strcmp(word, (*p)->word);
                if (cond < 0)
                        return lookup(word, &(*p)->left);
                else if (cond > 0)
                        return lookup(word, &(*p)->right);
                else
                        return *p;
        }
        if (next >= MAXWORDS)
                err("out of node storage");
        words[next].count = 0;
        words[next].left = words[next].right = 0;
        words[next].word = malloc(strlen(word) + 1);
        if (words[next].word == 0)
                err("out of word storage");
        strcpy(words[next].word, word);
        return *p = &words[next++];
}

/* tprint - print tree */
void tprint(tree) struct node *tree; {
        if (tree) {
                tprint(tree->left);
                printf("%d:%s\n", tree->count, tree->word);
                tprint(tree->right);
        }
}

int main(void)
{
        struct node *root;
        char word[20];

        in = fopen("wf1.in","rb");
        if (in == NULL) {
            return EXIT_FAILURE;
        }

        root = 0;
        next = 0;
        while (getword(word))
                lookup(word, &root)->count++;
        tprint(root);

        fclose(in);
        return 0;
}
