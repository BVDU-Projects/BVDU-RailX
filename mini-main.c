
#include <stdio.h>
#include <string.h>

#define MAX 20

char slow[][20] = {
    "Thane","Kalwa","Mumbra","Diva","Kopar",
    "Dombivali","Thakurli","Kalyan","Vitthalwadi",
    "Ulhasnagar","Ambernath","Badlapur"
};

char fast[][20] = {
    "Thane","Dombivali","Kalyan","Vitthalwadi",
    "Ulhasnagar","Ambernath","Badlapur"
};

int findIndex(char arr[][20], int size, char key[]) {
    for (int i = 0; i < size; i++) {
        if (strcmp(arr[i], key) == 0)
            return i;
    }
    return -1;
}

void printRoute(char arr[][20], int start, int end) {
    if (start <= end) {
        for (int i = start; i <= end; i++) {
            printf("%s", arr[i]);
            if (i != end) printf(" -> ");
        }
    } else {
        for (int i = start; i >= end; i--) {
            printf("%s", arr[i]);
            if (i != end) printf(" -> ");
        }
    }
    printf("\n");
}

int main() {
    char source[20], dest[20];

    int slowSize = 12;
    int fastSize = 7;

    printf("Enter source station: ");
    scanf("%s", source);

    printf("Enter destination station: ");
    scanf("%s", dest);

    int f1 = findIndex(fast, fastSize, source);
    int f2 = findIndex(fast, fastSize, dest);

    if (f1 != -1 && f2 != -1) {
        printf("\nFast Route:\n");
        printRoute(fast, f1, f2);
    } else {
        printf("\nFast Route: Not Available\n");
    }

    int s1 = findIndex(slow, slowSize, source);
    int s2 = findIndex(slow, slowSize, dest);

    if (s1 != -1 && s2 != -1) {
        printf("\nSlow Route:\n");
        printRoute(slow, s1, s2);
    } else {
        printf("\nSlow Route: Not Available\n");
    }

    return 0;
}
