#ifndef SMS_H
#define SMS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TOTAL_DAYS 7
#define MAX_SLOTS 3
#define MAX_NAME_LENGTH     50
#define MAX_TITLE_LENGTH    100
#define MAX_DATE_LENGTH     12
#define MAX_KEY_LENGTH_KVP   50
#define MAX_VALUE_LENGTH_KVP 100

extern char* dates[TOTAL_DAYS];

// Meeting node structure used in the Queue (implemented as a linked list)
typedef struct Meeting {
	int studentID;
	char name[MAX_NAME_LENGTH];
	char title[MAX_TITLE_LENGTH];
	int time;
	struct Meeting* next;
} Meeting;

// Queue structure (FIFO) used to manage meeting nodes for each date
typedef struct Queue {
	Meeting* front;
	Meeting* back;
	int count;
} Queue;

// HashTable structure used to map a date to its meeting queue
typedef struct HashTable {
	char date[MAX_DATE_LENGTH];
	Queue* meetingQueue;
} HashTable;

// Stack Implementation for Undo
typedef enum {
	OP_BOOK,
	OP_CANCEL
} OperationType;

// Structure to record a meeting operation (booking or cancellation)
typedef struct Operation {
	OperationType type;
	char date[MAX_DATE_LENGTH];
	int studentID;
	char name[MAX_NAME_LENGTH];
	char title[MAX_TITLE_LENGTH];
	int timeSlot;
} Operation;

// Node for the operation stack
typedef struct StackNode {
	Operation op;
	struct StackNode* next;
} StackNode;

// Stack structure for storing operations that can be undone
typedef struct OperationStack {
	StackNode* top;
} OperationStack;

void initializeHashTable(HashTable* ht);
int hashDate(char* date);
int findHashIndex(HashTable* ht, char* date);

void insertMeeting(HashTable* ht, char* date, int studentID, char* name, char* title, int time);

// cancelMeeting now returns an operation record used for undo functionality
Operation* cancelMeeting(HashTable* ht, char* date, int studentID);

void searchMeeting(HashTable* ht, char* date, int studentID);
void viewUpcomingMeetings(HashTable* ht);
void freeAllDataStructures(HashTable* meetingTable);

// Undoing operations:
int removeMeeting(HashTable* ht, Operation* op);
int reinsertMeeting(HashTable* ht, Operation* op);

void initStack(OperationStack* stack);
bool isStackEmpty(OperationStack* stack);
void pushStack(OperationStack* stack, Operation op);
int popStack(OperationStack* stack, Operation* op);
void freeStack(OperationStack* stack);

// KVP Log (Key-Value Pair) Implementation
typedef struct KVP {
	char key[MAX_KEY_LENGTH_KVP];
	char value[MAX_VALUE_LENGTH_KVP];
	struct KVP* next;
} KVP;

void insertKVP(KVP** head, char* key, char* value);
char* searchKVP(KVP* head, char* key);
void freeKVP(KVP* head);

#endif
