#include <stdio.h>
#include <stdlib.h>

int main() {

    int* buffer;

    int k = 420;

    buffer = (int*) malloc(sizeof k);

    for(int i = 0; i < 100; i++) {
        buffer[i] = i;
    }

    for(int j = 0; j < 100; j++) {
        int c = buffer[j];

        printf("%d, ", c);
    }

    return 0;
}