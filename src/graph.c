#include "railx.h"

/* Global Graph Storage & Algorithm State */
Station stations[MAX_STATIONS];
int stationCount = 0;

int visited[MAX_STATIONS];
int distanceArr[MAX_STATIONS];
int timeArr[MAX_STATIONS];
int parent[MAX_STATIONS];
int arrivalLine[MAX_STATIONS];

/* ================= UTILITY FUNCTIONS ================= */

/* Safely read string input from stdin and strip trailing newline */
void inputString(char str[], int size) {
    if (fgets(str, size, stdin) != NULL) {
        str[strcspn(str, "\n")] = 0;
    }
}

/* Format total travel minutes into Day X, HH:MM clock time */
void formatClockTime(int totalMinutes, char* outputBuf) {
    int day = (totalMinutes / (24 * 60)) + 1;
    int minsInDay = totalMinutes % (24 * 60);
    int hours = minsInDay / 60;
    int mins = minsInDay % 60;

    sprintf(outputBuf, "Day %d, %02d:%02d", day, hours, mins);
}

/* Print duration in human-readable Days, Hours, and Minutes */
void printFormattedDuration(int totalMinutes) {
    int days = totalMinutes / (24 * 60);
    int remainingMins = totalMinutes % (24 * 60);
    int hours = remainingMins / 60;
    int mins = remainingMins % 60;

    if (days > 0) {
        printf("%d Days, %d Hours, %d Mins", days, hours, mins);
    } else if (hours > 0) {
        printf("%d Hours, %d Mins", hours, mins);
    } else {
        printf("%d Mins", mins);
    }
}

/* Calculate ticket fare based on distance, duration, and service type */
int calculateFare(int distanceKm, int totalMinutes, int serviceType) {
    double fare = 20.0 + (distanceKm * 0.30) + (totalMinutes * 0.05);
    
    // Apply 20% Fast Express premium surcharge
    if (serviceType == SERVICE_FAST) {
        fare *= 1.20;
    }
    
    return (int)fare;
}

/* ================= GRAPH MANAGEMENT ================= */

/* Search station index by name (-1 if not found) */
int getStationIndex(const char name[]) {
    for (int i = 0; i < stationCount; i++) {
        if (strcmp(stations[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/* Register station into global database */
int addStation(const char name[], int isMajor) {
    if (strlen(name) == 0 || stationCount >= MAX_STATIONS) return -1;

    int existing = getStationIndex(name);
    if (existing != -1) {
        if (isMajor) stations[existing].isMajor = 1;
        return existing;
    }

    strcpy(stations[stationCount].name, name);
    stations[stationCount].isMajor = isMajor;
    stations[stationCount].head = NULL;

    return stationCount++;
}

/* Add bidirectional track edge between two stations */
void addConnection(int from, int to, int distance, int time, int line, int serviceType) {
    // Edge: from -> to
    Edge* e1 = (Edge*)malloc(sizeof(Edge));
    e1->to = to;
    e1->distance = distance;
    e1->time = time;
    e1->line = line;
    e1->serviceType = serviceType;
    e1->next = stations[from].head;
    stations[from].head = e1;

    // Edge: to -> from
    Edge* e2 = (Edge*)malloc(sizeof(Edge));
    e2->to = from;
    e2->distance = distance;
    e2->time = time;
    e2->line = line;
    e2->serviceType = serviceType;
    e2->next = stations[to].head;
    stations[to].head = e2;
}

/* ================= PATHFINDING ENGINE ================= */

/* Dijkstra's shortest-path algorithm optimized for total time & transfer delay */
void dijkstra(int start, int preferredService) {
    for (int i = 0; i < stationCount; i++) {
        distanceArr[i] = INT_MAX;
        timeArr[i] = INT_MAX;
        visited[i] = 0;
        parent[i] = -1;
        arrivalLine[i] = -1;
    }

    distanceArr[start] = 0;
    timeArr[start] = 0;

    for (int i = 0; i < stationCount - 1; i++) {
        int u = -1;
        int minTime = INT_MAX;

        // Pick unvisited station with minimal calculated time
        for (int j = 0; j < stationCount; j++) {
            if (!visited[j] && timeArr[j] < minTime) {
                minTime = timeArr[j];
                u = j;
            }
        }

        if (u == -1) break;
        visited[u] = 1;

        Edge* temp = stations[u].head;
        while (temp != NULL) {
            int v = temp->to;

            // Validate service mode compatibility
            int serviceValid = 0;
            if (preferredService == SERVICE_BOTH) {
                serviceValid = 1;
            } else if (preferredService == SERVICE_FAST) {
                serviceValid = (temp->serviceType == SERVICE_FAST || temp->serviceType == SERVICE_BOTH);
            } else if (preferredService == SERVICE_SLOW) {
                serviceValid = (temp->serviceType == SERVICE_SLOW || temp->serviceType == SERVICE_BOTH);
            }

            if (serviceValid) {
                // Apply +10 minute line transfer penalty if changing line IDs
                int transferCost = 0;
                if (arrivalLine[u] != -1 && arrivalLine[u] != temp->line) {
                    transferCost = TRANSFER_PENALTY_MINS;
                }

                int newTime = timeArr[u] + temp->time + transferCost;
                int newDist = distanceArr[u] + temp->distance;

                // Relax edge if a faster path is found
                if (!visited[v] && newTime < timeArr[v]) {
                    timeArr[v] = newTime;
                    distanceArr[v] = newDist;
                    parent[v] = u;
                    arrivalLine[v] = temp->line;
                }
            }
            temp = temp->next;
        }
    }
}

/* Print all system stations directory */
void printStations() {
    printf("\n=========================================================\n");
    printf("               BVDU-RAILX SYSTEM STATIONS                \n");
    printf("=========================================================\n");
    for (int i = 0; i < stationCount; i++) {
        printf("%2d. %-25s [%s]\n", 
               i + 1, 
               stations[i].name, 
               stations[i].isMajor ? "⚡ MAJOR HUB" : "  Local Station");
    }
    printf("=========================================================\n");
}
