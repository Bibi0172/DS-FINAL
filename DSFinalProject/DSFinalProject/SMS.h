#define _CRT_SECURE_NO_WARNINGS 

#ifndef SMS_H
#define SMS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TOTAL_DAYS 7
#define MAX_SLOTS 3


typedef struct Meeting {
    int studentID;
    char name[50];
    char title[100];
    int time;
    struct Meeting* next;
} Meeting;

typedef struct Queue {
    Meeting* front;
    Meeting* back;
    int count;
} Queue;

typedef struct HashTable {
    char date[12];
    Queue* meetingQueue;
} HashTable;

extern char* dates[TOTAL_DAYS];

void initializeHashTable(HashTable* ht);
void insertMeeting(HashTable* ht, char* date, int studentID, char* name, char* title, int time);
void cancelMeeting(HashTable* ht, char* date, int studentID);
void searchMeeting(HashTable* ht, char* date, int studentID);
void viewUpcomingMeetings(HashTable* ht);

#endif
