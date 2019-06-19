/*
 * PointerList.c
 *
 * Created: 12/9/2016 6:42:16 PM
 *  Author: reed
 */ 


#include <stdlib.h>
#include "PointerList.h"
#include <avr/io.h>
	

 
	//point head to null
	void List_Init(struct Node** head) {
		*head = NULL;
	}
	
	//frees each node and array in node 
	void List_Free(struct Node** head) {
		while(List_Pop(head));
	}

	//adds node to end of list, and adds dynamic array and the size of the array
	void List_Push(struct Node** head, int *value, int ArraySize, unsigned int address, unsigned int senderAddress) {
		struct Node *n = (struct Node *)malloc(sizeof(struct Node));
		if(n == NULL)
			{
				PORTA &= ~(1
				);
				return;
			}
		struct Node *temp; 
		if(*head == NULL) {
			n->data = value;
			n->ArraySize = ArraySize;
			n->count = 0;
			n->next = NULL;  
			n->address = address; 
			n->senderAddress = senderAddress;
			*head = n;
			return; 
		}
		temp = *head;
		while(temp->next != NULL) {
				temp = temp->next; 
		}
		n->data = value;
		n->ArraySize = ArraySize;
		n->count = 0;  
		n->address = address; 
		n->senderAddress = senderAddress;
		n->next = NULL;
		temp->next = n;
	}
	
	//frees allocated array in node and node
	int List_Pop(struct Node** head) {
		if(*head == NULL){
			return 0; 
		}
		if((*head)->next != NULL) {
			struct Node *n = (*head)->next;
			if((*head)->data != NULL)
			{
				free((*head)->data);
				(*head)->data = NULL;
			}
			free(*head);
			*head = n;
		}
		else{
			if((*head)->data != NULL)
			{
				free((*head)->data);
				(*head)->data = NULL; 
			}
			free(*head);
			*head = NULL; 
		}
		return 1; 
	}
