#include "SMS.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* dates[TOTAL_DAYS] = { "May 1", "May 2", "May 3", "May 4", "May 5", "May 6", "May 7"};

void insertMeeting(HashTable* ht, char* date, int studentID, char* name, char* title, int time) {

	int index = -1;
	for (int i = 0; i < TOTAL_DAYS; i++) {
		if (strcmp(ht[i].date, date) == 0) {
			index = i;
			break;
		}
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

		printf("Date: %s - %s\n", ht[i].date, (q->count >= MAX_SLOTS) ? " Full" : " Available");
		while (temp != NULL) {
			printf("  - Student: %s (ID: %d) | Title: %s | Time: %d\n", temp->name, temp->studentID, temp->title, temp->time);
			temp = temp->next;
		}
		printf("\n");
	}
}

