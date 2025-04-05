#ifndef UI_H
#define UI_H

#include "SMS.h"


void displayMenu(HashTable* ht);
int validMenuChoice(int maxNumber);
void cleanBuffer();
int getValidDateChoice();
int getValidTimeSlot(Queue* q);
void getValidUserInput(int* studentID, char* name, char* title);
int isTimeSlotBooked(Queue* q, int time);
#endif
