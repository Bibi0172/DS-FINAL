#include "SMS.h"
#include "UI.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The global array of dates
char* dates[TOTAL_DAYS] = { "May 1", "May 2", "May 3", "May 4", "May 5", "May 6", "May 7" };
historyNode* historyHead = NULL;

//Function: initializeHashTable
//Description: Initializes the hash table by assigning date values and allocating memory for meeting queues.
//Parameters: HashTable* ht - the hash table to initialize
//Returns: None
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

//Function: hashDate
//Description: Generates a hash value for a given date string using polynomial hashing.
//Parameters: char* date - the date string to hash
//Returns: int - the hashed index for the date
int hashDate(char* date) {
	int hash = 0;

	for (int i = 0; date[i] != '\0'; i++) {
		hash = 31 * hash + date[i];
	}

	return hash % TOTAL_DAYS;
}

//Function: findHashIndex
//Description: Finds the correct index in the hash table for a given date using linear probing.
//Parameters: HashTable* ht - the hash table,
//            char* date - the date string to locate
//Returns: int - the index of the matching date or -1 if not found
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

//Function: insertMeeting
//Description: Inserts a new meeting into the queue for the specified date while maintaining FIFO order.
//Parameters: HashTable* ht - the hash table to insert into,
//            char* date - the meeting date,
//            int studentID - the student’s ID,
//            char* name - the student’s name,
//            char* title - the meeting title
//Returns: None
void insertMeeting(HashTable* ht, char* date, int studentID, char* name, char* title) {
	int index = findHashIndex(ht, date);

	if (index == -1) {
		printf("Invalid date! Please choose a valid date.\n");
		return;
	}
	Queue* queue = ht[index].meetingQueue;

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

	printf("Meeting booked successfully on %s for Student ID %d .\n", date, studentID);
}

//Function: cancelMeeting
//Description: Cancels a meeting for a student ID on a specified date and returns an operation record for undo.
//Parameters: HashTable* ht - the hash table,
//            char* date - the meeting date,
//            int studentID - the ID to cancel
//Returns: Operation* - dynamically allocated operation record or NULL if not found
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

//Function: searchMeeting
//Description: Searches for a meeting by student ID on a specific date and displays details if found.
//Parameters: HashTable* ht - the hash table to search,
//            char* date - the meeting date,
//            int studentID - the ID to search for
//Returns: None
void searchMeeting(HashTable* ht, char* date, int studentID) {
	if (ht == NULL) {
		printf("Error: Invalid parameters for search\n");
		return;
	}

	bool found = false;
	// Search through all dates
	for (int i = 0; i < TOTAL_DAYS; i++) {
		// If date is specified, only search on that date
		if (date != NULL && strcmp(ht[i].date, date) != 0) {
			continue;
		}

		Meeting* current = ht[i].meetingQueue->front;
		while (current != NULL) {
			if (current->studentID == studentID) {
				if (!found) {
					printf("\n+--------------------------------------------------+\n");
					printf("|                 Meeting Found!                   |\n");
					printf("+--------------------------------------------------+\n");
					found = true;
				}
				printf("| Date       : %-35s |\n", ht[i].date);
				printf("| Student ID : %-35d |\n", current->studentID);
				printf("| Name       : %-35s |\n", current->name);
				printf("| Title      : %-35s |\n", current->title);
				printf("+--------------------------------------------------+\n");
			}
			current = current->next;
		}
	}

	if (!found) {
		printf("\n+--------------------------------------------------+\n");
		printf("| No meeting found for Student ID %d .|\n", studentID);
		printf("+--------------------------------------------------+\n");
	}
}

//Function: viewUpcomingMeetings
//Description: Displays all upcoming meetings grouped by date, indicating availability.
//Parameters: HashTable* ht - the hash table to display
//Returns: None
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
				printf("  - Student: %s (ID: %d) | Title: %s \n",
					current->name, current->studentID, current->title);
				current = current->next;
			}
		}
		printf("\n");
	}
}

//Function: freeMeetingTable
//Description: Frees all memory allocated for the hash table including queues and meeting nodes.
//Parameters: HashTable* meetingTable - the hash table to deallocate
//Returns: None
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

//Function: removeMeeting
//Description: Removes a meeting from the queue based on an operation record (used for undoing bookings).
//Parameters: HashTable* ht - the hash table,
//            Operation* op - the operation containing meeting details
//Returns: int - 1 if successfully removed, 0 otherwise
int removeMeeting(HashTable* ht, Operation* op) {
	int index = findHashIndex(ht, op->date);

	if (index == -1) {
		return 0;
	}

	Queue* queue = ht[index].meetingQueue;
	Meeting* temp = queue->front;
	Meeting* prev = NULL;
	while (temp != NULL) {
		if (temp->studentID == op->studentID) {
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

//Function: reinsertMeeting
//Description: Reinserts a meeting into the queue based on an operation record (used for undoing cancellations).
//Parameters: HashTable* ht - the hash table,
//            Operation* op - the operation containing meeting details
//Returns: int - 1 if successfully inserted, 0 otherwise
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
	//newMeeting->time = op->timeSlot;
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
		current = current->next;
	}
	printf("\n");
}

//Function: initStack  
//Description: Initializes the operation stack by setting its top pointer to NULL.
//Parameters: OperationStack* stack - pointer to the stack to initialize
//Returns: None
void initStack(OperationStack* stack) {
	stack->top = NULL;
}

//Function: isStackEmpty  
//Description: Checks whether the operation stack is empty.
//Parameters: OperationStack* stack - pointer to the stack
//Returns: bool - true if stack is empty, false otherwise
bool isStackEmpty(OperationStack* stack) {
	return (stack->top == NULL);
}

//Function: pushStack  
//Description: Pushes an operation record onto the stack.
//Parameters: OperationStack* stack - pointer to the stack,
//            Operation op - the operation to be pushed
//Returns: None
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

//Function: popStack  
//Description: Pops the top operation from the stack and copies it into the provided Operation pointer.
//Parameters: OperationStack* stack - pointer to the stack,
//            Operation* op - pointer to store the popped operation
//Returns: int - 1 if successful, 0 if stack is empty
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

//Function: freeStack  
//Description: Frees all memory allocated for the operation stack nodes.
//Parameters: OperationStack* stack - pointer to the stack
//Returns: None
void freeStack(OperationStack* stack) {
	StackNode* current = stack->top;

	while (current != NULL) {
		StackNode* temp = current;
		current = current->next;
		free(temp);
	}

	stack->top = NULL;
}

//Function: insertKVP  
//Description: Inserts a new key-value pair into the front of the operation log linked list.
//Parameters: KVP** head - pointer to the head of the linked list,
//            char* key - the key string,
//            char* value - the message/value string
//Returns: None
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

//Function: searchKVP  
//Description: Searches the key-value log list for a specific key and returns the corresponding value.
//Parameters: KVP* head - pointer to the head of the linked list,
//            char* key - the key string to search
//Returns: char* - pointer to the matching value if found, otherwise NULL
char* searchKVP(KVP* head, char* key) {
	while (head != NULL) {
		if (strcmp(head->key, key) == 0)
			return head->value;
		head = head->next;
	}
	return NULL;
}

//Function: freeKVP  
//Description: Frees all memory used by the key-value log linked list.
//Parameters: KVP* head - pointer to the head of the linked list
//Returns: None
void freeKVP(KVP* head) {
	while (head != NULL) {
		KVP* temp = head;
		head = head->next;
		free(temp);
	}
}

//Function: undoLastOperation
//Description: Undoes the last operation (either booking or cancellation) using the operation stack.
//Parameters: HashTable* ht - the hash table to modify,
//            OperationStack* opStack - the stack containing past operations,
//            KVP** logList - pointer to the linked list log to record undo actions
//Returns: None
void undoLastOperation(HashTable* ht, OperationStack* opStack, KVP** logList) {
	Operation op;
	char logKey[LOG_KEY_LENGHT] = "", logMsg[MAX_LOG_LENGTH] = "";

	if (!popStack(opStack, &op)) {
		printf("\nNo operations to undo.\n");
		return;
	}

	if (op.type == OP_BOOK) {
		if (removeMeeting(ht, &op)) {
			printf("\nUndo booking: Meeting for Student ID %d on %s has been removed.\n", op.studentID, op.date);
			sprintf_s(logKey, sizeof(logKey), "LOG%d", logCounter++);
			sprintf_s(logMsg, sizeof(logMsg), "Undid booking for Student ID %d on %s.", op.studentID, op.date);
			insertKVP(logList, logKey, logMsg);
		}
		else {
			printf("\nUndo booking failed: Meeting not found.\n");
		}
	}
	else if (op.type == OP_CANCEL) {
		if (reinsertMeeting(ht, &op)) {
			printf("Undo cancellation: Meeting for Student ID %d on %s has been restored.\n", op.studentID, op.date);
			sprintf_s(logKey, sizeof(logKey), "LOG%d", logCounter++);
			sprintf_s(logMsg, sizeof(logMsg), "Undid cancellation for Student ID %d on %s .", op.studentID, op.date);
			insertKVP(logList, logKey, logMsg);
		}
		else {
			printf("Undo cancellation failed: Could not restore the meeting.\n");
		}
	}
}

//Function: displayKVPLog
//Description: Displays all entries in the operation log as key-value pairs.
//Parameters: KVP* operationLog - the head of the log list
//Returns: None
void displayKVPLog(KVP* operationLog) {
	if (operationLog == NULL) {
		printf("\nOperation log is empty.\n");
		return;
	}
	printf("Operation Log:\n");
	while (operationLog != NULL) {
		printf("Key: %s | Value: %s\n", operationLog->key, operationLog->value);
		operationLog = operationLog->next;
	}
}

//Function: processFirstMeeting  
//Description: Processes the first available meeting across all dates, removing it from the queue, logging it, and storing for undo.
//Parameters: HashTable* ht - the hash table,
//            OperationStack* opStack - stack to record the operation for undo,
//            KVP** logList - pointer to the head of the operation log linked list
//Returns: None
void processFirstMeeting(HashTable* ht, OperationStack* opStack, KVP** logList) {
	char logKey[LOG_KEY_LENGHT] = "";
	char logMsg[MAX_LOG_LENGTH] = "";

	// Find the first date that has meetings
	for (int i = 0; i < TOTAL_DAYS; i++) {
		Queue* queue = ht[i].meetingQueue;

		if (queue->front != NULL) {
			Meeting* meetingToProcess = queue->front;

			// Create an operation record for processing
			Operation op;
			op.type = OP_CANCEL; // Similar to cancellation since we're removing it
			strcpy_s(op.date, MAX_DATE_LENGTH, ht[i].date);
			op.studentID = meetingToProcess->studentID;
			strcpy_s(op.name, MAX_NAME_LENGTH, meetingToProcess->name);
			strcpy_s(op.title, MAX_TITLE_LENGTH, meetingToProcess->title);

			// Add to meeting history
			addToHistory(*meetingToProcess);

			// Remove from queue
			queue->front = meetingToProcess->next;
			if (queue->front == NULL) {
				queue->back = NULL;
			}
			queue->count--;

			// Push the operation to stack (for potential undo)
			pushStack(opStack, op);

			// Add to log
			sprintf_s(logKey, sizeof(logKey), "LOG%d", logCounter++);
			sprintf_s(logMsg, sizeof(logMsg), "Processed meeting on %s for %s (ID: %d)",
				ht[i].date, meetingToProcess->name, meetingToProcess->studentID);
			insertKVP(logList, logKey, logMsg);

			free(meetingToProcess);

			printf("\nSuccessfully processed meeting for %s (ID: %d) on %s\n",
				op.name, op.studentID, op.date);
			return;
		}
	}

	printf("\nNo meetings to process - all queues are empty.\n");
}