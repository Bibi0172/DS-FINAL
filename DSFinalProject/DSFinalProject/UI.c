#include "UI.h"
#include <stdio.h>
#include <stdlib.h>

static int logCounter = 1;

void displayMenu(HashTable* ht, OperationStack* opStack, KVP** logList) {
	if (ht == NULL) {
		printf("Error: Invalid hash table\n");
		return;
	}

	int choice = 0, studentID = 0, dateChoice = 0, timeChoice = 0;
	char name[MAX_NAME_LENGTH] = "", title[MAX_TITLE_LENGTH] = "";
	Operation op = { 0 };
	char logKey[10], logMsg[150];

	do {
		printf("\n===========================================\n");
		printf("          Student Meeting Scheduler        \n");
		printf("===========================================\n");
		printf("1. Book Meeting\n2. Cancel Meeting\n3. Search Meeting\n4. View Upcoming Meetings\n5. Undo Last Operation\n6. Display Operation Log\n7. Exit\n");

		choice = validMenuChoice(7);
		switch (choice) {
		case BOOK:
			dateChoice = getValidDateChoice();
			if (dateChoice == -1) break;
			{
				Queue* q = ht[dateChoice].meetingQueue;
				timeChoice = getValidTimeSlot(q);
				if (timeChoice == -1) break;
				getValidUserInput(&studentID, name, title);
				insertMeeting(ht, dates[dateChoice], studentID, name, title, timeChoice);
				// Create operation record for booking

				op.type = OP_BOOK;
				strcpy_s(op.date, MAX_DATE_LENGTH, dates[dateChoice]);
				op.studentID = studentID;
				strcpy_s(op.name, MAX_NAME_LENGTH, name);
				strcpy_s(op.title, MAX_TITLE_LENGTH, title);
				op.timeSlot = timeChoice;
				pushStack(opStack, op);

				sprintf_s(logKey, sizeof(logKey), "LOG%d", logCounter++);
				sprintf_s(logMsg, sizeof(logMsg), "Booked meeting on %s for Student ID %d at %d", dates[dateChoice], studentID, timeChoice);
				insertKVP(logList, logKey, logMsg);
			}
			break;
		case CANCEL:
			dateChoice = getValidDateChoice();
			if (dateChoice == -1)
				break;
			printf("Enter Student ID: ");
			studentID = getUserInput();
			
			Operation* op = cancelMeeting(ht, dates[dateChoice], studentID);
			if (op != NULL) {
				pushStack(opStack, *op);
				sprintf_s(logKey, sizeof(logKey), "LOG%d", logCounter++);
				sprintf_s(logMsg, sizeof(logMsg), "Canceled meeting on %s for Student ID %d", dates[dateChoice], studentID);
				insertKVP(logList, logKey, logMsg);
				free(op);
			}
			break;
		case SEARCH:
			dateChoice = getValidDateChoice();
			if (dateChoice == -1) {
				break;
			}
			printf("Enter Student ID: ");
			studentID = getUserInput();
			searchMeeting(ht, dates[dateChoice], studentID);
			break;
		case VIEW:
			viewUpcomingMeetings(ht);
			break;
		case UNDO:
			undoLastOperation(ht, opStack, logList);
			break;
		case DISPLAY_LOG:
			displayKVPLog(*logList);
			break;
		case EXIT_PROGRAM:
			printf("Exiting... Goodbye!\n");
			break;
		}
	} while (choice != EXIT_PROGRAM);
}

int validMenuChoice(int maxNumber) {
	int choice;
	while (1) {
		printf("Enter your choice: ");
		choice = getUserInput();
		if ((choice >= 1 && choice <= maxNumber) || choice == -1) {
			return choice;
		}

		printf("Invalid Input. Please enter a number between 1 and %d OR -1 to return to menu.\n", maxNumber);
	}
}

void cleanBuffer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

int getValidDateChoice() {
	int choice = 0;
	while (1) {  // Loop until valid input is provided
		printf("\nAvailable Dates:\n");
		for (int i = 0; i < TOTAL_DAYS; i++) {
			printf("%d. %s\n", i + 1, dates[i]);
		}
		printf("Enter -1 to return to the main menu.\n");
		choice = validMenuChoice(TOTAL_DAYS);
		if (choice == -1) {
			return -1;
		}

		if (choice >= 1 && choice <= TOTAL_DAYS) {
			return choice - 1;
		}
		else {
			printf("Invalid date choice. Please select a valid date.\n");
		}
	}
}

int getValidTimeSlot(Queue* q) {
	if (q == NULL) {
		printf("Error: Invalid queue\n");
		return -1;
	}

	int choice;
	int timeSlots[MAX_MEETING_SLOT_COUNT] = { 1000, 1100, 1200 };

	while (1) {  // Loop until valid time slot is chosen
		printf("\nAvailable Time Slots:\n");
		for (int i = 0; i < MAX_MEETING_SLOT_COUNT; i++) {
			printf("%d. %d AM - %s\n", i + 1, timeSlots[i] / 100,
				isTimeSlotBooked(q, timeSlots[i]) ? "Booked" : "Available");
		}
		printf("Enter -1 to return to the main menu.\n");

		printf("Select a time slot (1 - %d): ", MAX_MEETING_SLOT_COUNT);
		choice = validMenuChoice(MAX_MEETING_SLOT_COUNT);

		// Check for return to main menu
		if (choice == -1) {
			return -1;  // Return to main menu
		}

		if (choice >= 1 && choice <= MAX_MEETING_SLOT_COUNT) {
			int selectedTime = timeSlots[choice - 1];

			// Check if the selected time slot is booked
			if (isTimeSlotBooked(q, selectedTime)) {
				printf("That time slot is already booked. Please try another.\n");
			}
			else {
				return selectedTime;  // Return the valid time slot
			}
		}
		else {
			printf("Invalid time slot! Please select a valid time slot.\n");
		}
	}
}

int isTimeSlotBooked(Queue* q, int time) {
	if (q == NULL) {
		return 0;
	}

	Meeting* temp = q->front;
	while (temp != NULL) {
		if (temp->time == time) {
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}

void getValidUserInput(int* studentID, char* name, char* title) {
	if (studentID == NULL || name == NULL || title == NULL) {
		printf("Error: Invalid input parameters\n");
		return;
	}

	printf("Enter Student ID: ");
	*studentID = getUserInput();

	printf("Enter Name: ");
	if (fgets(name, MAX_NAME_LENGTH, stdin) != NULL) {
		name[strcspn(name, "\n")] = '\0';
	}
	printf("Enter Meeting Title: ");
	if (fgets(title, MAX_TITLE_LENGTH, stdin) != NULL) {
		title[strcspn(title, "\n")] = '\0';
	}
}

void undoLastOperation(HashTable* ht, OperationStack* opStack, KVP** logList) {
	Operation op;
	char logKey[10], logMsg[150];

	if (!popStack(opStack, &op)) {
		printf("No operations to undo.\n");
		return;
	}

	if (op.type == OP_BOOK) {
		if (removeMeeting(ht, &op)) {
			printf("Undo booking: Meeting for Student ID %d on %s at %d has been removed.\n", op.studentID, op.date, op.timeSlot);
			sprintf_s(logKey, sizeof(logKey), "LOG%d", logCounter++);
			sprintf_s(logMsg, sizeof(logMsg), "Undid booking for Student ID %d on %s at %d", op.studentID, op.date, op.timeSlot);
			insertKVP(logList, logKey, logMsg);
		}
		else {
			printf("Undo booking failed: Meeting not found.\n");
		}
	}
	else if (op.type == OP_CANCEL) {
		if (reinsertMeeting(ht, &op)) {
			printf("Undo cancellation: Meeting for Student ID %d on %s at %d has been restored.\n", op.studentID, op.date, op.timeSlot);
			sprintf_s(logKey, sizeof(logKey), "LOG%d", logCounter++);
			sprintf_s(logMsg, sizeof(logMsg), "Undid cancellation for Student ID %d on %s at %d", op.studentID, op.date, op.timeSlot);
			insertKVP(logList, logKey, logMsg);
		}
		else {
			printf("Undo cancellation failed: Could not restore the meeting.\n");
		}
	}
}

// Safely read a number from stdin
int getUserInput() {
	char buffer[50];

	if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
		int value;
		if (sscanf_s(buffer, "%d", &value) == 1)
			return value;
	}
	cleanBuffer();
	return -1;
}

void displayKVPLog(KVP* operationLog) {
	if (operationLog == NULL) {
		printf("Operation log is empty.\n");
		return;
	}
	printf("Operation Log:\n");
	while (operationLog != NULL) {
		printf("Key: %s | Value: %s\n", operationLog->key, operationLog->value);
		operationLog = operationLog->next;
	}
}