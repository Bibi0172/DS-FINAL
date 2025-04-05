#include "UI.h"

int main() {
    HashTable meetingTable[TOTAL_DAYS];
    initializeHashTable(meetingTable); 
    displayMenu(meetingTable);         
    return 0;
}

void initializeHashTable(HashTable* ht) {

	for (int i = 0; i < TOTAL_DAYS; i++) {
		strcpy(ht[i].date, dates[i]);
		ht[i].meetingQueue = (Queue*)malloc(sizeof(Queue));
		ht[i].meetingQueue->front = ht[i].meetingQueue->back = NULL;
		ht[i].meetingQueue->count = 0;
	}
}