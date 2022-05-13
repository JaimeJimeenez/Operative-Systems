#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

pthread_cond_t oddsCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t evensCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t consumerCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int turne = -1; // -1 odds, even = 0, consumer = 1;
int data = 1;

void* Odds() {
    int i;

    for (i = 1; i < 1000; i += 2) {
        pthread_mutex_lock(&mutex);
        while (turne != -1) 
            pthread_cond_wait(&oddsCond, &mutex);
        data = i;
        printf("Odd Data: %d\n", data);

        turne = 1;
        pthread_cond_signal(&consumerCond);
        pthread_mutex_unlock(&mutex);
    }
}

void* Evens() {
    int i;
    for (i = 2; i < 1000; i += 2) {
        pthread_mutex_lock(&mutex);
        while (turne != 0)
            pthread_cond_wait(&evensCond, &mutex);
        data = i;
        printf("Even Data: %d\n", data);

        turne = 1;
        pthread_cond_signal(&consumerCond);
        pthread_mutex_unlock(&mutex);
    }

}

void* Consumer() {
    int i;

    for (i = 0; i < 1000;i++) {
        pthread_mutex_lock(&mutex);

        if (turne != 1)
            pthread_cond_wait(&consumerCond, &mutex);
        
        printf("Data: %d\n", data);

        if (data%2) {
            turne = 0;
            pthread_cond_signal(&evensCond);
        }
        else {
            turne = -1;
            pthread_cond_signal(&oddsCond);
        }

        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    pthread_t odds, evens, consumer;

    // Mutex 
    pthread_mutex_init(&mutex, NULL);

    // Condition Variables
    pthread_cond_init(&oddsCond, NULL);
    pthread_cond_init(&evensCond, NULL);
    pthread_cond_init(&consumerCond, NULL);

    // Threads
    pthread_create(&odds, NULL, Odds, NULL);
    pthread_create(&evens, NULL, Evens, NULL);
    pthread_create(&consumer, NULL, Consumer, NULL);

    pthread_join(odds, NULL);
    pthread_join(evens, NULL);
    pthread_join(consumer, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&oddsCond);
    pthread_cond_destroy(&evensCond);
    pthread_cond_destroy(&consumerCond);

    return 0;
}