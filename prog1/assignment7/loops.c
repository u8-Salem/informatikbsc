/*
Compile: make loops
Run: ./loops
make loops && ./loops
*/

#include "base.h"

void loops_a(int n) {
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= i; j++) {
            printf("%d ", j);
        }
        printf("\n");
    }
}

void loops_b(int n) {
    int j;
    for (int i = 1; i <= n; i++) {
        for (j = 1; j <= n - i; j++) {
            printf("  ");
        }
        for (j = 1; j <= i; j++) {
            printf("%d ", j);
        }
        printf("\n");
    }
}

void loops_c(int n) {
    int j;
    for (int i = 1; i <= n; i++) {
        for (j = 1; j <= n - i; j++) {
            printf(" ");
        } 
        for (j = 1; j <= i; j++) {
            printf("%d ", j);
        }
        printf("\n");
    }
}

void loops_d(int n) {
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= n; j++) {
            if (j == i || j == (n - i)) {
                printf("%d ", i);
            } else {
                printf("  ");
            }
        }
        printf("\n");
    }
}

// todo: implement helper functions for loops_e (recommended)

void loops_e(int n) {
    if (n < 3) { return; }

    int j, k;
    for (int i = 1; i <= n; i++) {
        for (j = 1; j <= n - i + 1; j++) {
            printf(" "); 
        }
        if (i == 1 || i == n) {
            printf("+");
            for (k = 1; k <= n; k++) {
                printf("--");
            }
            printf("+");
        } else {
            printf("/");
            for (k = 1; k <= n; k++) {
                printf("  ");
            }
            printf("/");
        }
        printf("\n");

    }
}

// todo: implement helper functions for loops_f (recommended)

void loops_f(int n) {
    if (n < 3) { return; }

    int j, k, count = 0;
    for (int i = 1; i <= n; i++) {
        for (j = 1; j <= n - i + 1; j++) {
            printf(" "); 
        }
        if (i == 1 || i == n) {
            printf("+");
            for (k = 1; k <= n; k++) {
                printf("--");
            }
            printf("+");
        } else {
            printf("/");
            for (k = 1; k <= n; k++) {
                printf("%d ", count);
                count >= 9 ? count = 0 : count++;
            }
            printf("/");
        }
        printf("\n");

    }
}

int main(void) {
    loops_a(5);
    loops_b(5);
    loops_c(5);
    loops_d(5);
    loops_a(9);
    loops_b(9);
    loops_c(9);
    loops_d(9);
    loops_e(3);
    loops_e(4);
    loops_e(5);
    loops_e(9);
    loops_e(12);
    loops_f(5);
    loops_f(9);
    return 0;
}
