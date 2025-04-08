#include "UI.h"

int main() {
	HashTable meetingTable[TOTAL_DAYS];
	initializeHashTable(meetingTable);

	OperationStack opStack;
	initStack(&opStack);

	KVP* logList = NULL;

	displayMenu(meetingTable, &opStack, &logList);

	freeAllDataStructures(meetingTable);
	freeStack(&opStack);
	freeKVP(logList);

	return 0;
}