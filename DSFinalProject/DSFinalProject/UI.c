#include "UI.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>  

static int logCounter = 1;

void displayMenu(HashTable* ht, OperationStack* operationStack, KVP** logList, ProcessedMeetingStack* processedStack) {
	if (ht == NULL) {
		printf("Error: Invalid hash table\n");
		return;
	}

	int menuNum = 0, studentID = 0, dateChoice = 0, timeChoice = 0;
	char name[MAX_NAME_LENGTH] = "", title[MAX_TITLE_LENGTH] = "";
	char logKey[LOG_KEY_LENGHT] = "", logMsg[MAX_LOG_LENGTH] = "";
	Operation op = { 0 };

	do {
		printf("\n===========================================\n");
		printf("          Student Meeting Scheduler        \n");
		printf("===========================================\n");
		printf("1. Book Meeting\n2. Cancel Meeting\n3. Search Meeting\n4. View Upcoming Meetings\n5. Undo Last Operation\n6. Display Operation Log\n7. View Meeting History\n8. Process Completed Meetings\n9. View Processed Meetings\n 10.Exit\n");

		menuNum = validMenuChoice(10);
		switch (menuNum) {
		case BOOK:
			dateChoice = getValidDateChoice();
			if (dateChoice == -1) {
				break;
			}

			Queue* queue = ht[dateChoice].meetingQueue;
			timeChoice = getValidTimeSlot(queue);
			if (timeChoice == -1) {
				break;
			}
			getValidUserInput(ht, &studentID, name, title);
			insertMeeting(ht, dates[dateChoice], studentID, name, title, timeChoice);
			// Create operation record for booking

			op.type = OP_BOOK;
			strcpy_s(op.date, MAX_DATE_LENGTH, dates[dateChoice]);
			op.studentID = studentID;
			strcpy_s(op.name, MAX_NAME_LENGTH, name);
			strcpy_s(op.title, MAX_TITLE_LENGTH, title);
			op.timeSlot = timeChoice;
			pushStack(operationStack, op);

			sprintf_s(logKey, sizeof(logKey), "LOG%d", logCounter++);
			sprintf_s(logMsg, sizeof(logMsg), "Booked meeting on %s for Student ID %d at %d", dates[dateChoice], studentID, timeChoice);
			insertKVP(logList, logKey, logMsg);
			break;
		case CANCEL:
			dateChoice = getValidDateChoice();
			if (dateChoice == -1) {
				break;
			}

			printf("Enter Student ID: ");
			studentID = getUserInput();

			Operation* op = cancelMeeting(ht, dates[dateChoice], studentID);
			if (op != NULL) {
				pushStack(operationStack, *op);
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
			undoLastOperation(ht, operationStack, logList);
			break;
		case VIEW_HISTORY:
			viewMeetingHistory();
			break;
		case DISPLAY_LOG:
			displayKVPLog(*logList);
			break;
		case PROCESS_MEETINGS: {
			char currentDate[MAX_DATE_LENGTH];
			printf("\nEnter current date (e.g., 'May 3'): ");
			if (fgets(currentDate, MAX_DATE_LENGTH, stdin) != NULL) {
				currentDate[strcspn(currentDate, "\n")] = '\0';

				// Validate date format
				int validDate = 0;
				for (int i = 0; i < TOTAL_DAYS; i++) {
					if (strcmp(dates[i], currentDate) == 0) {
						validDate = 1;
						break;
					}
				}

				if (validDate) {
					processCompletedMeetings(ht, processedStack, currentDate);

					sprintf_s(logKey, sizeof(logKey), "LOG%d", logCounter++);
					sprintf_s(logMsg, sizeof(logMsg), "Processed completed meetings for date %s", currentDate);
					insertKVP(logList, logKey, logMsg);
				}
				else {
					printf("Invalid date format. Please use format 'May X' where X is 1-7.\n");
				}
			}
			break;
		}
		case VIEW_PROCESSED:
			viewProcessedMeetings(processedStack);
			break;


		case EXIT_PROGRAM:
			printf("Exiting... Goodbye!\n");
			break;
		}
	} while (menuNum != EXIT_PROGRAM);
}

int validMenuChoice(int maxNumber)
{
	int choice;

	while (1) {
		printf("Enter your choice: ");//get valid number between 1 and max menu option
		if (scanf_s("%d", &choice) == 1 && choice >= 1 && choice <= maxNumber) {

			if (getchar() == '\n') {  // no extra charachters
				return choice;
			}
		}
		cleanBuffer();
		printf("Invalid Input. Please enter a number between 1 and %d.\n", maxNumber);
	}
}

// Safely read a number from stdin // NOT SURE WHY WE HAVE THIS 
int getUserInput() {
	char buffer[MAX_BUFFER_SIZE] = "";

	if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
		int value = 0;
		if (sscanf_s(buffer, "%d", &value) == 1)
			return value;
	}
	cleanBuffer();
	return -1;
}

void cleanBuffer() {
	int extraChar = 0;
	while ((extraChar = getchar()) != '\n' && extraChar != EOF);
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

int getValidTimeSlot(Queue* queue) {
	if (queue == NULL) {
		printf("Error: Invalid queue\n");
		return -1;
	}

	int choice = 0;
	int timeSlots[MAX_MEETING_SLOT_COUNT] = { 1000, 1100, 1200 };

	while (1) {  // Loop until valid time slot is chosen
		printf("\nAvailable Time Slots:\n");
		for (int i = 0; i < MAX_MEETING_SLOT_COUNT; i++) {
			printf("%d. %d AM - %s\n", i + 1, timeSlots[i] / 100,
				isTimeSlotBooked(queue, timeSlots[i]) ? "Booked" : "Available");
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
			if (isTimeSlotBooked(queue, selectedTime)) {
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

int isTimeSlotBooked(Queue* queue, int time) {
	if (queue == NULL) {
		return 0;
	}

	Meeting* temp = queue->front;
	while (temp != NULL) {
		if (temp->time == time) {
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}

int isDuplicateID(HashTable* ht, int studentID) {
	for (int i = 0; i < TOTAL_DAYS; i++) {
		Meeting* current = ht[i].meetingQueue->front;
		while (current != NULL) {
			if (current->studentID == studentID) {
				return 1; // Duplicate found
			}
			current = current->next;
		}
	}
	return 0; // No duplicate
}

int isDuplicateName(HashTable* ht, const char* name) {
	for (int i = 0; i < TOTAL_DAYS; i++) {
		Meeting* current = ht[i].meetingQueue->front;
		while (current != NULL) {
			if (strcmp(current->name, name) == 0) {
				return 1; // Duplicate found
			}
			current = current->next;
		}
	}
	return 0; // No duplicate
}

void getValidUserInput(HashTable* ht, int* studentID, char* name, char* title) {
	if (ht == NULL || studentID == NULL || name == NULL || title == NULL) {
		printf("Error: Invalid input parameters\n");
		return;
	}

	// Student ID validation
	char idInput[50];
	do {
		printf("Enter Student ID: ");
		if (fgets(idInput, sizeof(idInput), stdin) != NULL) {
			idInput[strcspn(idInput, "\n")] = '\0';

			// Check blank input
			if (strlen(idInput) == 0 || strspn(idInput, " \t") == strlen(idInput)) {
				printf("Student ID cannot be blank!\n");
				continue;
			}

			// Check all digits
			int allDigits = 1;
			for (int i = 0; idInput[i] != '\0'; i++) {
				if (!isdigit(idInput[i])) {
					allDigits = 0;
					break;
				}
			}

			if (!allDigits) {
				printf("Student ID must contain only numbers!\n");
				continue;
			}

			// Check length
			if (strlen(idInput) != 6) {
				printf("Student ID must be exactly 6 digits!\n");
				continue;
			}

			*studentID = atoi(idInput);

			// Check duplicate ID with inline confirmation
			if (isDuplicateID(ht, *studentID)) {
				char response;
				do {
					printf("Warning: This Student ID already has a meeting!\n");
					printf("Continue anyway? (y/n): ");
					scanf_s(" %c", &response);
					cleanBuffer();
					response = tolower(response);

					if (response == 'n') {
						break; // Will restart ID entry
					}
					else if (response == 'y') {
						goto ID_VALIDATED; // Skip to end of ID validation
					}
					printf("Invalid input! Please enter 'y' or 'n' only.\n");
				} while (1);
				continue;
			}
		ID_VALIDATED:
			break;
		}
	} while (1);

	// Name validation
	do {
		printf("Enter Name: ");
		if (fgets(name, MAX_NAME_LENGTH, stdin) != NULL) {
			name[strcspn(name, "\n")] = '\0';

			// Check blank input
			if (strlen(name) == 0 || strspn(name, " \t") == strlen(name)) {
				printf("Name cannot be blank!\n");
				continue;
			}

			// Check letters and spaces only
			int valid = 1;
			for (size_t i = 0; i < strlen(name); i++) {
				if (!isalpha(name[i]) && !isspace(name[i])) {
					valid = 0;
					break;
				}
			}

			if (!valid) {
				printf("Name can only contain letters and spaces!\n");
				continue;
			}

			// Check duplicate name with inline confirmation
			if (isDuplicateName(ht, name)) {
				char response;
				do {
					printf("Warning: This name already has a meeting!\n");
					printf("Continue anyway? (y/n): ");
					scanf_s(" %c", &response);
					cleanBuffer();
					response = tolower(response);

					if (response == 'n') {
						break; // Will restart name entry
					}
					else if (response == 'y') {
						goto NAME_VALIDATED; // Skip to end of name validation
					}
					printf("Invalid input! Please enter 'y' or 'n' only.\n");
				} while (1);
				continue;
			}
		NAME_VALIDATED:
			break;
		}
	} while (1);

	// Title validation (unchanged)
	do {
		printf("Enter Meeting Title: ");
		if (fgets(title, MAX_TITLE_LENGTH, stdin) != NULL) {
			title[strcspn(title, "\n")] = '\0';
			if (strlen(title) == 0 || strspn(title, " \t") == strlen(title)) {
				printf("Title cannot be blank!\n");
				continue;
			}
			if (strlen(title) >= MAX_TITLE_LENGTH - 1) {
				cleanBuffer();
				printf("Title too long! Max %d characters allowed.\n", MAX_TITLE_LENGTH - 1);
				continue;
			}
			break;
		}
	} while (1);
}

void undoLastOperation(HashTable* ht, OperationStack* opStack, KVP** logList) {
	Operation op;
	char logKey[LOG_KEY_LENGHT] = "", logMsg[MAX_LOG_LENGTH] = "";

	if (!popStack(opStack, &op)) {
		printf("\nNo operations to undo.\n");
		return;
	}

	if (op.type == OP_BOOK) {
		if (removeMeeting(ht, &op)) {
			printf("\nUndo booking: Meeting for Student ID %d on %s at %d has been removed.\n", op.studentID, op.date, op.timeSlot);
			sprintf_s(logKey, sizeof(logKey), "LOG%d", logCounter++);
			sprintf_s(logMsg, sizeof(logMsg), "Undid booking for Student ID %d on %s at %d", op.studentID, op.date, op.timeSlot);
			insertKVP(logList, logKey, logMsg);
		}
		else {
			printf("\nUndo booking failed: Meeting not found.\n");
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