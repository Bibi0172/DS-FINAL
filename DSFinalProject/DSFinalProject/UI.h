#ifndef UI_H
#define UI_H

#include "SMS.h"

#define MAX_MEETING_SLOT_COUNT	3
#define LOG_KEY_LENGHT	10
#define MAX_LOG_LENGTH	150
#define MAX_BUFFER_SIZE	50
#define MIN_STUDENT_ID 100000
#define MAX_STUDENT_ID 999999

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
int getValidDateChoice(HashTable* ht);
//int getValidTimeSlot(Queue* q);
void getValidUserInput(HashTable* ht, int* studentID, char* name, char* title);
int isDuplicateName(HashTable* ht, const char* name);
int isDuplicateID(HashTable* ht, int studentID);
//int isTimeSlotBooked(Queue* q, int time);

void undoLastOperation(HashTable* ht, OperationStack* opStack, KVP** logList);
void displayKVPLog(KVP* operationLog);

int getUserInput();

#endif
