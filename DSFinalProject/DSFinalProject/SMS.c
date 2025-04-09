#include "SMS.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The global array of dates
char* dates[TOTAL_DAYS] = { "May 1", "May 2", "May 3", "May 4", "May 5", "May 6", "May 7" };

void initializeHashTable(HashTable* ht) {
	for (int i = 0; i < TOTAL_DAYS; i++) {
		strcpy_s(ht[i].date, MAX_DATE_LENGTH, dates[i]);
		ht[i].meetingQueue = (Queue*)malloc(sizeof(Queue));
		if (ht[i].meetingQueue == NULL) {
			printf("Fail to allocate a memory\n");
			exit(EXIT_FAILURE);
		}

		ht[i].meetingQueue->front = ht[i].meetingQueue->back = NULL;
		ht[i].meetingQueue->count = 0;
	}
}

int hashDate(char* date) {
	int hash = 0;

	for (int i = 0; date[i] != '\0'; i++) {
		hash = 31 * hash + date[i];
	}

	return hash % TOTAL_DAYS;
}

int findHashIndex(HashTable* ht, char* date) {
	int hashIndex = hashDate(date);

	if (strcmp(ht[hashIndex].date, date) == 0) {
		return hashIndex;
	}

	for (int i = 0; i < TOTAL_DAYS; i++) {
		int probeIndex = (hashIndex + i) % TOTAL_DAYS;
		if (strcmp(ht[probeIndex].date, date) == 0) {
			return probeIndex;
		}
	}

	return -1;
}

void insertMeeting(HashTable* ht, char* date, int studentID, char* name, char* title, int time) {
	int index = findHashIndex(ht, date);

	if (index == -1) {
		printf("Invalid date! Please choose a valid date.\n");
		return;
	}
	Queue* q = ht[index].meetingQueue;

	// Check if time slot is already taken
	Meeting* current = q->front;
	while (current != NULL) {
		if (current->time == time) {
			printf("The %d time slot is already booked on %s. Please choose a different time.\n", time, date);
			return;
		}
		current = current->next;
	}

	// Check if maximum slots are reached
	if (q->count >= MAX_SLOTS) {
		printf("All slots are booked for %s. Please choose a different date.\n", date);
		return;
	}

	// Create new meeting node
	Meeting* newMeeting = (Meeting*)malloc(sizeof(Meeting));

	if (newMeeting == NULL) {
		printf("[insertMeeting] Insufficient memory\n");
		return;
	}

	newMeeting->studentID = studentID;
	strcpy_s(newMeeting->name, MAX_NAME_LENGTH, name);
	strcpy_s(newMeeting->title, MAX_TITLE_LENGTH, title);
	newMeeting->time = time;
	newMeeting->next = NULL;

	// Enqueue in FIFO order
	if (q->back == NULL) {
		q->front = q->back = newMeeting;
	}
	else {
		q->back->next = newMeeting;
		q->back = newMeeting;
	}
	q->count++;

	printf("Meeting booked successfully on %s for Student ID %d at %d.\n", date, studentID, time);
}

Operation* cancelMeeting(HashTable* ht, char* date, int studentID) {
	if (ht == NULL || date == NULL) {
		printf("Error: Invalid parameters for cancellation\n");
		return NULL;
	}
	
	int index = findHashIndex(ht, date);
	if (index == -1) {
		printf("Invalid date! Please enter a valid date.\n");
		return NULL;
	}
	
	Queue* q = ht[index].meetingQueue;
	if (q->front == NULL) {
		printf("No meetings found on %s.\n", date);
		return NULL;
	}

	Meeting* temp = q->front;
	Meeting* prev = NULL;
	while (temp != NULL && temp->studentID != studentID) {
		prev = temp;
		temp = temp->next;
	}
	
	if (temp == NULL) {
		printf("No meeting found for Student ID %d on %s.\n", studentID, date);
		return NULL;
	}
	
	// Allocate an Operation record to save details for undo (cancellation operation)
	Operation* op = (Operation*)malloc(sizeof(Operation));
	if (op == NULL) {
		printf("Failed to allocate memory for operation record.\n");
		return NULL;
	}
	
	op->type = OP_CANCEL;
	strcpy_s(op->date, MAX_DATE_LENGTH, date);
	op->studentID = temp->studentID;
	strcpy_s(op->name, MAX_NAME_LENGTH, temp->name);
	strcpy_s(op->title, MAX_TITLE_LENGTH, temp->title);
	op->timeSlot = temp->time;
	
	// Remove node from the queue
	if (prev == NULL) {  // Removing the first node
		q->front = temp->next;
	}
	else {
		prev->next = temp->next;
	}
	
	if (q->back == temp) {
		q->back = prev;
	}
	
	free(temp);
	
	q->count--;
	printf("Meeting canceled for Student ID %d on %s.\n", studentID, date);
	return op;
}

void searchMeeting(HashTable* ht, char* date, int studentID) {
	if (ht == NULL || date == NULL) {
		printf("Error: Invalid parameters for search\n");
		return;
	}

	int index = findHashIndex(ht, date);
	if (index == -1) {
		printf("Invalid date! Please enter a valid date.\n");
		return;
	}

	Queue* q = ht[index].meetingQueue;
	Meeting* temp = q->front;
	while (temp != NULL) {
		if (temp->studentID == studentID) {
			printf("Meeting Found!\nDate: %s\nStudent: %s\nTitle: %s\nTime: %d\n",
				date, temp->name, temp->title, temp->time);
			return;
		}
		temp = temp->next;
	}

	printf("No meeting found for Student ID %d on %s.\n", studentID, date);
}

void viewUpcomingMeetings(HashTable* ht) {
	if (ht == NULL) {
		printf("Error: Invalid hash table\n");
		return;
	}

	printf("\nUpcoming Meetings:\n");
	for (int i = 0; i < TOTAL_DAYS; i++) {
		Queue* q = ht[i].meetingQueue;
		Meeting* temp = q->front;

		printf("Date: %s - %s\n", ht[i].date, (q->count >= MAX_SLOTS) ? " Full" : " Available");
		if (temp == NULL) {
			printf("  - No meetings scheduled\n");
		}
		else {
			while (temp != NULL) {
				printf("  - Student: %s (ID: %d) | Title: %s | Time: %d\n",
					temp->name, temp->studentID, temp->title, temp->time);
				temp = temp->next;
			}
		}
		printf("\n");
	}
}

// Free all dynamic memory
void freeMeetingTable(HashTable* meetingTable) {
	if (meetingTable == NULL) {
		return;
	}

	for (int i = 0; i < TOTAL_DAYS; i++) {
		Queue* q = meetingTable[i].meetingQueue;
		if (q != NULL) {
			// Free all meeting nodes in Queue
			Meeting* current = q->front;
			while (current != NULL) {
				Meeting* next = current->next;
				free(current);
				current = next;
			}
			// Free Queue
			free(q);
			meetingTable[i].meetingQueue = NULL;
		}
	}
}

// Remove a meeting matching details from an operation record (used for undo of booking)
int removeMeeting(HashTable* ht, Operation* op) {
	int index = findHashIndex(ht, op->date);
	
	if (index == -1) {
		return 0;
	}

	Queue* q = ht[index].meetingQueue;
	Meeting* temp = q->front;
	Meeting* prev = NULL;
	while (temp != NULL) {
		if (temp->studentID == op->studentID && temp->time == op->timeSlot) {
			if (prev == NULL) {
				q->front = temp->next;
			}
			else {
				prev->next = temp->next;
			}
			if (q->back == temp) {
				q->back = prev;
			}
			free(temp);
			q->count--;
			return 1;
		}
		prev = temp;
		temp = temp->next;
	}
	return 0;
}

// Reinsert a meeting from an operation record (used for undo of cancellation)
int reinsertMeeting(HashTable* ht, Operation* op) {
	int index = findHashIndex(ht, op->date);
	
	if (index == -1) {
		return 0;
	}

	Queue* q = ht[index].meetingQueue;
	
	if (q->count >= MAX_SLOTS) {
		printf("Cannot undo cancellation: All slots are filled for %s.\n", op->date);
		return 0;
	}

	// Create a new meeting node from op details
	Meeting* newMeeting = (Meeting*)malloc(sizeof(Meeting));
	
	if (newMeeting == NULL) {
		return 0;
	}

	newMeeting->studentID = op->studentID;
	strcpy_s(newMeeting->name, MAX_NAME_LENGTH, op->name);
	strcpy_s(newMeeting->title, MAX_TITLE_LENGTH, op->title);
	newMeeting->time = op->timeSlot;
	newMeeting->next = NULL;
	// Enqueue the meeting node
	if (q->back == NULL) {
		q->front = q->back = newMeeting;
	}
	else {
		q->back->next = newMeeting;
		q->back = newMeeting;
	}

	q->count++;
	return 1;
}
void initStack(OperationStack* stack) {
	stack->top = NULL;
}

bool isStackEmpty(OperationStack* stack) {
	return (stack->top == NULL);
}

void pushStack(OperationStack* stack, Operation op) {
	StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
	
	if (newNode == NULL) {
		printf("Stack: Unable to allocate memory for new operation.\n");
		return;
	}

	newNode->op = op;
	newNode->next = stack->top;
	stack->top = newNode;
}

int popStack(OperationStack* stack, Operation* op) {
	if (isStackEmpty(stack))
	{
		return 0;
	}
	
	StackNode* temp = stack->top;
	*op = temp->op;
	stack->top = temp->next;
	free(temp);
	
	return 1;
}

void freeStack(OperationStack* stack) {
	StackNode* current = stack->top;
	
	while (current != NULL) {
		StackNode* temp = current;
		current = current->next;
		free(temp);
	}
	
	stack->top = NULL;
}

void insertKVP(KVP** head, char* key, char* value) {
	KVP* newNode = (KVP*)malloc(sizeof(KVP));
	
	if (newNode == NULL) {
		printf("Failed to allocate memory for KVP node.\n");
		return;
	}
	
	strcpy_s(newNode->key, MAX_KEY_LENGTH_KVP, key);
	strcpy_s(newNode->value, MAX_VALUE_LENGTH_KVP, value);
	newNode->next = *head;
	*head = newNode;
}

char* searchKVP(KVP* head, char* key) {
	while (head != NULL) {
		if (strcmp(head->key, key) == 0)
			return head->value;
		head = head->next;
	}
	return NULL;
}

void freeKVP(KVP* head) {
	while (head != NULL) {
		KVP* temp = head;
		head = head->next;
		free(temp);
	}
}
