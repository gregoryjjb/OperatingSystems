#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// The maximum time (in seconds) to sleep
#define MAX_SLEEP_TIME 3
// Number of students
#define NUM_OF_STUDENTS 5
// Number of TAs
#define NUM_OF_TAS 2
// Number of helps each student must receive before terminating
#define NUM_OF_HELPS 2
// Number of available seats
#define NUM_OF_SEATS 2

// Semaphores and mutex lock
pthread_mutex_t mutex_lock;
// The number of waiting students
int waiting_students;
// Semaphore declarations
sem_t students_sem; // wait: ta waits for a student to show up, signal: student notifies ta his/her arrival
sem_t ta_sem; // wait: student waits for ta to help, signal: ta notifies student he/she is ready to help

int rand_time(int id)
{
    int seed = time(NULL) / (id + 1);
    return rand_r(&seed) % MAX_SLEEP_TIME + 1;
}

void* student(void* idRef)
{
    int id = *(int*)idRef;
    int numHelps = 0;
    
    while(numHelps < NUM_OF_HELPS) {
        // Do some programming
        int sleepTime = rand_time(id);
        printf("Student[%d, %d] programs for %d seconds\n", id, numHelps, sleepTime);
        fflush(NULL);
        
        sleep(sleepTime);
        
        // Finish programming
        // Check if there are open seats
        pthread_mutex_lock(&mutex_lock);
        
        // If there's an open seat
        if(waiting_students < NUM_OF_SEATS) {
            // Fill a seat
            waiting_students += 1;
            printf("Student[%d, %d] takes a seat, %d students waiting\n", id, numHelps, waiting_students);
            fflush(NULL);
            pthread_mutex_unlock(&mutex_lock);
            
            // Notify that a student has arrived
            sem_post(&students_sem);
            
            // Wait for a TA to be available
            sem_wait(&ta_sem);
            
            // Leave seat
            pthread_mutex_lock(&mutex_lock);
            
            waiting_students -= 1;
            numHelps += 1;
            printf("Student[%d, %d] got help; %d students waiting\n", id, numHelps, waiting_students);
            fflush(NULL);
            
            pthread_mutex_unlock(&mutex_lock);
        }
        // If all seats are full
        else {
            pthread_mutex_unlock(&mutex_lock);
            printf("Student[%d, %d] will try again later\n", id, numHelps);
            fflush(NULL);
            // Go back to programming
        }
    }
    
    // Max helps reached
    return NULL;
}

void* ta(void* idRef)
{
    int id = *(int*)idRef;
    
    int numHelps = 0;
    
    while(1) {
        // Nap, wait for students
        sem_wait(&students_sem);
        
        // Notify student you are awake
        sem_post(&ta_sem);
        
        int sleepTime = rand_time(id);
        
        printf("TA[%d, %d] helps a student for %d seconds\n", id, numHelps, sleepTime);
        fflush(NULL);
        
        sleep(sleepTime);
    }
    
    
    return NULL;
}

int main()
{
    printf("CS149 SleepingTA from Greg Brisebois\n");
    fflush(NULL);
    
    pthread_t ta_threads[NUM_OF_TAS];
    pthread_t student_threads[NUM_OF_STUDENTS];
    
    sem_init(&ta_sem, 0, 0);
    sem_init(&students_sem, 0, 0);
    pthread_mutex_init(&mutex_lock, NULL);
    
    int student_ids[NUM_OF_STUDENTS];
    int ta_ids[NUM_OF_TAS];
    
    // Create IDs for students
    for(int i = 0; i < NUM_OF_STUDENTS; i++) {
        student_ids[i] = i;
    }
    
    // Create IDs for TAs
    for(int i = 0; i < NUM_OF_TAS; i++) {
        ta_ids[i] = i;
    }
    
    // Create TAs
    for(int i = 0; i < NUM_OF_TAS; i++) {
        pthread_create(&ta_threads[i], NULL, ta, (void*) &ta_ids[i]);
    }
    
    // Create students
    for(int i = 0; i < NUM_OF_STUDENTS; i++) {
        pthread_create(&student_threads[i], NULL, student, (void*) &student_ids[i]);
    }
    
    // Wait for students
    for(int i = 0; i < NUM_OF_STUDENTS; i++) {
        pthread_join(student_threads[i], NULL);
    }
    
    // Cancel TAs
    for(int i = 0; i < NUM_OF_TAS; i++) {
        pthread_cancel(ta_threads[i]);
    }
    
    // Clean up
    sem_destroy(&ta_sem);
    sem_destroy(&students_sem);
    pthread_mutex_destroy(&mutex_lock);
    
    printf("All students helped %d times\n", NUM_OF_HELPS);
    fflush(NULL);
}