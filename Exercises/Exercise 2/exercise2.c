#include <stdio.h>
#include <pthread.h>

typedef struct {
    int val;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} my_semaphore;

void my_semaphore_init(my_semaphore* s, int val) {
    s->val = val;
    pthread_mutex_init(&(s->mutex), NULL);
    pthread_cond_init(&(s->cond), NULL);
}

void wait(my_semaphore* s) {
    pthread_mutex_lock(&(s->mutex));
    while (s->val == 0)
        pthread_cond_wait(&(s->cond), &(s->mutex));
    s->val--;
    pthread_mutex_unlock(&(s->mutex));
}

void signal(my_semaphore* s) {
    pthread_mutex_lock(&(s->mutex));
    s->val++;
    pthread_cond_broadcast(&(s->cond));
    pthread_mutex_unlock(&(s->mutex));
}

int main() {

}