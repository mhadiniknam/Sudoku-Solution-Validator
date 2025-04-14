#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int Ans = 0 ;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define NUM_THREADS 9

int sudoku[NUM_THREADS][NUM_THREADS] = {
    {5, 3, 4, 6, 7, 8, 9, 1, 2},
    {6, 7, 2, 1, 9, 5, 3, 4, 8},
    {1, 9, 8, 3, 4, 2, 5, 6, 7},
    {8, 5, 9, 7, 6, 1, 4, 2, 3},
    {4, 2, 6, 8, 5, 3, 7, 9, 1},
    {7, 1, 3, 9, 2, 4, 8, 5, 6},
    {9, 6, 1, 5, 3, 7, 2, 8, 4},
    {2, 8, 7, 4, 1, 9, 6, 3, 5},
    {3, 4, 5, 2, 8, 6, 1, 7, 9}
    };

int CheckArray(int *array) {
    int check[9] = {0};
    for (int i = 0; i < 9; i++) {
        if (array[i] != 0) {
            check[array[i] - 1]++;
        }
    }
    for (int i = 0; i < 9; i++) {
        if (check[i] > 1) {
            return 0; 
        }
    }
    return 1; 
}

// Function to be executed by each thread
void *Row(void *threadid) {
    long tid = (long)threadid;
    long row = tid;
    int valid = CheckArray(sudoku[row]);
    if (valid) {
        printf("Row %ld is valid.\n", row);
    } else {
        printf("Row %ld is invalid.\n", row);
    }
    pthread_mutex_lock(&mutex);
    Ans += !(valid);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void *Column(void *threadid) {
    for (int i=0; i < NUM_THREADS; i++) {
        long col = i ;
        int column[9];
        for (int i = 0 ; i < 9; i++) {
            column[i] = sudoku[i][col];
        }
        int valid = CheckArray(column);
        if (valid) {
            printf("Column %ld is valid.\n", col);
        } else {
            printf("Column %ld is invalid.\n", col);
        }
        pthread_mutex_lock(&mutex);
        Ans += !(valid);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void *Box(void *threadid) {
    long tid = (long)threadid;
    long b = tid;
    int box[9];
    for(int i = 0 ; i < 9 ; i++){
        int row = b / 3 * 3 + i / 3;
        int col = b % 3 * 3 + i % 3;
        box[i] = sudoku[row][col]; 
    }
    int valid = CheckArray(box);
    if (valid) {
        printf("Box %ld is valid.\n", b);
    } else {
        printf("Box %ld is invalid.\n", b);
    }
    pthread_mutex_lock(&mutex);
    Ans += !(valid);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t colthreads;
    pthread_t rowthreads;
    pthread_t boxthreads[NUM_THREADS];
    long t;
    int rc_1 = pthread_create(&colthreads, NULL, *Column, (void *)t);
    int rc_2 = pthread_create(&rowthreads, NULL, *Row, (void *)t);
    for(t = 0; t < NUM_THREADS; t++) {
        printf("In main: creating Box thread %ld\n", t);
        int rc_3 = pthread_create(&boxthreads[t], NULL, *Box, (void *)t);
        if (rc_1) {
            printf("ERROR; return code from pthread_create() for Column is %d\n", rc_1);
            printf("ERROR; return code from pthread_create() for Row is %d\n", rc_2);
            printf("ERROR; return code from pthread_create() for Box is %d\n", rc_3);
            exit(-1);
        }
    }
        
    pthread_join(colthreads, NULL);
    pthread_join(rowthreads, NULL);
    // Synching the Box threads
    for(t = 0; t < NUM_THREADS; t++) {
        printf("In main: waiting for thread %ld to complete\n", t);

        pthread_join(boxthreads[t], NULL);
    }
    if (Ans == 0) {
        printf("Sudoku is valid.\n");
    } else {
        printf("Sudoku is invalid.\n");
    }

    printf("Program completed.\n");
    pthread_exit(NULL);
}
