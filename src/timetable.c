#include "railx.h"

/* Global Storage Initialization */
Train trainSchedules[MAX_TRAINS];
int trainCount = 0;

/* Add a new train schedule to system inventory */
void addTrainSchedule(const char trainNo[], const char name[], const char origin[], const char dest[], int depMins, int line, int serviceType) {
    if (trainCount >= MAX_TRAINS) return;

    int u = getStationIndex(origin);
    int v = getStationIndex(dest);

    if (u == -1 || v == -1) return;

    strcpy(trainSchedules[trainCount].trainNo, trainNo);
    strcpy(trainSchedules[trainCount].trainName, name);
    trainSchedules[trainCount].originIndex = u;
    trainSchedules[trainCount].destIndex = v;
    trainSchedules[trainCount].departureMins = depMins;
    trainSchedules[trainCount].line = line;
    trainSchedules[trainCount].serviceType = serviceType;
    trainSchedules[trainCount].totalSeats = SEATS_PER_TRAIN;
    trainSchedules[trainCount].availableSeats = SEATS_PER_TRAIN;

    for (int i = 0; i < SEATS_PER_TRAIN; i++) {
        trainSchedules[trainCount].seatMap[i] = 0; // Initialize all seats as OPEN
    }

    trainCount++;
}

/* Display timetable for all scheduled trains */
void viewAllSchedules() {
    printf("\n=======================================================================================\n");
    printf("                               MASTER TRAIN TIMETABLE                                  \n");
    printf("=======================================================================================\n");
    printf(" %-8s | %-28s | %-18s | %-18s | %-10s | %-10s\n", 
           "Train No", "Name", "Origin", "Destination", "Departure", "Seats Left");
    printf("---------------------------------------------------------------------------------------\n");

    char timeBuf[30];
    for (int i = 0; i < trainCount; i++) {
        formatClockTime(trainSchedules[i].departureMins, timeBuf);
        printf(" %-8s | %-28s | %-18s | %-18s | %-10s | %d/%d\n", 
               trainSchedules[i].trainNo,
               trainSchedules[i].trainName,
               stations[trainSchedules[i].originIndex].name,
               stations[trainSchedules[i].destIndex].name,
               timeBuf,
               trainSchedules[i].availableSeats,
               trainSchedules[i].totalSeats);
    }
    printf("=======================================================================================\n");

    // Enable direct ticket booking from schedule view
    char choice[10];
    printf("\n👉 Enter Train Number to Book a Ticket (or press Enter to return to main menu): ");
    inputString(choice, 10);

    if (strlen(choice) > 0) {
        bookTicketDirect(choice); // Cross-call booking module directly
    }
}

/* Print stop-by-stop detailed itinerary for a specific train */
void printTrainDetailedItinerary(int trainIdx) {
    Train t = trainSchedules[trainIdx];
    dijkstra(t.originIndex, t.serviceType);

    int path[MAX_STATIONS], pathLen = 0;
    for (int v = t.destIndex; v != -1; v = parent[v]) {
        path[pathLen++] = v;
    }

    printf("\n========================================================================\n");
    printf(" 🚆 TIMETABLE ITINERARY: [%s] %s\n", t.trainNo, t.trainName);
    printf(" Service: %s | Line: %s | Seats Left: %d/%d\n", 
           (t.serviceType == SERVICE_FAST ? "⚡ FAST EXPRESS" : "🐢 SLOW COMMUTER"),
           (t.line == 1 ? "Black Line" : (t.line == 2 ? "White Line" : "Grey Line")),
           t.availableSeats, t.totalSeats);
    printf("========================================================================\n");
    printf(" %-4s | %-25s | %-15s | %-10s\n", "Stop", "Station Name", "Arrival Time", "Distance");
    printf("------------------------------------------------------------------------\n");

    char timeBuf[30];
    for (int i = pathLen - 1; i >= 0; i--) {
        int stIdx = path[i];
        formatClockTime(t.departureMins + timeArr[stIdx], timeBuf);
        printf(" %-4d | %-25s | %-15s | %d km\n", 
               pathLen - i, stations[stIdx].name, timeBuf, distanceArr[stIdx]);
    }
    printf("========================================================================\n");

    // Offer direct booking prompt
    char bookChoice;
    printf("\n👉 Do you want to book a seat on this train right now? (y/n): ");
    scanf(" %c", &bookChoice);
    getchar(); // Clear buffer

    if (bookChoice == 'y' || bookChoice == 'Y') {
        bookTicketDirect(t.trainNo); // Cross-call booking module
    }
}

/* Seed 3-line railway topology and master train timetable */
void seedMasterNetworkAndTimetable() {
    // 1. BLACK LINE (Line 1 - Residential Corridor: 25 Stations)
    int b[25];
    b[0]  = addStation("Pinegrove West", 1);
    b[1]  = addStation("Oakwood Terrace", 0);
    b[2]  = addStation("Willowbrook Gardens", 0);
    b[3]  = addStation("Cedarwood Village", 0);
    b[4]  = addStation("Amber Hill Heights", 1);
    b[5]  = addStation("Meadowvale Green", 0);
    b[6]  = addStation("Silver Stream", 0);
    b[7]  = addStation("Maplewood Valley", 0);
    b[8]  = addStation("Glenwood Crossing", 1);
    b[9]  = addStation("Rosewood Crest", 0);
    b[10] = addStation("Fairway Park", 0);
    b[11] = addStation("Sun Valley", 0);
    b[12] = addStation("Nexus Central", 1);
    b[13] = addStation("Harmony Heights", 0);
    b[14] = addStation("Brookside Lane", 0);
    b[15] = addStation("Crestview Manor", 0);
    b[16] = addStation("Highland Park", 1);
    b[17] = addStation("Springdale Green", 0);
    b[18] = addStation("Cloverdale Ridge", 0);
    b[19] = addStation("Parkview East", 0);
    b[20] = addStation("Verdant Valley", 1);
    b[21] = addStation("Fernwood Ridge", 0);
    b[22] = addStation("Sunnyvale Meadows", 0);
    b[23] = addStation("Timberland Springs", 0);
    b[24] = addStation("Ironwood Park", 1);

    // Local Tracks
    for (int i = 0; i < 24; i++) {
        addConnection(b[i], b[i+1], 3, 5, 1, SERVICE_SLOW);
    }
    // Express Bypass Tracks
    addConnection(b[0],  b[4],  12, 14, 1, SERVICE_FAST);
    addConnection(b[4],  b[8],  12, 14, 1, SERVICE_FAST);
    addConnection(b[8],  b[12], 12, 14, 1, SERVICE_FAST);
    addConnection(b[12], b[16], 12, 14, 1, SERVICE_FAST);
    addConnection(b[16], b[20], 12, 14, 1, SERVICE_FAST);
    addConnection(b[20], b[24], 12, 14, 1, SERVICE_FAST);

    // 2. WHITE LINE (Line 2 - Commercial Business District: 9 Stations)
    int w[9];
    w[0] = addStation("Vanguard Towers", 1);
    w[1] = addStation("Apex Plaza", 0);
    w[2] = addStation("Silicon Heights", 1);
    w[3] = getStationIndex("Glenwood Crossing"); // Shared Interchange
    w[4] = addStation("Commerce Square", 0);
    w[5] = getStationIndex("Nexus Central");     // Shared Interchange
    w[6] = addStation("Crystal District", 0);
    w[7] = addStation("Trade Gate Plaza", 0);
    w[8] = addStation("Horizon Trade Port", 1);

    // Local Tracks
    for (int i = 0; i < 8; i++) {
        addConnection(w[i], w[i+1], 6, 8, 2, SERVICE_SLOW);
    }
    // Express Bypass Tracks
    addConnection(w[0], w[2], 12, 12, 2, SERVICE_FAST);
    addConnection(w[2], w[3], 6,  6,  2, SERVICE_FAST);
    addConnection(w[3], w[5], 12, 12, 2, SERVICE_FAST);
    addConnection(w[5], w[8], 18, 18, 2, SERVICE_FAST);

    // 3. GREY LINE (Line 3 - Interstate Freight & Express: 16 Stations)
    int o[16];
    o[0]  = addStation("Titan Freight Harbor", 1);
    o[1]  = addStation("Iron Gate Depot", 0);
    o[2]  = addStation("Steel Valley Yard", 1);
    o[3]  = getStationIndex("Ironwood Park");     // Shared Interchange
    o[4]  = addStation("Summit Ridge Outpost", 0);
    o[5]  = addStation("Badland Gap", 0);
    o[6]  = getStationIndex("Nexus Central");     // Shared Interchange
    o[7]  = addStation("Midland Junction", 0);
    o[8]  = addStation("Frontier Fortress", 1);
    o[9]  = addStation("Canyon Creek Crossing", 0);
    o[10] = addStation("Northgate Terminus", 1);
    o[11] = addStation("Frost Valley Reach", 0);
    o[12] = addStation("Vanguard Citadel", 1);
    o[13] = addStation("Outland Basin", 0);
    o[14] = addStation("Borderland Pass", 0);
    o[15] = addStation("Aetheria Far West", 1);

    int outerDistances[15] = {200, 180, 220, 190, 210, 250, 170, 230, 210, 240, 190, 260, 220, 200, 280};
    int outerTimes[15]     = {240, 210, 260, 220, 250, 300, 200, 270, 250, 290, 230, 310, 260, 240, 330};

    for (int i = 0; i < 15; i++) {
        addConnection(o[i], o[i+1], outerDistances[i], outerTimes[i], 3, SERVICE_BOTH);
    }

    // Schedule Master Trains
    addTrainSchedule("B-101",  "Black Line Morning Local",      "Pinegrove West",       "Ironwood Park",      360, 1, SERVICE_SLOW);
    addTrainSchedule("BF-201", "Black Bullet Express",          "Pinegrove West",       "Ironwood Park",      480, 1, SERVICE_FAST);
    addTrainSchedule("W-301",  "Commercial Local Shuttle",     "Vanguard Towers",      "Horizon Trade Port", 510, 2, SERVICE_SLOW);
    addTrainSchedule("WF-401", "Corporate Flyer Fast",          "Vanguard Towers",      "Horizon Trade Port", 540, 2, SERVICE_FAST);
    addTrainSchedule("EX-901", "Titan Interstate Super-Fast",   "Titan Freight Harbor", "Aetheria Far West",    0, 3, SERVICE_FAST);
}
