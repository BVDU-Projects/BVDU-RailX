#include "railx.h"

/* Global Storage Initialization */
Booking pnrDatabase[MAX_BOOKINGS];
int bookingCount = 0;
int pnrCounter = 1001; // Generates PNRs like RX-1001, RX-1002

/* Display live 20-seat matrix for a train */
void displaySeatMap(int trainIdx) {
    Train *t = &trainSchedules[trainIdx];

    printf("\n=================================================================\n");
    printf(" 💺 LIVE SEAT MATRIX FOR TRAIN: [%s] %s\n", t->trainNo, t->trainName);
    printf(" Capacity: %d Total | Available: %d | Booked: %d\n", 
           t->totalSeats, t->availableSeats, t->totalSeats - t->availableSeats);
    printf("=================================================================\n");

    for (int i = 0; i < SEATS_PER_TRAIN; i++) {
        if (t->seatMap[i] == 0) {
            printf(" [Seat %02d: 🟢 OPEN ] ", i + 1);
        } else {
            printf(" [Seat %02d: ❌ BOOKED] ", i + 1);
        }

        if ((i + 1) % 2 == 0) printf("   "); // Aisle space
        if ((i + 1) % 4 == 0) printf("\n");  // Row break
    }
    printf("=================================================================\n");
}

/* Perform direct booking given a train number string */
void bookTicketDirect(const char targetTrainNo[]) {
    int trainIdx = -1;
    for (int i = 0; i < trainCount; i++) {
        if (strcmp(trainSchedules[i].trainNo, targetTrainNo) == 0) {
            trainIdx = i;
            break;
        }
    }

    if (trainIdx == -1) {
        printf("\n❌ Error: Train '%s' not found in system timetable.\n", targetTrainNo);
        return;
    }

    Train *t = &trainSchedules[trainIdx];

    // Show live inventory map
    displaySeatMap(trainIdx);

    if (t->availableSeats == 0) {
        printf("\n❌ REGRET: Train %s is FULLY BOOKED! No seats available.\n", t->trainNo);
        return;
    }

    char passengerName[MAX_NAME];
    printf("\nEnter Passenger Full Name: ");
    inputString(passengerName, MAX_NAME);

    if (strlen(passengerName) == 0) {
        printf("\n❌ Error: Passenger name cannot be empty.\n");
        return;
    }

    int requestedSeat;
    printf("Select Preferred Open Seat (1 - %d): ", SEATS_PER_TRAIN);
    if (scanf("%d", &requestedSeat) != 1) {
        getchar();
        printf("\n❌ Error: Invalid seat selection.\n");
        return;
    }
    getchar(); // Clear buffer

    if (requestedSeat < 1 || requestedSeat > SEATS_PER_TRAIN) {
        printf("\n❌ Error: Invalid seat selection range.\n");
        return;
    }

    int seatIdx = requestedSeat - 1;

    if (t->seatMap[seatIdx] == 1) {
        printf("\n❌ ERROR: Seat #%d is ALREADY BOOKED by another passenger! Please select an open seat.\n", requestedSeat);
        return;
    }

    // Process Booking
    t->seatMap[seatIdx] = 1; // Mark seat occupied
    t->availableSeats--;     // Decrement capacity

    // Calculate Fare using route engine
    dijkstra(t->originIndex, t->serviceType);
    int fare = calculateFare(distanceArr[t->destIndex], timeArr[t->destIndex], t->serviceType);

    // Record PNR entry
    Booking *b = &pnrDatabase[bookingCount];
    sprintf(b->pnr, "RX-%d", pnrCounter++);
    strcpy(b->passengerName, passengerName);
    strcpy(b->trainNo, t->trainNo);
    b->seatNumber = requestedSeat;
    b->startStationIdx = t->originIndex;
    b->endStationIdx = t->destIndex;
    b->fare = fare;
    b->active = 1;

    bookingCount++;

    // Print Receipt
    printf("\n=================================================================\n");
    printf("              🎉 TICKET BOOKED SUCCESSFULLY!                     \n");
    printf("=================================================================\n");
    printf(" PNR NUMBER      : %s\n", b->pnr);
    printf(" PASSENGER NAME  : %s\n", b->passengerName);
    printf(" TRAIN           : [%s] %s\n", t->trainNo, t->trainName);
    printf(" ASSIGNED SEAT   : Seat #%d (Confirmed)\n", b->seatNumber);
    printf(" FROM            : %s\n", stations[t->originIndex].name);
    printf(" TO              : %s\n", stations[t->destIndex].name);
    printf(" TOTAL FARE PAID : $%d / ₹%d\n", fare, fare);
    printf(" REMAINING SEATS : %d / %d seats left\n", t->availableSeats, t->totalSeats);
    printf("=================================================================\n");
}

/* Manual booking flow */
void bookTicket() {
    char trainNo[10];
    printf("\n--- 🎟️ BOOK TRAIN TICKET ---\n");
    printf("Enter Train Number (e.g., B-101, BF-201, W-301, EX-901): ");
    inputString(trainNo, 10);

    bookTicketDirect(trainNo);
}

/* Lookup PNR details */
void checkPNRStatus() {
    char pnr[12];
    printf("\nEnter PNR Number (e.g., RX-1001): ");
    inputString(pnr, 12);

    int found = -1;
    for (int i = 0; i < bookingCount; i++) {
        if (strcmp(pnrDatabase[i].pnr, pnr) == 0) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("\n❌ Error: PNR '%s' not found in system records.\n", pnr);
        return;
    }

    Booking b = pnrDatabase[found];
    printf("\n=================================================================\n");
    printf("                   PNR RESERVATION STATUS                        \n");
    printf("=================================================================\n");
    printf(" PNR NUMBER      : %s\n", b.pnr);
    printf(" PASSENGER NAME  : %s\n", b.passengerName);
    printf(" TRAIN NO        : %s\n", b.trainNo);
    printf(" SEAT ALLOCATED  : Seat #%d\n", b.seatNumber);
    printf(" ROUTE           : %s -> %s\n", stations[b.startStationIdx].name, stations[b.endStationIdx].name);
    printf(" TOTAL FARE      : $%d / ₹%d\n", b.fare, b.fare);
    printf(" STATUS          : %s\n", b.active ? "🟢 CONFIRMED" : "🔴 CANCELLED");
    printf("=================================================================\n");
}

/* Cancel ticket and release seat */
void cancelTicket() {
    char pnr[12];
    printf("\nEnter PNR Number to Cancel: ");
    inputString(pnr, 12);

    int found = -1;
    for (int i = 0; i < bookingCount; i++) {
        if (strcmp(pnrDatabase[i].pnr, pnr) == 0 && pnrDatabase[i].active == 1) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("\n❌ Error: Active PNR '%s' not found or already cancelled.\n", pnr);
        return;
    }

    pnrDatabase[found].active = 0; // Cancel booking record

    // Locate train and free seat
    for (int i = 0; i < trainCount; i++) {
        if (strcmp(trainSchedules[i].trainNo, pnrDatabase[found].trainNo) == 0) {
            int seatIdx = pnrDatabase[found].seatNumber - 1;
            trainSchedules[i].seatMap[seatIdx] = 0; // Mark seat open
            trainSchedules[i].availableSeats++;     // Increment capacity

            printf("\n✅ Success: PNR %s cancelled successfully.\n", pnr);
            printf("   Seat #%d on Train %s is now OPEN again for other passengers!\n", 
                   pnrDatabase[found].seatNumber, trainSchedules[i].trainNo);
            return;
        }
    }
}
