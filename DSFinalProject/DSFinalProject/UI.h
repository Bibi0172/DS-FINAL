#ifndef UI_H
#define UI_H

#include "SMS.h"

#define MAX_MEETING_SLOT_COUNT	3

typedef enum {
	BOOK = 1,
	CANCEL,
	SEARCH,
	VIEW,
	UNDO,
	DISPLAY_LOG,
	EXIT_PROGRAM,
} KIND_OF_MENU;

void displayMenu(HashTable* ht, OperationStack* opStack, KVP** logList);
int validMenuChoice(int maxNumber);
void cleanBuffer();
int getValidDateChoice();
int getValidTimeSlot(Queue* q);
void getValidUserInput(int* studentID, char* name, char* title);
int isTimeSlotBooked(Queue* q, int time);

void undoLastOperation(HashTable* ht, OperationStack* opStack, KVP** logList);
void displayKVPLog(KVP* operationLog);

int getUserInput();

#endif
