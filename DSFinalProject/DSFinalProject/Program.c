#include "UI.h"

int main() {
    HashTable meetingTable[TOTAL_DAYS];
    initializeHashTable(meetingTable); 
    displayMenu(meetingTable);         

	freeAllDataStructures(meetingTable);
    return 0;
}

void initializeHashTable(HashTable* ht) {

	for (int i = 0; i < TOTAL_DAYS; i++) {
		strcpy(ht[i].date, dates[i]);
		ht[i].meetingQueue = (Queue*)malloc(sizeof(Queue));

		if (ht[i].meetingQueue == NULL) {
			printf("Fail to allocate a memory\n");
			exit(EXIT_FAILURE);
		}

		ht[i].meetingQueue->front = ht[i].meetingQueue->back = NULL;
		ht[i].meetingQueue->count = 0;
	}
}

void freeAllDataStructures(HashTable* meetingTable) {
	if (meetingTable == NULL) {
		return;
	}

	for (int i = 0; i < TOTAL_DAYS; i++) {
		Queue* q = meetingTable[i].meetingQueue;
		if (q != NULL) {
			// Free all meeting nodes in the queue
			Meeting* current = q->front;
			while (current != NULL) {
				Meeting* next = current->next;
				free(current);
				current = next;
			}
			// Free the queue structure
			free(q);
			meetingTable[i].meetingQueue = NULL;
		}
	}
}