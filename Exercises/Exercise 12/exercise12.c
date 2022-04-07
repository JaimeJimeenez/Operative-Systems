#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

sem_t* mutex; //1
sem_t* puerta; //2
pthread_mutex_t mutex;


int surtidor[2] = {1, 1}; // 1 libre, 0 ocupado

void ServirCombustible(int* surtidor, int dinero) {

}

//Suponiendo que a la hora de liberar el semáforo correspondiente se desbloquea el primer cliente en orden de llegada
void cliente(int dinero) {
    //<declaracion de variables locales>
    int mi_surtidor;
    //Comprobar que se cumplen los requisitos para repostar
    sem_wait(&puerta);
    sem_wait(&mutex);
    mi_surtidor = surtidor[0] == 1 ? 0 : 1;
    surtidor[mi_surtidor] = 0;
    sem_post(&mutex);
    ServirCombustible(surtidor, dinero);
    // Acciones de salida
    sem_wait(&mutex);
    surtidor[mi_surtidor] = 1;
    sem_post(&mutex);
    sem_post(&puerta);
}

int main() {

}

#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define N 10;

sem_t* barbero; //0
sem_t* sala; //1
pthread_mutex_t mutex;


int tamañoBarberia = N;
int ocupadas = 0;
int dormido = 0;

void recibirCorteDePelo() {

}

void cortarPelo() {

}

void* Barbero() {
    while (true) {
        lock(&mutex);
        if (ocupadas == 0) {
            dormido = 
            unlock(&mutex);
            sem_wait(&barbero);
        }
        else unlock(mutex);
        cortarPelo();
    }
}

void* Cliente() {
    lock(&mutex);
    if (tamañoBarberia > ocupadas) {
        ocupadas++;
        if (dormido == 0) {
            dormido = 1;
            sem_post(&barbero);
        }

        unlock(&mutex);
        sem_wait(&sala);
        recirbirCorteDePelo();
        sem_post(&sala);
        lock(&mutex);
        ocupadas--;
        unlock(&mutex);
    }
    else unlock(&mutex);
}


int main() {

}