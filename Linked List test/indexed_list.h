//
//  indexed_list.h
//  a3_q1
//
//  Created by Lew Ivanchuk on 11-11-15.
//  Copyright (c) 2011 University of Manitoba. All rights reserved.
//

#ifndef a3_q1_indexed_list_h
#define a3_q1_indexed_list_h
#include "log.h"

typedef int boolean;
#define TRUE 1
#define FALSE 0

typedef struct NODE Node;
typedef struct LIST List;

List *construct();
boolean insert( List *list, Log *data );
boolean removeFirst( List *list );
boolean removeWithId( List *list, int logId );
int node_count( List *list );
int size( List *list );
void print( List *list );
int traversals();
void destroy( List *list );
void destroyNode( Node *node );

#endif
