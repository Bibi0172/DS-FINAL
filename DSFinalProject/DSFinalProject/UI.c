#include "UI.h"

void displayMenu(HashTable* ht) {
	if (ht == NULL) {
		printf("Error: Invalid hash table\n");
		return;
	}

	int choice = 0, studentID = 0, dateChoice = 0, timeChoice = 0;
	char name[MAX_NAME_LENGTH] = "", title[MAX_TITLE_LENGTH] = "";

	do {
		printf("\n===========================================\n");
		printf("          Student Meeting Scheduler        \n");
		printf("===========================================\n");
		printf("1. Book Meeting\n2. Cancel Meeting\n3. Search Meeting\n4. View Upcoming Meetings\n5. Exit\n");

		choice = validMenuChoice(5);

		switch (choice) {
		case BOOK:
			dateChoice = getValidDateChoice();
			if (dateChoice == -1) break;

			Queue* q = ht[dateChoice].meetingQueue;
			timeChoice = getValidTimeSlot(q);
			if (timeChoice == -1) break;

			getValidUserInput(&studentID, name, title);
			insertMeeting(ht, dates[dateChoice], studentID, name, title, timeChoice);
			break;

		case CANCEL:
			dateChoice = getValidDateChoice();
			if (dateChoice == -1) break;
			printf("Enter Student ID: ");
			scanf_s("%d", &studentID);
			cancelMeeting(ht, dates[dateChoice], studentID);
			break;

		case SEARCH:
			dateChoice = getValidDateChoice();
			if (dateChoice == -1) break;
			printf("Enter Student ID: ");
			scanf_s("%d", &studentID);
			searchMeeting(ht, dates[dateChoice], studentID);
			break;

		case VIEW:
			viewUpcomingMeetings(ht);
			break;

		case EXIT_PROGRAM:
			printf("Exiting... Goodbye!\n");
			break;
		}
	} while (choice != EXIT_PROGRAM);
}

int validMenuChoice(int maxNumber) {
	int choice = 0;
	while (1) {
		printf("Enter your choice: ");
		if (scanf_s("%d", &choice) == 1 && ((choice >= 1 && choice <= maxNumber) || choice == -1)) {
			if (getchar() == '\n') return choice;  // If valid input, return the choice
		}
		cleanBuffer();
		printf("Invalid Input. Please enter a number between 1 and %d OR -1 to return to main menu.\n", maxNumber);
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

		// Check for return to main menu
		if (choice == -1) {
			return -1;  // Return to main menu
		}

		// Validate choice within range
		if (choice >= 1 && choice <= TOTAL_DAYS) {
			return choice - 1;  // Return valid date index
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

		if (choice >= 1 && choice <= 3) {
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

int isTimeSlotBooked(Queue* q, int time){
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
	scanf_s("%d", studentID);
	cleanBuffer();

	printf("Enter Name: ");
	fgets(name, MAX_NAME_LENGTH, stdin);
	name[strcspn(name, "\n")] = 0;

	printf("Enter Meeting Title: ");
	fgets(title, MAX_TITLE_LENGTH, stdin);
	title[strcspn(title, "\n")] = 0;
}

