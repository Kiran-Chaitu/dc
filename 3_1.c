#include <stdio.h>
#include <string.h>
#include <ctype.h>

int n, m = 0, i = 0, j = 0;
char a[10][10], firstSet[10], followSet[10];

void follow(char c);
void first(char c);
int isPresent(char[], char);

int main() {
    int z;
    char c, ch;
    printf("Enter the number of productions: ");
    scanf("%d", &n);
    printf("Enter the productions (epsilon = $):\n");

    for (i = 0; i < n; i++) {
        scanf("%s", a[i]);
    }

    do {
        m = 0;
        printf("Enter the element whose FIRST and FOLLOW is to be found: ");
        scanf(" %c", &c); // notice the space before %c

        // FIRST
        memset(firstSet, 0, sizeof(firstSet));
        m = 0;
        first(c);
        printf("FIRST(%c) = { ", c);
        for (i = 0; i < m; i++) printf("%c ", firstSet[i]);
        printf("}\n");

        // FOLLOW
        memset(followSet, 0, sizeof(followSet));
        m = 0;
        follow(c);
        printf("FOLLOW(%c) = { ", c);
        for (i = 0; i < m; i++) printf("%c ", followSet[i]);
        printf("}\n");

        printf("Do you want to continue? (0/1): ");
        scanf("%d", &z);
    } while (z == 1);

    return 0;
}

void first(char c) {
    int k;

    if (!isupper(c)) {
        if (!isPresent(firstSet, c)) firstSet[m++] = c;
        return;
    }

    for (k = 0; k < n; k++) {
        if (a[k][0] == c) {
            if (a[k][2] == '$') {
                follow(a[k][0]);
            } else if (islower(a[k][2])) {
                if (!isPresent(firstSet, a[k][2])) firstSet[m++] = a[k][2];
            } else {
                first(a[k][2]);
            }
        }
    }
}

void follow(char c) {
    if (a[0][0] == c) {
        if (!isPresent(followSet, '$')) followSet[m++] = '$';
    }

    for (i = 0; i < n; i++) {
        for (j = 2; j < strlen(a[i]); j++) {
            if (a[i][j] == c) {
                if (a[i][j + 1] != '\0') {
                    first(a[i][j + 1]);
                    for (int x = 0; x < m; x++) {
                        if (!isPresent(followSet, firstSet[x])) {
                            followSet[m++] = firstSet[x];
                        }
                    }
                }
                if (a[i][j + 1] == '\0' && c != a[i][0]) {
                    follow(a[i][0]);
                }
            }
        }
    }
}

int isPresent(char arr[], char c) {
    for (int i = 0; i < m; i++) {
        if (arr[i] == c) return 1;
    }
    return 0;
}
