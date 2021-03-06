#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include "indexed_list.h"


struct LOG
{
	/* data */
  int x;
  int y;
  int logID;
  boolean hasFrog;
  //enum type;
  pthread_mutex_t log_mutex;
  pthread_mutexattr_t log_attr;
};

struct NODE
{
	char *string; // needs to be a log struct
	Log *data; // a log struct pointes
	Node *next;
};


struct LIST {
	Node *top;
	Node *index[256];
};



static int total_num_traversals = 0;

//test the vairants
void validate_list(List *list){
    assert(NULL != list);
    assert(NULL != list->index);
}

// construct a empty linked list
List *construct() {
	List *list;
    int i;

	list = malloc( sizeof( List ) );
	list->top = NULL;
    for(i = 0; i < 256; i++)
    list->index[i] = NULL;
    
	return list;
}

// perform an ordered insertion of an item into a list
boolean insert( List *list, Log *toInsert)
{
	boolean rc = TRUE;

	Node *newNode = NULL;
    Node *curr = NULL;
    
    //creating a toInsert Node
	newNode = malloc( sizeof( Node ) );
	newNode->data = toInsert;
	newNode->next = NULL;

	curr = list->top; //getting the list top

	while(curr != NULL){
		curr = curr->next;
	}

	curr = newNode;//inserting

	return rc;
}

boolean removeFirst( List *list ){
	boolean rc = TRUE;

    Node *curr = NULL;

    curr = list->top; //get a top pointer

    list->top = curr->next; //reset top pointer

    destroyNode(curr); //free up memory

    return rc;

}

boolean removeWithId( List *list, int logId){
	boolean rc = FALSE;
	int isFound = -1;


    Node *curr = NULL;
    Node *prev = NULL;

    curr = list->top; //get a top pointer

    while(NULL != curr && isFound < 0){
    	if(curr->data->logID == logId){
    		isFound+=10;
    		rc = TRUE;
    	}else{
    		prev = curr;
    		curr=curr->next;


    	}
    }

    //at this point we are pointing to the one we want to remove
    //remove it
    
    if(isFound > 0){
    	prev->next = curr->next;
    	destroyNode(curr);
    }

    return rc;

}


// calculates the number of nodes in a list
int node_count( List *list )
{
	int count;
	Node *curr = list->top;

	count = 0;
	while ( NULL != curr ) {
		count++;
		curr = curr->next;
	}

	return count;
}

// calculates the size of a list
int size( List *list )
{
	return node_count( list );
}

// print the contents of a list, one item per line


// returns the total number of items traversed in the list
int traversals()
{
	return total_num_traversals;
}

// destroy the nodes in a list
void destroy( List *list )
{
	Node *curr = list->top;
	Node *next;

	while ( NULL != curr ) {
		next = curr->next;
		free( curr->data);
		free( curr );
		curr = next;
	}
	
	free( list );
    free(list->index);
    
}

void destroyNode( Node *node){

	free(node->data);
	free(node);

}
