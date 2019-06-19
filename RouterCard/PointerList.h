/*
 * PointerList.h
 *
 * Created: 12/9/2016 7:15:42 PM
 *  Author: reed
 */ 


#ifndef POINTERLIST_H_
#define POINTERLIST_H_

/*this library is to create a FIFO list that contains pointers to an array, and a counter to keep track of which element of the array you are interested in. 

     (CUrrent node you are working on) head>node>node>node> (New Nodes Added Here)
*/

	typedef struct Node {
		int* data;
		struct Node *next;
		int count; 
		int ArraySize; 
		unsigned int address;
		unsigned int senderAddress; 
	} Node;

	
	void List_Init(struct Node** head);
	void List_Free(struct Node** head);
	void List_Push(struct Node** head, int *value, int ArraySize, unsigned int address, unsigned int senderAddress);
	int List_Pop(struct Node** head); 
	
#endif /* POINTERLIST_H_ */