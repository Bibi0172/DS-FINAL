#include "SMS.h"

#ifndef UI_H
#define UI_H

#define MAX_MEETING_SLOT_COUNT	3

typedef enum {
	BOOK = 1,
	CANCEL,
	SEARCH,
	VIEW,
	EXIT_PROGRAM,
} KIND_OF_MENU;

void displayMenu(HashTable* ht);
int validMenuChoice(int maxNumber);
void cleanBuffer();
int getValidDateChoice();
int getValidTimeSlot(Queue* q);
void getValidUserInput(int* studentID, char* name, char* title);
int isTimeSlotBooked(Queue* q, int time);
#endif
