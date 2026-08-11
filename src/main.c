#include "railx.h"

int main() {
    // Initialize graph network and timetable schedules on launch
    seedMasterNetworkAndTimetable();

    int choice;
    while (1) {
        printf("\n=========================================\n");
        printf("  🚆 BVDU-RailX Integrated Rail System  \n");
        printf("=========================================\n");
        printf("1. View System Stations\n");
        printf("2. Search Train Routes & Fare\n");
        printf("3. View Master Train Timetable (Direct Booking)\n");
        printf("4. Inspect Train Itinerary (Direct Booking)\n");
        printf("5. Book Ticket Manually\n");
        printf("6. Check PNR Ticket Status\n");
        printf("7. Cancel Ticket (Release Seat)\n");
        printf("0. Exit RailX\n");
        printf("=========================================\n");
        printf("Select Option: ");

        if (scanf("%d", &choice) != 1) {
            break;
        }
        getchar(); // Clear newline buffer

        switch (choice) {
            case 1:
                printStations();
                break;

            case 2: {
                char src[MAX_NAME], dest[MAX_NAME];
                printf("\nEnter Departure Station: ");
                inputString(src, MAX_NAME);
                printf("Enter Destination Station: ");
                inputString(dest, MAX_NAME);

                int u = getStationIndex(src);
                int v = getStationIndex(dest);

                if (u == -1 || v == -1) {
                    printf("\n❌ Error: Invalid station name entered. Please check spelling.\n");
                    break;
                }

                dijkstra(u, SERVICE_BOTH);

                if (timeArr[v] == INT_MAX) {
                    printf("\n❌ No valid rail route found between %s and %s.\n", src, dest);
                } else {
                    int fare = calculateFare(distanceArr[v], timeArr[v], SERVICE_BOTH);
                    printf("\n=========================================================\n");
                    printf("               OPTIMIZED ROUTE SUMMARY                   \n");
                    printf("=========================================================\n");
                    printf(" From        : %s\n", stations[u].name);
                    printf(" To          : %s\n", stations[v].name);
                    printf(" Total Dist  : %d km\n", distanceArr[v]);
                    printf(" Total Time  : ");
                    printFormattedDuration(timeArr[v]);
                    printf("\n Est. Fare   : $%d / ₹%d\n", fare, fare);
                    printf("=========================================================\n");
                }
                break;
            }

            case 3:
                viewAllSchedules(); // Calls timetable module, which enables direct seat booking
                break;

            case 4: {
                char trainNo[10];
                printf("Enter Train Number (e.g., B-101, BF-201, W-301, EX-901): ");
                inputString(trainNo, 10);

                int found = -1;
                for (int i = 0; i < trainCount; i++) {
                    if (strcmp(trainSchedules[i].trainNo, trainNo) == 0) {
                        found = i;
                        break;
                    }
                }

                if (found != -1) {
                    printTrainDetailedItinerary(found); // Prompts passenger to book directly
                } else {
                    printf("\n❌ Error: Train number '%s' not found.\n", trainNo);
                }
                break;
            }

            case 5:
                bookTicket();
                break;

            case 6:
                checkPNRStatus();
                break;

            case 7:
                cancelTicket();
                break;

            case 0:
                printf("\nExiting BVDU-RailX. Safe Travels!\n");
                exit(0);

            default:
                printf("\n❌ Invalid option selected. Please try again.\n");
        }
    }

    return 0;
}
