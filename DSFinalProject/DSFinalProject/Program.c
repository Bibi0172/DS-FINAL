#include "UI.h"

int main() {
	HashTable meetingTable[TOTAL_DAYS];
	OperationStack opStack;
	KVP* logList = NULL;

	initializeHashTable(meetingTable);
	initStack(&opStack);

	displayMenu(meetingTable, &opStack, &logList);

	freeMeetingTable(meetingTable);
	freeStack(&opStack);
	freeKVP(logList);

	return 0;
}