#ifndef RAILX_H
#define RAILX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* System Limits & Configuration */
#define MAX_STATIONS 100
#define MAX_NAME 50
#define MAX_TRAINS 20
#define MAX_BOOKINGS 100
#define SEATS_PER_TRAIN 20
#define TRANSFER_PENALTY_MINS 10

/* Service Types */
#define SERVICE_SLOW 1
#define SERVICE_FAST 2
#define SERVICE_BOTH 3

/* ================= DATA STRUCTURES ================= */

typedef struct Edge {
    int to;
    int distance;
    int time;
    int line;          // 1 = Black Line, 2 = White Line, 3 = Grey Line
    int serviceType;   // 1 = Slow, 2 = Fast, 3 = Both
    struct Edge* next;
} Edge;

typedef struct {
    char name[MAX_NAME];
    int isMajor;       // 1 = Major Hub, 0 = Local Station
    Edge* head;
} Station;

typedef struct {
    char trainNo[10];
    char trainName[MAX_NAME];
    int originIndex;
    int destIndex;
    int departureMins;
    int line;
    int serviceType;
    int totalSeats;
    int availableSeats;
    int seatMap[SEATS_PER_TRAIN]; // 0 = Open, 1 = Booked
} Train;

typedef struct {
    char pnr[12];
    char passengerName[MAX_NAME];
    char trainNo[10];
    int seatNumber;
    int startStationIdx;
    int endStationIdx;
    int fare;
    int active;        // 1 = Active, 0 = Cancelled
} Booking;

/* ================= SHARED GLOBAL VARIABLES ================= */

extern Station stations[MAX_STATIONS];
extern int stationCount;

extern Train trainSchedules[MAX_TRAINS];
extern int trainCount;

extern Booking pnrDatabase[MAX_BOOKINGS];
extern int bookingCount;
extern int pnrCounter;

/* Dijkstra Algorithm State Tracking Arrays */
extern int visited[MAX_STATIONS];
extern int distanceArr[MAX_STATIONS];
extern int timeArr[MAX_STATIONS];
extern int parent[MAX_STATIONS];
extern int arrivalLine[MAX_STATIONS];

/* ================= FUNCTION PROTOTYPES ================= */

/* Utility Functions (graph.c) */
void inputString(char str[], int size);
void formatClockTime(int totalMinutes, char* outputBuf);
void printFormattedDuration(int totalMinutes);
int calculateFare(int distanceKm, int totalMinutes, int serviceType);

/* Graph & Network Functions (graph.c) */
int getStationIndex(const char name[]);
int addStation(const char name[], int isMajor);
void addConnection(int from, int to, int distance, int time, int line, int serviceType);
void dijkstra(int start, int preferredService);
void printStations();

/* Timetable & Schedule Functions (timetable.c) */
void addTrainSchedule(const char trainNo[], const char name[], const char origin[], const char dest[], int depMins, int line, int serviceType);
void seedMasterNetworkAndTimetable();
void viewAllSchedules();
void printTrainDetailedItinerary(int trainIdx);

/* Seat Matrix & Booking Engine Functions (booking.c) */
void displaySeatMap(int trainIdx);
void bookTicketDirect(const char targetTrainNo[]);
void bookTicket();
void checkPNRStatus();
void cancelTicket();

#endif /* RAILX_H */
