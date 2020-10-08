/*
 * Sean Keough
 * GitHub: SKeough117
 *
 * set.c - CS50 'set' module
 *
 * see set.h for more information.
 *
 * Sean Keough, January 2020
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "set.h"
#include "memory.h"

/******************* file-local global variables ****************/
/* none */

/********* local types ************/

typedef struct setnode {
  void *item;		// pointer to value stored with key
  char *key;	// pointer to string key for value
  struct setnode *next; 
} setnode_t;

/***************** global types *******************/

typedef struct set {
	struct setnode *head;	// head of the list of nodes in the set
} set_t;

/*************** global functions *****************/
/* functions that are visible outside this file */
/* see set.h for comments 			*/

/************** local functions ***************/
/* not visible outside this file */

set_t *set_new(void);

bool set_insert(set_t *set, const char *key, void *item);

setnode_t *setnode_new(const char *key, void *item);

void set_delete(set_t *set, void(*itemdelete)(void *item));

void set_iterate(set_t *set, void *arg, void(*itemfunc)(void *arg, const char *key, void *item));

void set_print(set_t *set, FILE *fp, void(*itemprint)(FILE *fp, const char *key, void *item));

/*************** set_new() *****************/

set_t *set_new(void)
{
	// Allocate memory for set and set pointer
	set_t *set = malloc(sizeof(set_t));

	// If the pointer is null (no items), then print an error
	if (set == NULL) {
		
		printf("Error: Set equal to Null\n");

		return NULL;
	} else {
		set->head = NULL;
		return set;
	}
}

/******************** set_insert() ********************/
bool set_insert(set_t *set, const char *key, void *item)
{
	// Check for set and item validity
	if (set != NULL && item != NULL)
	{
		if(key != NULL) {
			// Loop through the list, checking for identical keys
			// allocate a new node for the linked list
			setnode_t *new = setnode_new(key, item);
	    	if (new != NULL) {
				// add the node to the head of the set list
				new->next = set->head;
				set->head = new;
				return true;
			}
            
		}

        
	}
	fprintf(stdout, "Set_Insert Error: parameter passed not valid\n");
	return false;
}

void set_set(set_t *set, const char *key, char *newkey){
    if(set_find(set,key)){
        setnode_t *current_node = set->head;
        while(current_node != NULL) {
        // Find identical key for resetting
            if(strcmp(current_node->key, key) == 0) {
            current_node->key = newkey;
            }
            current_node = current_node->next;
        }
    }

}

/***************** setnode_new **********************/
/* Allocate and initialize a setnode  */ 
setnode_t *setnode_new(const char *key, void *item)
{
  setnode_t *node = malloc(sizeof(setnode_t));

  if (node == NULL) {
	  // error allocating memory for node; returns data
	  return NULL;
  } else {
	
	node->key = malloc(sizeof(char)*(strlen(key)+1));
	node->item = malloc(sizeof(char)*(strlen(item)+1));
	strcpy(node->key, key);
	strcpy(node->item,item);
	node->next = NULL;

	return node;
  }
}

/************************ set_find() **************************/
void *set_find(set_t *set, const char *key) {
	if(set == NULL || key == NULL) {
		return NULL; // bad set or key
	} else {
		
		setnode_t *curr_node = set->head;

		while(curr_node != NULL) {
			if(strcmp(key, curr_node->key) == 0) {
				return curr_node->item;
			}
			curr_node = curr_node->next;
		}
		return NULL;
	}
		
}

void *set_find2(set_t *set, const char *key1, const char *key2) {

	if(set == NULL || key1 == NULL || key2 == NULL) {
		return NULL; // bad set or key
	} else {
		
		setnode_t *curr_node = set->head;

		while(curr_node != NULL) {
			if((strcmp(key1, curr_node->key) == 0) && (strcmp(key2, curr_node->item) == 0)) {
				return curr_node->item;
			}
			if((strcmp(key2, curr_node->key) == 0) && (strcmp(key1, curr_node->item) == 0)) {
				return curr_node->item;
			}
			curr_node = curr_node->next;
		}
		return NULL;
	}
		
}
	

/*************************** set_print() **********************************/
void set_print(set_t *set, FILE *fp, void (*itemprint)(FILE *fp, const char *key, void *item) ) 
{
	// Check file path validity
	if (fp != NULL) {
		// Check set validity
		if (set != NULL && itemprint != NULL) {

			// fprintf(fp, "{ KEY : VALUE }\n");
            //     	fprintf(fp, "----------------\n");

			// Loop through all the nodes
			for(setnode_t *node = set->head; node != NULL; node = node->next) {
				// Check validity of function passed
				if(itemprint != NULL) {

					// print the node's key and item 
					(*itemprint)(fp, node->key, node->item);
					}
				}
			}
		} else {
		fputs("Error: null set or function pointers\n", fp);
		}
}


/*************************** set_iterate() *****************************/
void set_iterate(set_t *set, void *arg, void(*itemfunc)(void *arg, const char *key, void *item))
{
	if(set != NULL && arg != NULL) {
	
		if(itemfunc != NULL) {
		// call the (*itemfunc) with arg on each key and item
			for(setnode_t *node = set->head; node != NULL; node = node->next) {
				(*itemfunc)(arg, node->key, node->item);
			}
		}
	}
}


/************************** set_delete() *******************/
void set_delete(set_t *set, void(*itemdelete)(void *item))
{
 if (set != NULL) {
        for (setnode_t *pair = set->head; pair != NULL;) {
            if (itemdelete != NULL) {
                (*itemdelete)(pair->item);
            }
            setnode_t *next = pair->next; // Store next pair
            count_free(pair->key);
            count_free(pair);
            pair = next; // Iterate
        }
       count_free(set);
    }

}

