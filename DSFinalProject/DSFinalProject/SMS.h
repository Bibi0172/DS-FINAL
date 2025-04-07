#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SMS_H
#define SMS_H

#define TOTAL_DAYS 7
#define MAX_SLOTS 3
#define MAX_NAME_LENGTH		50
#define MAX_TITLE_LENGTH	100
#define MAX_DATE_LENGTH     12

typedef struct Meeting {
    int studentID;
    char name[MAX_NAME_LENGTH];
    char title[MAX_TITLE_LENGTH];
    int time;
    struct Meeting* next;
} Meeting;

typedef struct Queue {
    Meeting* front;
    Meeting* back;
    int count;
} Queue;

typedef struct HashTable {
    char date[MAX_DATE_LENGTH];
    Queue* meetingQueue;
} HashTable;

extern char* dates[TOTAL_DAYS];

void initializeHashTable(HashTable* ht);
void insertMeeting(HashTable* ht, char* date, int studentID, char* name, char* title, int time);
void cancelMeeting(HashTable* ht, char* date, int studentID);
void searchMeeting(HashTable* ht, char* date, int studentID);
void viewUpcomingMeetings(HashTable* ht);

void freeAllDataStructures(HashTable* meetingTable);

#endif
