#include "UI.h"

static int logCounter = 1;

void displayMenu(HashTable* ht, OperationStack* operationStack, KVP** logList) {
	if (ht == NULL) {
		printf("Error: Invalid hash table\n");
		return;
	}

	int menuNum = 0, studentID = 0, dateChoice = 0;
	char name[MAX_NAME_LENGTH] = "", title[MAX_TITLE_LENGTH] = "";
	char logKey[LOG_KEY_LENGHT] = "", logMsg[MAX_LOG_LENGTH] = "";
	Operation op = { 0 };

	do {
		printf("\n===========================================\n");
		printf("          Student Meeting Scheduler        \n");
		printf("===========================================\n");
		printf("1. Book Meeting\n2. Cancel Meeting\n3. Search Meeting\n4. View Upcoming Meetings\n5. Undo Last Operation\n6. Display Operation Log\n7. View Meeting History\n 8.Exit\n");

		menuNum = validMenuChoice(7);
		switch (menuNum) {
		case BOOK:
			dateChoice = getValidDateChoice(ht);
			if (dateChoice == -1) {
				break;
			}

			Queue* queue = ht[dateChoice].meetingQueue;
			if (queue->count >= MAX_SLOTS) {
				printf("All slots are already booked for %s. Please choose another date.\n", dates[dateChoice]);
				break;
			}
			//timeChoice = getValidTimeSlot(queue);
			//if (timeChoice == -1) {
			//	break;
			//}
			getValidUserInput(ht, &studentID, name, title);
			if (studentID == -1) break;

			insertMeeting(ht, dates[dateChoice], studentID, name, title);
			// Create operation record for booking

			op.type = OP_BOOK;
			strcpy_s(op.date, MAX_DATE_LENGTH, dates[dateChoice]);
			op.studentID = studentID;
			strcpy_s(op.name, MAX_NAME_LENGTH, name);
			strcpy_s(op.title, MAX_TITLE_LENGTH, title);
			//op.timeSlot = timeChoice;
			pushStack(operationStack, op);

			sprintf_s(logKey, sizeof(logKey), "LOG%d", logCounter++);
			sprintf_s(logMsg, sizeof(logMsg), "Booked meeting on %s for Student ID %d .", dates[dateChoice], studentID);
			insertKVP(logList, logKey, logMsg);
			break;
		case CANCEL:
			dateChoice = getValidDateChoice(ht);
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
			dateChoice = getValidDateChoice(ht);
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

int getValidDateChoice(HashTable* ht) {
	int choice = 0;

	while (1) {
		printf("\nAvailable Dates:\n");
		for (int i = 0; i < TOTAL_DAYS; i++) {
			int booked = ht[i].meetingQueue->count;
			int available = MAX_SLOTS - booked;
			printf("%d. %s - %d/%d slots booked (%d available)\n", i + 1, dates[i], booked, MAX_SLOTS, available);
		}
		printf("Enter -1 to return to the main menu.\n");
		choice = validMenuChoice(TOTAL_DAYS);
		if (choice == -1) return -1;
		if (choice >= 1 && choice <= TOTAL_DAYS) return choice - 1;

		printf("Invalid date choice. Please try again.\n");
	}

}

//
//int getValidTimeSlot(Queue* queue) {
//	if (queue == NULL) {
//		printf("Error: Invalid queue\n");
//		return -1;
//	}
//
//	int choice = 0;
//	int timeSlots[MAX_MEETING_SLOT_COUNT] = { 1000, 1100, 1200 };
//
//	while (1) {  // Loop until valid time slot is chosen
//		printf("\nAvailable Time Slots:\n");
//		for (int i = 0; i < MAX_MEETING_SLOT_COUNT; i++) {
//			printf("%d. %d AM - %s\n", i + 1, timeSlots[i] / 100,
//				isTimeSlotBooked(queue, timeSlots[i]) ? "Booked" : "Available");
//		}
//		printf("Enter -1 to return to the main menu.\n");
//
//		printf("Select a time slot (1 - %d): ", MAX_MEETING_SLOT_COUNT);
//		choice = validMenuChoice(MAX_MEETING_SLOT_COUNT);
//
//		// Check for return to main menu
//		if (choice == -1) {
//			return -1;  // Return to main menu
//		}
//
//		if (choice >= 1 && choice <= MAX_MEETING_SLOT_COUNT) {
//			int selectedTime = timeSlots[choice - 1];
//
//			// Check if the selected time slot is booked
//			if (isTimeSlotBooked(queue, selectedTime)) {
//				printf("That time slot is already booked. Please try another.\n");
//			}
//			else {
//				return selectedTime;  // Return the valid time slot
//			}
//		}
//		else {
//			printf("Invalid time slot! Please select a valid time slot.\n");
//		}
//	}
//////}

//int isTimeSlotBooked(Queue* queue, int time) {
//	if (queue == NULL) {
//		return 0;
//	}
//
//	Meeting* temp = queue->front;
//	while (temp != NULL) {
//		if (temp->time == time) {
//			return 1;
//		}
//		temp = temp->next;
//	}
//	return 0;
//}

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
	char inputBuffer[50];
	do {
		printf("Enter Student ID : ");
		if (fgets(inputBuffer, sizeof(inputBuffer), stdin) != NULL) {
			inputBuffer[strcspn(inputBuffer, "\n")] = '\0';

			// Check for menu request
			if (tolower(inputBuffer[0]) == 'm' && strlen(inputBuffer) == 1) {
				*studentID = -1;
				return;
			}

			// Check blank input
			if (strlen(inputBuffer) == 0 || strspn(inputBuffer, " \t") == strlen(inputBuffer)) {
				printf("Invalid input! Student ID cannot be blank. Try again or 'm' for Main Menu\n");
				continue;
			}

			// Check all digits
			int allDigits = 1;
			for (int i = 0; inputBuffer[i] != '\0'; i++) {
				if (!isdigit(inputBuffer[i])) {
					allDigits = 0;
					break;
				}
			}

			if (!allDigits) {
				printf("Invalid input! Student ID must contain only numbers. Try again or 'm' for Main Menu\n");
				continue;
			}

			// Check length
			if (strlen(inputBuffer) != 6) {
				printf("Invalid input! Student ID must be exactly 6 digits. Try again or 'm' for Main Menu\n");
				continue;
			}

			*studentID = atoi(inputBuffer);

			if (isDuplicateID(ht, *studentID)) {
				char response;
				do {
					printf("Warning: This Student ID already has a meeting!\n");
					printf("Continue anyway? (y/n): ");
					scanf_s(" %c", &response, (int)sizeof(response));
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
		printf("Enter Name : ");
		if (fgets(name, MAX_NAME_LENGTH, stdin) != NULL) {
			name[strcspn(name, "\n")] = '\0';

			// Check for menu request
			if (tolower(name[0]) == 'm' && strlen(name) == 1) {
				*studentID = -1;
				return;
			}

			// Check blank input
			if (strlen(name) == 0 || strspn(name, " \t") == strlen(name)) {
				printf("Invalid input! Name cannot be blank. Try again or 'm' for Main Menu\n");
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
				printf("Invalid input! Name can only contain letters and spaces. Try again or 'm' for Main Menu\n");
				continue;
			}

			// Check duplicate name with inline confirmation
			if (isDuplicateName(ht, name)) {
				char response;
				do {
					printf("Warning: This name already has a meeting!\n");
					printf("Continue anyway? (y/n): ");
					scanf_s(" %c", &response, (int)sizeof(response));
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

	// Title validation
	do {
		printf("Enter Meeting Title : ");
		if (fgets(title, MAX_TITLE_LENGTH, stdin) != NULL) {
			title[strcspn(title, "\n")] = '\0';

			// Check for menu request
			if (tolower(title[0]) == 'm' && strlen(title) == 1) {
				*studentID = -1;
				return;
			}

			if (strlen(title) == 0 || strspn(title, " \t") == strlen(title)) {
				printf("Invalid input! Title cannot be blank. Try again or 'm' for Main Menu\n");
				continue;
			}
			if (strlen(title) >= MAX_TITLE_LENGTH - 1) {
				cleanBuffer();
				printf("Invalid input! Title too long (max %d chars). Try again or 'm' for Main Menu\n",
					MAX_TITLE_LENGTH - 1);
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