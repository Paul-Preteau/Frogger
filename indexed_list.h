//
//  indexed_list.h
//  a3_q1
//
//  Created by Lew Ivanchuk on 11-11-15.
//  Copyright (c) 2011 University of Manitoba. All rights reserved.
//

#ifndef INDEXED_LIST_H
#define INDEXED_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>



typedef int boolean;
#define TRUE 1
#define FALSE 0

typedef struct NODE Node;
typedef struct LIST List;
typedef struct LOG Log;

List *construct();
//boolean insert( List *list, Log *data );
boolean removeFirst( List *list );
boolean removeWithId( List *list, int logId );
int node_count( List *list );
int size( List *list );
void print( List *list );
int traversals();
void destroy( List *list );
void destroyNode( Node *node );

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
List* construct() {
	List *list;
    int i;

	list = malloc( sizeof( List ) );
	list->top = NULL;
    for(i = 0; i < 256; i++)
    list->index[i] = NULL;
    
	return list;
}

boolean insert( List *list, Log *toInsert)
{
  boolean rc = TRUE;

  Node *newNode = NULL;
  Node *curr = NULL;
    
    //creating a toInsert Node
  newNode = malloc( sizeof( Node ) );
  assert(toInsert != NULL);
  newNode->data = toInsert;
  newNode->next = NULL;

  assert(newNode != NULL);
  assert(newNode->data != NULL);


  curr = list->top; //getting the list top

  if( curr == NULL){
    list->top = newNode;
  }else{

    while(curr->next != NULL){
    curr = curr->next;
  }

  curr->next = malloc( sizeof( Node ) );

  curr->next = newNode;
  curr = newNode;
  curr->next = NULL;

  }

  //assert(curr != NULL);

  



  return rc;
}

// perform an ordered insertion of an item into a list


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

    while(NULL != curr){
    	if(curr->data->logID == logId){
    		if(curr == list->top){
    			list->top = curr->next;
    			destroyNode(curr);
    			rc = TRUE;
    			return rc;
    		}else{
    		prev->next = curr->next;
    		destroyNode(curr);
    		rc = TRUE;
    		return rc;
    		
			}
		}else{
				prev=curr;
				curr=curr->next;
			}
		
    }

    //at this point we are pointing to the one we want to remove
    //remove it
  

    return rc;

}

void display_Id(List *list){
	  Node *curr=list->top;
    if(curr==NULL)
    {
    return;
    }
    while(curr!=NULL)
    {
    printf("%d\n",curr->data->logID);
    curr=curr->next;
    }
    printf("\n");
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

void lock_list( List *list ){
	Node *curr;
	curr = list->top;

	while(curr != NULL){
		pthread_mutex_lock(&curr->data->log_mutex);
		curr = curr->next;
	}

}


void unlock_list( List *list ){
	Node *curr;
	curr = list->top;

	while(curr != NULL){
		pthread_mutex_unlock(&curr->data->log_mutex);
		curr = curr->next;
	}

}



#endif


