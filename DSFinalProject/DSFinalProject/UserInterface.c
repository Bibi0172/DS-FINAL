#include "UserInterface.h"

//Function: displayMenu
//Description: Displays the main menu and handles user selections
//Parameters: HashTable* ht, OperationStack* operationStack, KVP** logList
//Returns: None
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
		printf("\n+--------------------------------------------------+\n");
		printf("|            Student Meeting Scheduler             |\n");
		printf("+--------------------------------------------------+\n");
		printf("| 1. Book a Meeting                                |\n");
		printf("| 2. View Upcoming Meetings                        |\n");
		printf("| 3. Process Meeting                               |\n");
		printf("| 4. Cancel a Meeting                              |\n");
		printf("| 5. Search for a Meeting                          |\n");
		printf("| 6. View Meeting History                          |\n");
		printf("| 7. Undo Last Operation                           |\n");
		printf("| 8. Display Operation Log                         |\n");
		printf("| 9. Exit Scheduler                                |\n");
		printf("+--------------------------------------------------+\n");
		printf("Select an option (1-9): ");

		menuNum = validMenuChoice(9);

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

			getValidUserInput(ht, &studentID, name, title);
			if (studentID == -1)
				break;

			insertMeeting(ht, dates[dateChoice], studentID, name, title);

			// Create operation record for booking
			op.type = OP_BOOK;
			strcpy_s(op.date, MAX_DATE_LENGTH, dates[dateChoice]);
			op.studentID = studentID;
			strcpy_s(op.name, MAX_NAME_LENGTH, name);
			strcpy_s(op.title, MAX_TITLE_LENGTH, title);

			pushStack(operationStack, op);

			sprintf_s(logKey, sizeof(logKey), "LOG%d", logCounter++);
			sprintf_s(logMsg, sizeof(logMsg), "Booked meeting on %s for Student ID %d .", dates[dateChoice], studentID);
			insertKVP(logList, logKey, logMsg);
			break;


		case VIEW:
			viewUpcomingMeetings(ht);
			break;

		case PROCESS_MEETING:
			processFirstMeeting(ht, operationStack, logList);
			break;

		case CANCEL:
			printf("\n+--------------------------------------------------+\n");
			printf("| Please enter the Student ID to Cancel for:       |\n");
			printf("+--------------------------------------------------+\n");
			printf("Enter Student ID: ");
			studentID = getUserInput();

			if (studentID == -1 || studentID == -2)
				break;

			// Get date choice for the specific student
			dateChoice = getValidDateChoiceForStudent(ht, studentID);
			if (dateChoice == -1)
				break;

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
			printf("\n+--------------------------------------------------+\n");
			printf("| Please enter the Student ID to search for:       |\n");
			printf("+--------------------------------------------------+\n");
			printf("Enter Student ID: ");
			studentID = getUserInput();
			if (studentID == -1)
				break;
			if (studentID == -2)
				break;

			searchMeeting(ht, NULL, studentID);
			break;

		case VIEW_HISTORY:
			viewMeetingHistory();
			break;

		case UNDO:
			undoLastOperation(ht, operationStack, logList);
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

//Function: getUserInput
//Description: Gets and validates numeric input from user for Student ID.
//Parameters: None
//Returns: int
int getUserInput() {
	char inputBuffer[MAX_BUFFER_SIZE] = "";
	int value = 0;

	while (1) {
		if (fgets(inputBuffer, sizeof(inputBuffer), stdin) != NULL) {
			inputBuffer[strcspn(inputBuffer, "\n")] = '\0';

			if (tolower(inputBuffer[0]) == 'm' && strlen(inputBuffer) == 1) {
				return -1;
			}

			if (strlen(inputBuffer) == 0 || strspn(inputBuffer, " \t") == strlen(inputBuffer)) {
				printf("Invalid input! Student ID cannot be blank. Try again or 'm' for Main Menu\n");
				continue;
			}

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

			if (strlen(inputBuffer) != 6) {
				printf("Invalid input! Student ID must be exactly 6 digits. Try again or 'm' for Main Menu\n");
				continue;
			}

			// Convert to integer
			if (sscanf_s(inputBuffer, "%d", &value) == 1) {
				return value;
			}
		}
		else {
			// Handle EOF case
			cleanBuffer();
			return -2;
		}
		printf("Invalid input format. Please try again or 'm' for Main Menu\n");

	}

}

//Function: cleanBuffer
//Description: Clears any remaining input from the input buffer.
//Parameters: None
//Returns: None
void cleanBuffer() {
	int extraChar = 0;
	while ((extraChar = getchar()) != '\n' && extraChar != EOF);
}

// Function: validMenuChoice
// Description: Prompts user for a menu selection and validates it.
// Parameters: int maxNumber - the maximum valid menu option
// Returns: int - the validated choice from the user
int validMenuChoice(int maxNumber) {
	int choice;
	char buffer[100];
	char extra;

	while (1) {

		if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
			// Check for empty input (just enter key)
			if (buffer[0] == '\n') {
				printf("Invalid Input. Input cannot be empty.\n");
				continue;
			}

			// Check if input is a valid integer and doesn't have extra characters
			if (sscanf_s(buffer, "%d %c", &choice, &extra, 1) == 1) {
				if (choice >= 1 && choice <= maxNumber) {
					return choice;
				}
			}

			// Otherwise it's invalid
			printf("Invalid Input. Please enter a number between 1 and %d.\n", maxNumber);
		}
		else {
			cleanBuffer(); // In case of EOF or input error
		}
	}
}

//Function: getValidDateChoice
//Description: Displays dates with availability and prompts user to choose a date.
//Parameters: HashTable* ht
//Returns: int
int getValidDateChoice(HashTable* ht) {
	char buffer[100];
	int choice;

	char extra;
	printf("\n+-----------------------------------------------------------+\n");
	printf("|                    Available Dates                        |\n");
	printf("+------+-------------+---------------+----------------------+\n");
	printf("| No.  |    Date     |  Booked Slots |   Available Slots    |\n");
	printf("+------+-------------+---------------+----------------------+\n");

	for (int i = 0; i < TOTAL_DAYS; i++) {
		int booked = ht[i].meetingQueue->count;
		int available = MAX_SLOTS - booked;
		printf("| %-4d | %-11s | %-13d | %-20d |\n", i + 1, dates[i], booked, available);
	}
	printf("+------+-------------+---------------+----------------------+\n");

	while (1) {

		printf("Enter your choice (1-%d) or M to return to the main menu: ", TOTAL_DAYS);

		if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
			// Check for 'M' or 'm' to return to main menu
			if (buffer[0] == 'M' || buffer[0] == 'm') {
				if (buffer[1] == '\n' || buffer[1] == '\0') {
					return -1;  // Return to main menu
				}
			}
			if (sscanf_s(buffer, "%d %c", &choice, &extra, 1) == 1) {
				if (choice >= 1 && choice <= TOTAL_DAYS) {
					return choice;
				}
			}
			// Try to read as integer
			if (sscanf_s(buffer, "%d", &choice) == 1) {
				if (choice >= 1 && choice <= TOTAL_DAYS) {
					return choice - 1;
				}
			}

			// If we get here, input was invalid
			printf("Invalid choice. Please enter a number between 1 and %d or M to return.\n", TOTAL_DAYS);
		}
		else {
			// Handle EOF case
			cleanBuffer();
			return -1;
		}
	}
}

// Function: isDuplicateID
// Description: Checks if a student ID already exists in the hash table.
// Parameters: HashTable* ht - the hash table to search
//             int studentID - the ID to check for duplicates
// Returns: bool - true if duplicate exists, false otherwise
bool isDuplicateID(HashTable* ht, int studentID) {
	for (int i = 0; i < TOTAL_DAYS; i++) {
		Meeting* current = ht[i].meetingQueue->front;
		while (current != NULL) {
			if (current->studentID == studentID) {
				return true; // Duplicate found
			}
			current = current->next;
		}
	}
	return false; // No duplicate found
}

// Function: isDuplicateName
// Description: Checks if a student name already exists in the hash table.
// Parameters: HashTable* ht - the hash table to search
//             const char* name - the name to check for duplicates
// Returns: bool - true if duplicate exists, false otherwise
bool isDuplicateName(HashTable* ht, const char* name) {
	for (int i = 0; i < TOTAL_DAYS; i++) {
		Meeting* current = ht[i].meetingQueue->front;
		while (current != NULL) {
			if (strcmp(current->name, name) == 0) {
				return true; // Duplicate found
			}
			current = current->next;
		}
	}
	return false; // No duplicate found
}

//Function: getValidStudentID
//Description: Prompts user to enter a valid 6-digit student ID. If it's a duplicate, it asks for confirmation and retrieves the existing name if accepted.
//Parameters: HashTable* ht - the hash table to check for duplicates,
//            char* existingName - buffer to store the name if duplicate ID is reused
//Returns: int - the validated student ID, or -1 if the user chooses to return to the menu
int getValidStudentID(HashTable* ht, char* existingName)
{
	char inputBuffer[50];
	int studentID;

	while (1) {
		printf("Enter Student ID : ");
		if (fgets(inputBuffer, sizeof(inputBuffer), stdin) != NULL) {
			inputBuffer[strcspn(inputBuffer, "\n")] = '\0';

			if (tolower(inputBuffer[0]) == 'm' && strlen(inputBuffer) == 1) {
				return -1;
			}

			if (strlen(inputBuffer) == 0 || strspn(inputBuffer, " \t") == strlen(inputBuffer)) {
				printf("Invalid input! Student ID cannot be blank. Try again or 'm' for Main Menu\n");
				continue;
			}

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

			if (strlen(inputBuffer) != 6) {
				printf("Invalid input! Student ID must be exactly 6 digits. Try again or 'm' for Main Menu\n");
				continue;
			}

			studentID = atoi(inputBuffer);

			if (isDuplicateID(ht, studentID)) {
				char response;
				do {
					printf("Warning: This Student ID already has a meeting!\n");
					printf("Continue anyway? (y/n): ");
					scanf_s(" %c", &response, (int)sizeof(response));
					cleanBuffer();
					response = tolower(response);

					if (response == 'n') {
						break;
					}
					else if (response == 'y') {
						// Look for the matching name and copy it into existingName
						for (int i = 0; i < TOTAL_DAYS; i++) {
							Meeting* current = ht[i].meetingQueue->front;
							while (current != NULL) {
								if (current->studentID == studentID) {
									strcpy_s(existingName, MAX_NAME_LENGTH, current->name);
									printf("Using existing name: %s\n", existingName);
									return studentID;
								}
								current = current->next;
							}
						}
					}
					printf("Invalid input! Please enter 'y' or 'n' only.\n");
				} while (1);
				continue;
			}
			return studentID;
		}
	}
}

//Function: getValidName
//Description: Prompts user to enter a valid name and checks for duplicates with confirmation.
//Parameters: HashTable* ht - the hash table to check for duplicate names,
//            char* name - buffer to store the validated name
//Returns: None
void getValidName(HashTable* ht, char* name)

{
	while (1) {
		printf("Enter Name : ");
		if (fgets(name, MAX_NAME_LENGTH, stdin) != NULL) {
			name[strcspn(name, "\n")] = '\0';

			if (tolower(name[0]) == 'm' && strlen(name) == 1) {
				name[0] = '\0';
				return;
			}

			if (strlen(name) == 0 || strspn(name, " \t") == strlen(name)) {
				printf("Invalid input! Name cannot be blank. Try again or 'm' for Main Menu\n");
				continue;
			}

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

			if (isDuplicateName(ht, name)) {
				char response;
				do {
					printf("Warning: This name already has a meeting!\n");
					printf("Continue anyway? (y/n): ");
					scanf_s(" %c", &response, (int)sizeof(response));
					cleanBuffer();
					response = tolower(response);

					if (response == 'n') {
						break;
					}
					else if (response == 'y') {
						return;
					}
					printf("Invalid input! Please enter 'y' or 'n' only.\n");
				} while (1);
				continue;
			}
			return;
		}
	}
}

//Function: getValidTitle
//Description: Prompts user to enter a valid non-blank meeting title and stores it.
//Parameters: char* title - buffer to store the validated title
//Returns: None
void getValidTitle(char* title)
{
	while (1) {
		printf("Enter Meeting Title : ");
		if (fgets(title, MAX_TITLE_LENGTH, stdin) != NULL) {
			title[strcspn(title, "\n")] = '\0';

			if (tolower(title[0]) == 'm' && strlen(title) == 1) {
				title[0] = '\0';
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
			return;
		}
	}
}

//Function: getValidUserInput
//Description: Combines student ID, name, and title validation to gather input for booking a meeting.
//Parameters: HashTable* ht - the hash table to check for duplicates,
//            int* studentID - pointer to store the validated student ID,
//            char* name - buffer to store the name,
//            char* title - buffer to store the meeting title
//Returns: None
void getValidUserInput(HashTable* ht, int* studentID, char* name, char* title)
{
	// Clear name buffer at start to ensure fresh input
	name[0] = '\0';

	*studentID = getValidStudentID(ht, name);
	if (*studentID == -1)
		return;

	// Only skip name input if we got a name from duplicate ID confirmation
	if (name[0] == '\0') {
		getValidName(ht, name);
		if (name[0] == '\0') {
			*studentID = -1;
			return;
		}
	}

	getValidTitle(title);
	if (title[0] == '\0') {
		*studentID = -1;
	}
}

// Function: getValidDateChoiceForStudent
// Description: Displays dates with meetings for a specific student ID and
// prompts user to choose a date. Parameters: HashTable* ht - the hash table,
//             int studentID - the student ID to search for
// Returns: int - the index of the chosen date or -1 if cancelled
int getValidDateChoiceForStudent(HashTable* ht, int studentID)
{
	char buffer[100];
	int choice;
	int validDates[TOTAL_DAYS * MAX_SLOTS]; // Increased size to handle multiple
	int validCount = 0;

	// First, find all meetings for this student
	for (int i = 0; i < TOTAL_DAYS; i++) {
		Meeting* current = ht[i].meetingQueue->front;
		while (current != NULL) {
			if (current->studentID == studentID) {
				validDates[validCount++] = i;
			}
			current = current->next;
		}
	}

	if (validCount == 0) {
		printf("No meetings found for Student ID %d.\n", studentID);
		return -1;
	}

	printf("\n+-----------------------------------------------------------+\n");
	printf("|                    Available Dates for Student ID %d        |\n", studentID);
	printf("+------+-------------+---------------+----------------------+\n");
	printf("| No.  |    Date     |  Student Name |      Meeting Title   |\n");
	printf("+------+-------------+---------------+----------------------+\n");

	for (int i = 0; i < validCount; i++) {
		int dateIndex = validDates[i];
		Meeting* current = ht[dateIndex].meetingQueue->front;
		while (current != NULL) {
			if (current->studentID == studentID) {
				printf("| %-4d | %-11s | %-13s | %-20s |\n", i + 1, dates[dateIndex], current->name, current->title);
				break;
			}
			current = current->next;
		}
	}
	printf("+------+-------------+---------------+----------------------+\n");

	while (1) {
		printf("Enter your choice (1-%d) or M to return to the main menu: ", validCount);

		if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
			// Check for 'M' or 'm' to return to main menu
			if (buffer[0] == 'M' || buffer[0] == 'm') {
				if (buffer[1] == '\n' || buffer[1] == '\0') {
					return -1; // Return to main menu
				}
			}

			// Try to read as integer
			if (sscanf_s(buffer, "%d", &choice) == 1) {
				if (choice >= 1 && choice <= validCount) {
					return validDates[choice - 1]; // Return the actual date index
				}
			}

			// If we get here, input was invalid
			printf("Invalid choice. Please enter a number between 1 and %d or M to return.\n", validCount);
		}
		else {
			// Handle EOF case
			cleanBuffer();
			return -1;
		}
	}
}