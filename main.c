#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_STATIONS 100
#define MAX_NAME 50

typedef struct Edge {
    int to;
    int distance;
    int time;
    struct Edge* next;
} Edge;

typedef struct {
    char name[MAX_NAME];
    int line;
    Edge* head;
} Station;

Station stations[MAX_STATIONS];
int stationCount = 0;

int visited[MAX_STATIONS];
int distanceArr[MAX_STATIONS];
int timeArr[MAX_STATIONS];
int parent[MAX_STATIONS];

int getStationIndex(char name[]) {
    for (int i = 0; i < stationCount; i++) {
        if (strcmp(stations[i].name, name) == 0)
            return i;
    }
    return -1;
}

void addStation(char name[], int line) {
    strcpy(stations[stationCount].name, name);
    stations[stationCount].line = line;
    stations[stationCount].head = NULL;
    stationCount++;
}

void addConnection(int from, int to, int distance, int time) {
    Edge* e = (Edge*)malloc(sizeof(Edge));
    e->to = to;
    e->distance = distance;
    e->time = time;
    e->next = stations[from].head;
    stations[from].head = e;

    Edge* r = (Edge*)malloc(sizeof(Edge));
    r->to = from;
    r->distance = distance;
    r->time = time;
    r->next = stations[to].head;
    stations[to].head = r;
}

void printStations() {
    printf("\nStations in Network:\n\n");
    for (int i = 0; i < stationCount; i++) {
        printf("%d. %s (Line %d)\n", i, stations[i].name, stations[i].line);
    }
}

int minDistance() {
    int min = INT_MAX;
    int index = -1;

    for (int i = 0; i < stationCount; i++) {
        if (!visited[i] && distanceArr[i] <= min) {
            min = distanceArr[i];
            index = i;
        }
    }

    return index;
}

void dijkstra(int start) {

    for (int i = 0; i < stationCount; i++) {
        distanceArr[i] = INT_MAX;
        timeArr[i] = INT_MAX;
        visited[i] = 0;
        parent[i] = -1;
    }

    distanceArr[start] = 0;
    timeArr[start] = 0;

    for (int i = 0; i < stationCount - 1; i++) {

        int u = minDistance();
        visited[u] = 1;

        Edge* temp = stations[u].head;

        while (temp != NULL) {

            int v = temp->to;

            if (!visited[v] &&
                distanceArr[u] + temp->distance < distanceArr[v]) {

                distanceArr[v] = distanceArr[u] + temp->distance;
                timeArr[v] = timeArr[u] + temp->time;
                parent[v] = u;
            }

            temp = temp->next;
        }
    }
}

void printPath(int node) {

    if (parent[node] == -1) {
        printf("%s", stations[node].name);
        return;
    }

    printPath(parent[node]);
    printf(" -> %s", stations[node].name);
}

void findRoute(char startName[], char endName[]) {

    int start = getStationIndex(startName);
    int end = getStationIndex(endName);

    if (start == -1 || end == -1) {
        printf("Station not found.\n");
        return;
    }

    dijkstra(start);

    printf("\nRoute:\n\n");
    printPath(end);

    printf("\n\nTotal Distance : %d km\n", distanceArr[end]);
    printf("Estimated Time : %d minutes\n\n", timeArr[end]);
}

void loadSampleNetwork() {

    addStation("Ironwood", 1);
    addStation("MapleCross", 1);
    addStation("Stonebridge", 1);
    addStation("SilverOak", 1);

    addStation("Crestmont", 2);
    addStation("Hillford", 2);
    addStation("UnionPoint", 2);

    int a = getStationIndex("Ironwood");
    int b = getStationIndex("MapleCross");
    int c = getStationIndex("Stonebridge");
    int d = getStationIndex("SilverOak");

    int e = getStationIndex("Crestmont");
    int f = getStationIndex("Hillford");
    int g = getStationIndex("UnionPoint");

    addConnection(a,b,5,7);
    addConnection(b,c,4,6);
    addConnection(c,d,6,8);

    addConnection(e,f,3,5);
    addConnection(f,g,4,6);

    addConnection(c,f,7,10);
}

void menu() {

    int choice;
    char start[MAX_NAME];
    char end[MAX_NAME];

    while (1) {

        printf("\n=== RailX System ===\n");
        printf("1. View Stations\n");
        printf("2. Find Route\n");
        printf("3. Exit\n");
        printf("Choice: ");
        scanf("%d",&choice);

        if (choice == 1) {
            printStations();
        }

        else if (choice == 2) {

            printf("Start Station: ");
            scanf("%s",start);

            printf("Destination Station: ");
            scanf("%s",end);

            findRoute(start,end);
        }

        else if (choice == 3) {
            printf("Exiting RailX\n");
            break;
        }

        else {
            printf("Invalid option\n");
        }
    }
}

int main() {

    loadSampleNetwork();
    menu();

    return 0;
}
