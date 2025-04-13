#include "SMS.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The global array of dates
char* dates[TOTAL_DAYS] = { "May 1", "May 2", "May 3", "May 4", "May 5", "May 6", "May 7" };
historyNode* historyHead = NULL;


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
	Queue* queue = ht[index].meetingQueue;

	// Check if time slot is already taken
	Meeting* current = queue->front;
	while (current != NULL) {
		if (current->time == time) {
			printf("The %d time slot is already booked on %s. Please choose a different time.\n", time, date);
			return;
		}
		current = current->next;
	}

	// Check if maximum slots are reached
	if (queue->count >= MAX_SLOTS) {
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
	if (queue->back == NULL) {
		queue->front = queue->back = newMeeting;
	}
	else {
		queue->back->next = newMeeting;
		queue->back = newMeeting;
	}
	queue->count++;

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

	Queue* queue = ht[index].meetingQueue;
	if (queue->front == NULL) {
		printf("\nNo meetings found on %s.\n", date);
		return NULL;
	}

	Meeting* temp = queue->front;
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
		queue->front = temp->next;
	}
	else {
		prev->next = temp->next;
	}

	if (queue->back == temp) {
		queue->back = prev;
	}

	free(temp);

	queue->count--;
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

	Meeting* current = ht[index].meetingQueue->front;
	while (current != NULL) {
		if (current->studentID == studentID) {
			printf("Meeting Found!\nDate: %s\nStudent: %s\nTitle: %s\nTime: %d\n",
				date, current->name, current->title, current->time);
			return;
		}
		current = current->next;
	}

	printf("\nNo meeting found for Student ID %d on %s.\n", studentID, date);
}

void viewUpcomingMeetings(HashTable* ht) {
	if (ht == NULL) {
		printf("Error: Invalid hash table\n");
		return;
	}

	printf("\nUpcoming Meetings:\n");
	for (int i = 0; i < TOTAL_DAYS; i++) {
		Queue* queue = ht[i].meetingQueue;
		Meeting* current = queue->front;

		printf("Date: %s - %s\n", ht[i].date, (queue->count >= MAX_SLOTS) ? " Full" : " Available");
		if (current == NULL) {
			printf("  - No meetings scheduled\n");
		}
		else {
			while (current != NULL) {
				printf("  - Student: %s (ID: %d) | Title: %s | Time: %d\n",
					current->name, current->studentID, current->title, current->time);
				current = current->next;
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
		Queue* queue = meetingTable[i].meetingQueue;
		if (queue != NULL) {
			// Free all meeting nodes in Queue
			Meeting* current = queue->front;
			while (current != NULL) {
				Meeting* next = current->next;
				free(current);
				current = next;
			}
			// Free Queue
			free(queue);
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

	Queue* queue = ht[index].meetingQueue;
	Meeting* temp = queue->front;
	Meeting* prev = NULL;
	while (temp != NULL) {
		if (temp->studentID == op->studentID && temp->time == op->timeSlot) {
			if (prev == NULL) {
				queue->front = temp->next;
			}
			else {
				prev->next = temp->next;
			}
			if (queue->back == temp) {
				queue->back = prev;
			}
			free(temp);
			queue->count--;
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

	Queue* queue = ht[index].meetingQueue;

	if (queue->count >= MAX_SLOTS) {
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
	if (queue->back == NULL) {
		queue->front = queue->back = newMeeting;
	}
	else {
		queue->back->next = newMeeting;
		queue->back = newMeeting;
	}

	queue->count++;
	return 1;
}
//Function: addToHistory
//Description: adds a processed meeting to the end of the history linked list
//Parameters: Meeting meet
//Returns none
void addToHistory(Meeting meet) {
	historyNode* newNode = (historyNode*)malloc(sizeof(historyNode));
	newNode->data = meet;
	newNode->next = NULL;

	//first entry
	if (historyHead == NULL) {
		historyHead = newNode;
	}
	else {
		historyNode* current = historyHead;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = newNode;
	}
}
//Function: viewMeetingHistory
//Description: function will display all processed meetings
//Parameters: None
//Returns None
void viewMeetingHistory() {
	if (historyHead == NULL) {
		printf("No Meetings have been Processed\n");
		return;
	}
	printf("Processed Meeting History\n");
	historyNode* current = historyHead;
	int count = 1;

	while (current != NULL) {
		Meeting meet = current->data;

		printf("[%d] Meeting: \n", count++);
		printf("Student     : %s\n", meet.name);
		printf("ID          : %d\n", meet.studentID);
		printf("Title       : %s\n", meet.title);
		printf("Time        : %d\n", meet.time);
		current = current->next;
	}
	printf("\n");
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





// Initialize the processed meeting stack
void initProcessedStack(ProcessedMeetingStack* stack) {
	stack->Top = NULL;
}

// Push a processed meeting onto the stack
void pushProcessedMeeting(ProcessedMeetingStack* stack, Meeting meeting, char* date) {
	ProcessedMeetingNode* newNode = (ProcessedMeetingNode*)malloc(sizeof(ProcessedMeetingNode));

	if (newNode == NULL) {
		printf("Failed to allocate memory for processed meeting.\n");
		return;
	}

	// Copy meeting data
	newNode->data = meeting;
	strcpy_s(newNode->date, MAX_DATE_LENGTH, date);

	// Add to top of stack
	newNode->NextNode = stack->Top;
	stack->Top = newNode;
}

// Pop a processed meeting from the stack
int popProcessedMeeting(ProcessedMeetingStack* stack, Meeting* meeting, char* date) {
	if (stack->Top == NULL) {
		return 0;  // Stack is empty
	}

	ProcessedMeetingNode* temp = stack->Top;
	*meeting = temp->data;
	strcpy_s(date, MAX_DATE_LENGTH, temp->date);

	stack->Top = temp->NextNode;
	free(temp);
	return 1;
}

// Process completed meetings based on current date
void processCompletedMeetings(HashTable* ht, ProcessedMeetingStack* stack, char* currentDate)
{
	if (ht == NULL || stack == NULL || currentDate == NULL) {
		printf("Error: Invalid parameters for processing meetings\n");
		return;
	}

	int processedCount = 0;

	// Iterate through all dates in the schedule
	for (int i = 0; i < TOTAL_DAYS; i++) 
	{
		// Skip if the date is today or in the future
		if (strcmp(ht[i].date, currentDate) >= 0)
		{
			continue;  // Skip dates that haven't passed
		}

		Queue* queue = ht[i].meetingQueue;
		Meeting* current = queue->front;
		Meeting* prev = NULL;

		// Process all meetings for this past date
		while (current != NULL) {
			// Save the next pointer before removing the current node
			Meeting* next = current->next;

			// Add to processed stack
			pushProcessedMeeting(stack, *current, ht[i].date);

			// Also add to history (connecting with existing functionality)
			addToHistory(*current);

			processedCount++;

			// Remove from queue
			if (prev == NULL) {
				queue->front = next;
			}
			else {
				prev->next = next;
			}

			if (queue->back == current) {
				queue->back = prev;
			}

			free(current);
			queue->count--;

			current = next;
		}
	}

	if (processedCount > 0) {
		printf("Processed %d meetings.\n", processedCount);
	}
	else {
		printf("No meetings to process.\n");
	}
}

// View processed meetings in stack (most recent first)
void viewProcessedMeetings(ProcessedMeetingStack* stack)
{
	if (stack == NULL || stack->Top == NULL)
	{
		printf("No processed meetings in stack.\n");
		return;
	}

	printf("\n===== Processed Meeting History (Latest First) =====\n");
	ProcessedMeetingNode* current = stack->Top;
	int count = 1;

	while (current != NULL)
	{
		Meeting meeting = current->data;

		printf("\n[%d] Meeting:\n", count++);
		printf("Date      : %s\n", current->date);
		printf("Student   : %s (ID: %d)\n", meeting.name, meeting.studentID);
		printf("Title     : %s\n", meeting.title);
		printf("Time      : %d\n", meeting.time);
		printf("------------------------\n");

		current = current->NextNode;
	}
}

// Free memory used by processed meeting stack
void freeProcessedStack(ProcessedMeetingStack* stack) 
{
	ProcessedMeetingNode* current = stack->Top;

	while (current != NULL)
	{
		ProcessedMeetingNode* temp = current;
		current = current->NextNode;
		free(temp);
	}

	stack->Top = NULL;
}
