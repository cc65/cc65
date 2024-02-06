/*
  !!DESCRIPTION!! Dijkstras Algorithm
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Groepaz/Hitmen
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

#define DIJKSTRA_INFINITY       (0xffff)

#define DIJKSTRA_FLAG_UNVISITED (0x0)
/*
#define DIJKSTRA_FLAG_OPEN      (0x1)
*/
#define DIJKSTRA_FLAG_CLOSED    (0x2)

typedef struct _DIJKSTRA_EDGE {
    struct _DIJKSTRA_NODE *NEXTNODE;
    unsigned short DISTANCE;
} DIJKSTRA_EDGE;

typedef struct _DIJKSTRA_NODE {
    DIJKSTRA_EDGE *EDGES;
    unsigned char  TAG;
    unsigned char  FLAG;
    unsigned short MINDIST;
    struct _DIJKSTRA_NODE *PREVIOUS;
} DIJKSTRA_NODE;

/* init with graph, startnode, working-array */
void Dijkstra_Init(const DIJKSTRA_NODE *graph,DIJKSTRA_NODE *start,DIJKSTRA_NODE *nodes);

/* call main algo with working-array */
void Dijkstra_Search(DIJKSTRA_NODE *nodes);

/* print path, call with working-array, endnode */
void Dijkstra_Path(DIJKSTRA_NODE *nodes,DIJKSTRA_NODE *end);

/* print table, call with working-array, current node */
void Dijkstra_Table(DIJKSTRA_NODE *nodes,DIJKSTRA_NODE *current);

/* internally used routines */

unsigned short Dijkstra_Distance(DIJKSTRA_NODE *currnode,DIJKSTRA_NODE *nextnode);
void Dijkstra_Relax(DIJKSTRA_NODE *currnode,DIJKSTRA_NODE *nextnode);
DIJKSTRA_NODE *Dijkstra_NextCheapest(DIJKSTRA_NODE *graph);
unsigned short Dijkstra_CountUnvisited(DIJKSTRA_NODE *graph);

/* define to get printed info as the algorithm proceeds */
#define DIJKSTRA_PRINTDEBUG

/* the working array */
DIJKSTRA_NODE mynodes[0x10];

/* test-network data (mypoints and myedges) */

const DIJKSTRA_EDGE myedges_A[]={
    {&mynodes[1],1},
    {NULL}
};
const DIJKSTRA_EDGE myedges_B[]={
    {&mynodes[0],1},
    {&mynodes[2],2},
    {&mynodes[3],1},
    {NULL}
};
const DIJKSTRA_EDGE myedges_C[]={
    {&mynodes[1],2},
    {&mynodes[5],1},
    {NULL}
};
const DIJKSTRA_EDGE myedges_D[]={
    {&mynodes[1],1},
    {&mynodes[4],1},
    {NULL}
};
const DIJKSTRA_EDGE myedges_E[]={
    {&mynodes[6],1},
    {NULL}
};
const DIJKSTRA_EDGE myedges_F[]={
    {&mynodes[7],1},
    {NULL}
};
const DIJKSTRA_EDGE myedges_G[]={
    {&mynodes[8],1},
    {&mynodes[7],4},
    {NULL}
};
const DIJKSTRA_EDGE myedges_H[]={
    {&mynodes[9],1},
    {&mynodes[6],4},
    {NULL}
};
const DIJKSTRA_EDGE myedges_I[]={
    {&mynodes[10],5},
    {NULL}
};
const DIJKSTRA_EDGE myedges_J[]={
    {&mynodes[10],1},
    {NULL}
};
const DIJKSTRA_EDGE myedges_K[]={
    {&mynodes[11],1},
    {NULL}
};
const DIJKSTRA_EDGE myedges_L[]={
    {&mynodes[10],1},
    {NULL}
};

const DIJKSTRA_NODE mypoints[]={
    {(DIJKSTRA_EDGE *)&myedges_A[0],'A'},
    {(DIJKSTRA_EDGE *)&myedges_B[0],'B'},
    {(DIJKSTRA_EDGE *)&myedges_C[0],'C'},
    {(DIJKSTRA_EDGE *)&myedges_D[0],'D'},
    {(DIJKSTRA_EDGE *)&myedges_E[0],'E'},
    {(DIJKSTRA_EDGE *)&myedges_F[0],'F'},
    {(DIJKSTRA_EDGE *)&myedges_G[0],'G'},
    {(DIJKSTRA_EDGE *)&myedges_H[0],'H'},
    {(DIJKSTRA_EDGE *)&myedges_I[0],'I'},
    {(DIJKSTRA_EDGE *)&myedges_J[0],'J'},
    {(DIJKSTRA_EDGE *)&myedges_K[0],'K'},
    {(DIJKSTRA_EDGE *)&myedges_L[0],'L'},
    {NULL}
};

/*
 *      initialize working-array
 */

void Dijkstra_Init(const DIJKSTRA_NODE *graph,DIJKSTRA_NODE *start,DIJKSTRA_NODE *nodes) {
    while(graph->EDGES!=NULL) {
        nodes->EDGES=graph->EDGES;
        nodes->TAG=graph->TAG;
        nodes->FLAG=DIJKSTRA_FLAG_UNVISITED;
        nodes->MINDIST=DIJKSTRA_INFINITY;
        nodes->PREVIOUS=NULL;

        graph++;nodes++;

    }
/*
    start->FLAG=DIJKSTRA_FLAG_OPEN;
    start->PREVIOUS=NULL;
 */
    start->MINDIST=0;
}

/*
 *      compute the distance between two Nodes in the Graph
 */

unsigned short Dijkstra_Distance(DIJKSTRA_NODE *currnode,DIJKSTRA_NODE *nextnode){
DIJKSTRA_EDGE *edge;

    edge=currnode->EDGES;

    while(edge!=NULL) {
        if(edge->NEXTNODE == nextnode){
            return(edge->DISTANCE);
        }

        edge++;

    }

    return(DIJKSTRA_INFINITY);
}

/*
 *      'relax' one node against another
 */

void Dijkstra_Relax(DIJKSTRA_NODE *currnode,DIJKSTRA_NODE *nextnode){
unsigned short newdist;

#ifdef DIJKSTRA_PRINTDEBUG
    printf("relax >%c< to >%c<\n",currnode->TAG,nextnode->TAG);
#endif

    newdist=currnode->MINDIST+Dijkstra_Distance(currnode,nextnode);

    if((nextnode->MINDIST)>(newdist)){
        nextnode->MINDIST=newdist;
        nextnode->PREVIOUS=currnode;

    }
}

/*
 *      find the yet unprocessed Node with the currently
 *      smallest estimated MINDIST
 */

DIJKSTRA_NODE *Dijkstra_NextCheapest(DIJKSTRA_NODE *graph){
unsigned short mindist;
DIJKSTRA_NODE *node;

    node=NULL;
    mindist=DIJKSTRA_INFINITY;

    while(graph->EDGES!=NULL) {
        if(graph->FLAG!=DIJKSTRA_FLAG_CLOSED){
            if(!(mindist<graph->MINDIST)){
                     mindist=graph->MINDIST;
                     node=graph;
            }
        }

        graph++;

    }

#ifdef DIJKSTRA_PRINTDEBUG
    if(node!=NULL) printf("next cheapest Node: >%c<\n",node->TAG);
#endif

    return(node);
}

/*
 *      count number of Nodes that are left for processing
 */

unsigned short Dijkstra_CountUnvisited(DIJKSTRA_NODE *graph){
unsigned short num;

    num=0;

    while(graph->EDGES!=NULL) {
        if(graph->FLAG!=DIJKSTRA_FLAG_CLOSED){
            num++;
        }

        graph++;

    }

    return(num);
}

/*
 *      Dijkstra-Algorithmus main processing
 */

void Dijkstra_Search(DIJKSTRA_NODE *graph){
DIJKSTRA_NODE *currnode,*nextnode;
DIJKSTRA_EDGE *edge;

    currnode=graph;

    while(Dijkstra_CountUnvisited(graph)>0){
        edge=currnode->EDGES;
        while(edge->NEXTNODE!=NULL){
            nextnode=edge->NEXTNODE;
            if(nextnode->FLAG!=DIJKSTRA_FLAG_CLOSED){
/*
   nextnode->FLAG=DIJKSTRA_FLAG_OPEN;
 */
                Dijkstra_Relax(currnode,nextnode);
#ifdef DIJKSTRA_PRINTDEBUG
                Dijkstra_Table(graph,currnode);
#endif
            }
            edge++;
        }
        currnode=Dijkstra_NextCheapest(graph);
        currnode->FLAG=DIJKSTRA_FLAG_CLOSED;
    }
}

/*
 *      print the Path from start Node to one other Node
 */

void Dijkstra_Path(DIJKSTRA_NODE *graph,DIJKSTRA_NODE *end){
DIJKSTRA_NODE *currnode,*nextnode;

    printf("Path from >%c< to >%c< : ",end->TAG,graph->TAG);

    currnode=end;

        while(currnode->PREVIOUS!=NULL){
            printf(">%c< ",currnode->TAG);
            currnode=currnode->PREVIOUS;
        }

    printf(">%c<\n",currnode->TAG);
}

/*
 *      print working-array as a table
 */

void Dijkstra_Table(DIJKSTRA_NODE *graph,DIJKSTRA_NODE *current){
DIJKSTRA_NODE *g;

    printf("----------------------\n");

    printf("Node    |");
    g=graph;while(g->EDGES!=NULL) {
        printf("-->%c<-|",g->TAG);
        g++;
    }
    printf("\n");

    printf("MinDist |");
    g=graph;while(g->EDGES!=NULL) {
        printf(" %5u|",g->MINDIST);
        g++;
    }
    printf("\n");

    printf("Flag    |");
    g=graph;while(g->EDGES!=NULL) {
        switch(g->FLAG){
/*
            case DIJKSTRA_FLAG_OPEN:
                printf("opened|");
                break;
 */
            case DIJKSTRA_FLAG_CLOSED:
                printf("closed|");
                break;
            default:
                if(g->MINDIST!=DIJKSTRA_INFINITY){
                    printf("opened|");
                } else {
                    printf("------|");
                }
                break;
        }
        g++;
    }
    printf("\n");

    printf("Previous|");
    g=graph;while(g->EDGES!=NULL) {
        if(g->PREVIOUS==NULL)
            printf("------|");
        else
            printf("  (%c) |",g->PREVIOUS->TAG);
        g++;
    }
    printf("\n");

    printf("----------------------\n");
}

int main(void)
{
    /* init with graph, startnode, working-array */
    Dijkstra_Init(&mypoints[0],&mynodes[0],&mynodes[0]);
    /* call main algo with working-array */
    Dijkstra_Search(&mynodes[0]);
    /* print table, call with working-array, endnode */
    Dijkstra_Table(&mynodes[0],&mynodes[11]);
    /* print path, call with working-array, endnode */
    Dijkstra_Path(&mynodes[0],&mynodes[11]);

    return 0;
}
