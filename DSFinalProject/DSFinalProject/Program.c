#include "UI.h"

int main() {
	HashTable meetingTable[TOTAL_DAYS];
	OperationStack opStack;
	KVP* logList = NULL;
	ProcessedMeetingStack processedStack;

	initializeHashTable(meetingTable);
	initStack(&opStack);
	initProcessedStack(&processedStack);

	displayMenu(meetingTable, &opStack, &logList, &processedStack);

	freeMeetingTable(meetingTable);
	freeStack(&opStack);
	freeKVP(logList);

	return 0;
}