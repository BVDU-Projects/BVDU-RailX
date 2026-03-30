#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

char stations[MAX][50];
Node* adjList[MAX];
int stationCount = 0;

/* Utility */

void inputString(char str[]) {
    fgets(str, 50, stdin);
    str[strcspn(str, "\n")] = 0;
}

int findStationIndex(char name[]) {
    for(int i = 0; i < stationCount; i++) {
        if(strcmp(stations[i], name) == 0)
            return i;
    }
    return -1;
}

/* File Handling */

void saveToFile() {
    FILE *fp = fopen("graph.txt", "w");
    if(fp == NULL) {
        printf("Error saving file.\n");
        return;
    }

    fprintf(fp, "%d\n", stationCount);

    for(int i = 0; i < stationCount; i++) {
        fprintf(fp, "%s\n", stations[i]);
    }

    int edgeCount = 0;
    for(int i = 0; i < stationCount; i++) {
        Node* temp = adjList[i];
        while(temp) {
            if(i < temp->vertex)
                edgeCount++;
            temp = temp->next;
        }
    }

    fprintf(fp, "%d\n", edgeCount);

    for(int i = 0; i < stationCount; i++) {
        Node* temp = adjList[i];
        while(temp) {
            if(i < temp->vertex)
                fprintf(fp, "%d %d\n", i, temp->vertex);
            temp = temp->next;
        }
    }

    fclose(fp);
}

void loadFromFile() {
    FILE *fp = fopen("graph.txt", "r");

    if(fp == NULL) {
        return;
    }

    for(int i = 0; i < stationCount; i++) {
        Node* temp = adjList[i];
        while(temp) {
            Node* t = temp;
            temp = temp->next;
            free(t);
        }
        adjList[i] = NULL;
    }

    stationCount = 0;

    fscanf(fp, "%d\n", &stationCount);

    for(int i = 0; i < stationCount; i++) {
        fgets(stations[i], 50, fp);
        stations[i][strcspn(stations[i], "\n")] = 0;
        adjList[i] = NULL;
    }

    int edgeCount;
    fscanf(fp, "%d", &edgeCount);

    for(int i = 0; i < edgeCount; i++) {
        int s, d;
        fscanf(fp, "%d %d", &s, &d);

        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode->vertex = d;
        newNode->next = adjList[s];
        adjList[s] = newNode;

        newNode = (Node*)malloc(sizeof(Node));
        newNode->vertex = s;
        newNode->next = adjList[d];
        adjList[d] = newNode;
    }

    fclose(fp);
}

/* Station Operations */

// FIX 4: Added autoSave flag to avoid excessive file writes during bulk loading
int addStation(char name[], int autoSave) {
    if(strlen(name) == 0) {
        printf("Invalid input.\n");
        return 0;
    }

    if(findStationIndex(name) != -1) {
        printf("Station already exists.\n");
        return 0;
    }

    strcpy(stations[stationCount], name);
    adjList[stationCount] = NULL;
    stationCount++;

    if(autoSave)
        saveToFile();

    return 1;
}

void deleteStation(char name[]) {
    int index = findStationIndex(name);

    if(index == -1) {
        printf("Station not found.\n");
        return;
    }

    // Free adjacency list of the station being deleted
    Node* temp = adjList[index];
    while(temp) {
        Node* t = temp;
        temp = temp->next;
        free(t);
    }
    adjList[index] = NULL;

    // Remove all edges pointing to the deleted station
    for(int i = 0; i < stationCount; i++) {
        if(i == index) continue;

        Node* curr = adjList[i];
        Node* prev = NULL;

        while(curr) {
            if(curr->vertex == index) {
                if(prev)
                    prev->next = curr->next;
                else
                    adjList[i] = curr->next;

                free(curr);
                break;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    // Shift stations array left
    for(int i = index; i < stationCount - 1; i++) {
        strcpy(stations[i], stations[i + 1]);
        adjList[i] = adjList[i + 1];
    }
    adjList[stationCount - 1] = NULL;

    stationCount--;

    // FIX 2: Update all vertex indices greater than deleted index
    for(int i = 0; i < stationCount; i++) {
        Node* curr = adjList[i];
        while(curr) {
            if(curr->vertex > index)
                curr->vertex--;
            curr = curr->next;
        }
    }

    saveToFile();
    printf("Station deleted.\n");
}

/* Graph */

// FIX 3: Check for duplicate edges before adding
int edgeExists(int s, int d) {
    Node* temp = adjList[s];
    while(temp) {
        if(temp->vertex == d)
            return 1;
        temp = temp->next;
    }
    return 0;
}

void addEdge(char src[], char dest[]) {
    int s = findStationIndex(src);
    int d = findStationIndex(dest);

    if(s == -1 || d == -1) {
        printf("Invalid station.\n");
        return;
    }

    // FIX 3: Prevent duplicate edges
    if(edgeExists(s, d)) {
        printf("Connection already exists.\n");
        return;
    }

    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = d;
    newNode->next = adjList[s];
    adjList[s] = newNode;

    newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = s;
    newNode->next = adjList[d];
    adjList[d] = newNode;

    saveToFile();
}

/* BFS */

void BFS(int start, int end) {
    int visited[MAX] = {0};
    int queue[MAX];

    // FIX 1: Initialize parent array to -1 to avoid garbage values
    int parent[MAX];
    memset(parent, -1, sizeof(parent));

    int front = 0, rear = 0;

    queue[rear++] = start;
    visited[start] = 1;

    while(front < rear) {
        int current = queue[front++];

        if(current == end)
            break;

        Node* temp = adjList[current];

        while(temp) {
            if(!visited[temp->vertex]) {
                visited[temp->vertex] = 1;
                parent[temp->vertex] = current;
                queue[rear++] = temp->vertex;
            }
            temp = temp->next;
        }
    }

    if(!visited[end]) {
        printf("No route found.\n");
        return;
    }

    int path[MAX], count = 0;

    for(int v = end; v != -1; v = parent[v]) {
        path[count++] = v;
    }

    printf("Route:\n");
    for(int i = count - 1; i >= 0; i--) {
        printf("%s", stations[path[i]]);
        if(i != 0) printf(" -> ");
    }
    printf("\n");
}

/* Features */

void findRoute() {
    char src[50], dest[50];

    printf("Enter source: ");
    inputString(src);

    printf("Enter destination: ");
    inputString(dest);

    int s = findStationIndex(src);
    int d = findStationIndex(dest);

    if(s == -1 || d == -1) {
        printf("Invalid station name.\n");
        return;
    }

    if(s == d) {
        printf("%s\n", src);
        return;
    }

    BFS(s, d);
}

void showStations() {
    printf("\nStations:\n");
    for(int i = 0; i < stationCount; i++) {
        printf("%d. %s\n", i + 1, stations[i]);
    }
}

/* Default Data */

// FIX 4: Use autoSave=0 during bulk load, save once at the end
void loadDefaultStations() {
    char* list[] = {
        "Khopoli","Lowjee","Dolavali","Kelavali","Palasdhari","Karjat",
        "Vangani","Shelu","Neral","Bhivpuri Road","Badlapur","Ambarnath",
        "Ulhasnagar","Vithalvadi","Kalyan","Thakurli","Dombivali","Kopar",
        "Diva","Mumbra","Kalwa","Thane"
    };

    int n = 22;

    for(int i = 0; i < n; i++) {
        addStation(list[i], 0);  // no auto-save during bulk load
    }
}

void buildSlowRoute() {
    addEdge("Khopoli","Lowjee");
    addEdge("Lowjee","Dolavali");
    addEdge("Dolavali","Kelavali");
    addEdge("Kelavali","Palasdhari");
    addEdge("Palasdhari","Karjat");
    addEdge("Karjat","Vangani");
    addEdge("Vangani","Shelu");
    addEdge("Shelu","Neral");
    addEdge("Neral","Bhivpuri Road");
    addEdge("Bhivpuri Road","Badlapur");
    addEdge("Badlapur","Ambarnath");
    addEdge("Ambarnath","Ulhasnagar");
    addEdge("Ulhasnagar","Vithalvadi");
    addEdge("Vithalvadi","Kalyan");
    addEdge("Kalyan","Thakurli");
    addEdge("Thakurli","Dombivali");
    addEdge("Dombivali","Kopar");
    addEdge("Kopar","Diva");
    addEdge("Diva","Mumbra");
    addEdge("Mumbra","Kalwa");
    addEdge("Kalwa","Thane");
}

void buildFastRoute() {
    addEdge("Khopoli","Karjat");
    addEdge("Karjat","Badlapur");
    addEdge("Badlapur","Ambarnath");
    addEdge("Ambarnath","Ulhasnagar");
    addEdge("Ulhasnagar","Kalyan");
    addEdge("Kalyan","Dombivali");
    addEdge("Dombivali","Diva");
    addEdge("Diva","Thane");
}

/* Main */

int main() {
    int choice;
    char name1[50], name2[50];

    loadFromFile();

    if(stationCount == 0) {
        loadDefaultStations();
        buildSlowRoute();
        buildFastRoute();
        saveToFile();  // FIX 4: Save once after all default data is loaded
    }

    while(1) {
        printf("\n1. Show Stations\n");
        printf("2. Find Route\n");
        printf("3. Add Station\n");
        printf("4. Delete Station\n");
        printf("5. Add Connection\n");
        printf("0. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        switch(choice) {
            case 1:
                showStations();
                break;

            case 2:
                findRoute();
                break;

            case 3:
                printf("Enter station name: ");
                inputString(name1);
                addStation(name1, 1);  // auto-save enabled for manual add
                break;

            case 4:
                printf("Enter station name: ");
                inputString(name1);
                deleteStation(name1);
                break;

            case 5:
                printf("Enter source: ");
                inputString(name1);
                printf("Enter destination: ");
                inputString(name2);
                addEdge(name1, name2);
                break;

            case 0:
                saveToFile();
                exit(0);

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}
