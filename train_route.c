#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

char stations[MAX][50];
Node* slowAdj[MAX];
Node* fastAdj[MAX];
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
    if(fp == NULL) return;

    fprintf(fp, "%d\n", stationCount);

    for(int i = 0; i < stationCount; i++)
        fprintf(fp, "%s\n", stations[i]);

    fclose(fp);
}

void loadFromFile() {
    FILE *fp = fopen("graph.txt", "r");
    if(fp == NULL) return;

    fscanf(fp, "%d\n", &stationCount);

    for(int i = 0; i < stationCount; i++) {
        fgets(stations[i], 50, fp);
        stations[i][strcspn(stations[i], "\n")] = 0;
        slowAdj[i] = NULL;
        fastAdj[i] = NULL;
    }

    fclose(fp);
}

/* Station Operations */

int addStation(char name[]) {
    if(strlen(name) == 0) return 0;

    if(findStationIndex(name) != -1) {
        printf("Station already exists.\n");
        return 0;
    }

    strcpy(stations[stationCount], name);
    slowAdj[stationCount] = NULL;
    fastAdj[stationCount] = NULL;
    stationCount++;

    saveToFile();
    return 1;
}

/* Graph */

void addEdge(Node* adj[], char src[], char dest[]) {
    int s = findStationIndex(src);
    int d = findStationIndex(dest);

    if(s == -1 || d == -1) return;

    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = d;
    newNode->next = adj[s];
    adj[s] = newNode;

    newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = s;
    newNode->next = adj[d];
    adj[d] = newNode;
}

/* BFS */

void BFS(Node* adj[], int start, int end) {
    int visited[MAX] = {0}, parent[MAX];
    int queue[MAX], front = 0, rear = 0;

    queue[rear++] = start;
    visited[start] = 1;
    parent[start] = -1;

    while(front < rear) {
        int current = queue[front++];

        if(current == end) break;

        Node* temp = adj[current];
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
    for(int v = end; v != -1; v = parent[v])
        path[count++] = v;

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
    int type;

    printf("Enter source: ");
    inputString(src);

    printf("Enter destination: ");
    inputString(dest);

    int s = findStationIndex(src);
    int d = findStationIndex(dest);

    if(s == -1 || d == -1) {
        printf("Invalid station.\n");
        return;
    }

    printf("Select Route Type:\n");
    printf("1. Slow Train\n");
    printf("2. Fast Train\n");
    printf("Choice: ");
    scanf("%d", &type);
    getchar();

    if(type == 1)
        BFS(slowAdj, s, d);
    else if(type == 2)
        BFS(fastAdj, s, d);
    else
        printf("Invalid choice.\n");
}

void showStations() {
    printf("\nStations:\n");
    for(int i = 0; i < stationCount; i++)
        printf("%d. %s\n", i + 1, stations[i]);
}

/* Default Data */

void loadDefaultStations() {
    char* list[] = {
        "Khopoli","Lowjee","Dolavali","Kelavali","Palasdhari","Karjat",
        "Vangani","Shelu","Neral","Bhivpuri Road","Badlapur","Ambarnath",
        "Ulhasnagar","Vithalvadi","Kalyan","Thakurli","Dombivali","Kopar",
        "Diva","Mumbra","Kalwa","Thane"
    };

    int n = 22;
    for(int i = 0; i < n; i++)
        addStation(list[i]);
}

void buildSlowRoute() {
    addEdge(slowAdj,"Khopoli","Lowjee");
    addEdge(slowAdj,"Lowjee","Dolavali");
    addEdge(slowAdj,"Dolavali","Kelavali");
    addEdge(slowAdj,"Kelavali","Palasdhari");
    addEdge(slowAdj,"Palasdhari","Karjat");
    addEdge(slowAdj,"Karjat","Vangani");
    addEdge(slowAdj,"Vangani","Shelu");
    addEdge(slowAdj,"Shelu","Neral");
    addEdge(slowAdj,"Neral","Bhivpuri Road");
    addEdge(slowAdj,"Bhivpuri Road","Badlapur");
    addEdge(slowAdj,"Badlapur","Ambarnath");
    addEdge(slowAdj,"Ambarnath","Ulhasnagar");
    addEdge(slowAdj,"Ulhasnagar","Vithalvadi");
    addEdge(slowAdj,"Vithalvadi","Kalyan");
    addEdge(slowAdj,"Kalyan","Thakurli");
    addEdge(slowAdj,"Thakurli","Dombivali");
    addEdge(slowAdj,"Dombivali","Kopar");
    addEdge(slowAdj,"Kopar","Diva");
    addEdge(slowAdj,"Diva","Mumbra");
    addEdge(slowAdj,"Mumbra","Kalwa");
    addEdge(slowAdj,"Kalwa","Thane");
}

void buildFastRoute() {
    addEdge(fastAdj,"Khopoli","Karjat");
    addEdge(fastAdj,"Karjat","Badlapur");
    addEdge(fastAdj,"Badlapur","Ambarnath");
    addEdge(fastAdj,"Ambarnath","Ulhasnagar");
    addEdge(fastAdj,"Ulhasnagar","Kalyan");
    addEdge(fastAdj,"Kalyan","Dombivali");
    addEdge(fastAdj,"Dombivali","Diva");
    addEdge(fastAdj,"Diva","Thane");
}

/* Main */

int main() {
    int choice;

    loadFromFile();

    if(stationCount == 0) {
        loadDefaultStations();
        buildSlowRoute();
        buildFastRoute();
    } else {
        buildSlowRoute();
        buildFastRoute();
    }

    while(1) {
        printf("\n1. Show Stations\n");
        printf("2. Find Route\n");
        printf("3. Add Station\n");
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

            case 3: {
                char name[50];
                printf("Enter station name: ");
                inputString(name);
                addStation(name);
                break;
            }

            case 0:
                saveToFile();
                exit(0);

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}
