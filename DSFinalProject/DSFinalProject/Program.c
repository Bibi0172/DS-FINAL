#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SLOTS 5  // Max slots per day
#define TOTAL_DAYS 4  // March 20-23

// Meeting structure (Node in queue)
typedef struct Meeting {
	int studentID;
	char name[50];
	char title[100];
	int time;  // HHMM format
	struct Meeting* next;
} Meeting;

// Queue structure (FIFO for each day)
typedef struct Queue {
	Meeting* front;
	Meeting* back;
	int count;  // Track slots booked
} Queue;

// Hash Table for Dates
typedef struct HashTable {
	char date[12];  // Store "March 20", etc.
	Queue* meetingQueue; // Queue of meetings for this day
} HashTable;

typedef struct Stack {
	Meeting* Top;
} Stack;

void initializeHashTable(HashTable* ht) {
	char* dates[TOTAL_DAYS] = { "March 20", "March 21", "March 22", "March 23" };

	for (int i = 0; i < TOTAL_DAYS; i++) {
		strcpy(ht[i].date, dates[i]);
		ht[i].meetingQueue = (Queue*)malloc(sizeof(Queue));
		ht[i].meetingQueue->front = ht[i].meetingQueue->back = NULL;
		ht[i].meetingQueue->count = 0;
	}
}
void insertMeeting(HashTable* ht, char* date, int studentID, char* name, char* title, int time) {
	// Find the date index in hash table
	int index = -1;
	for (int i = 0; i < TOTAL_DAYS; i++) {
		if (strcmp(ht[i].date, date) == 0) {
			index = i;
			break;
		}
	}

	if (index == -1) {
		printf("Invalid date! Only March 20-23 are allowed.\n");
		return;
	}

	Queue* q = ht[index].meetingQueue;

	if (q->count >= MAX_SLOTS) {
		printf("No available slots on %s. Please choose another day.\n", date);
		return;
	}

	// Create new meeting node
	Meeting* newMeeting = (Meeting*)malloc(sizeof(Meeting));
	newMeeting->studentID = studentID;
	strcpy(newMeeting->name, name);
	strcpy(newMeeting->title, title);
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
void cancelMeeting(HashTable* ht, char* date, int studentID) {
	int index = -1;
	for (int i = 0; i < TOTAL_DAYS; i++) {
		if (strcmp(ht[i].date, date) == 0) {
			index = i;
			break;
		}
	}

	if (index == -1) {
		printf("Invalid date! Please enter between March 20-23.\n");
		return;
	}

	Queue* q = ht[index].meetingQueue;
	if (q->front == NULL) {
		printf("No meetings found on %s.\n", date);
		return;
	}

	Meeting* temp = q->front, * prev = NULL;

	// Search for meeting with matching Student ID
	while (temp != NULL && temp->studentID != studentID) {
		prev = temp;
		temp = temp->next;
	}

	if (temp == NULL) {
		printf("No meeting found for Student ID %d on %s.\n", studentID, date);
		return;
	}

	// Remove node
	if (prev == NULL) { // First node case
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
}
void searchMeeting(HashTable* ht, char* date, int studentID) {
	int index = -1;
	for (int i = 0; i < TOTAL_DAYS; i++) {
		if (strcmp(ht[i].date, date) == 0) {
			index = i;
			break;
		}
	}

	if (index == -1) {
		printf("Invalid date! Please enter between March 20-23.\n");
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
	printf("\nUpcoming Meetings:\n");
	for (int i = 0; i < TOTAL_DAYS; i++) {
		Queue* q = ht[i].meetingQueue;
		Meeting* temp = q->front;

		printf("Date: %s\n", ht[i].date);
		while (temp != NULL) {
			printf("  - Student: %s (ID: %d) | Title: %s | Time: %d\n", temp->name, temp->studentID, temp->title, temp->time);
			temp = temp->next;
		}
		printf("\n");
	}
}
int main() {
	HashTable meetingTable[TOTAL_DAYS];
	initializeHashTable(meetingTable);

	int choice, studentID, time;
	char date[12], name[50], title[100];

	do {
		printf("\n1. Book Meeting\n2. Cancel Meeting\n3. Search Meeting\n4. View Upcoming Meetings\n5. Exit\nEnter choice: ");
		scanf("%d", &choice);
		getchar(); // Flush newline

		switch (choice) {
		case 1:
			printf("Enter date (March 20-23): ");
			gets(date);
			printf("Enter Student ID: ");
			scanf("%d", &studentID);
			getchar();
			printf("Enter Name: ");
			gets(name);
			printf("Enter Meeting Title: ");
			gets(title);
			printf("Enter Time (HHMM): ");
			scanf("%d", &time);
			insertMeeting(meetingTable, date, studentID, name, title, time);
			break;

		case 2:
			printf("Enter date and Student ID: ");
			gets(date);
			scanf("%d", &studentID);
			cancelMeeting(meetingTable, date, studentID);
			break;

		case 3:
			printf("Enter date and Student ID: ");
			gets(date);
			scanf("%d", &studentID);
			searchMeeting(meetingTable, date, studentID);
			break;

		case 4:
			viewUpcomingMeetings(meetingTable);
			break;
		}
	} while (choice != 5);

	return 0;
}