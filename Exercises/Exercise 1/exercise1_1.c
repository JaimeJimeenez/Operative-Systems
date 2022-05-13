#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

//This solution is implemented with shared memory and sempahores

#define PROD 10
#define SEM_PAR 0
#define SEM_IMPAR 1
#define SEM_CONS 2

//Generador de pares
void Pares(int* data, sem_t* sems) {
    int i;
    
    for (i = 2; i <= PROD; i += 2) {
        sem_wait(sems + SEM_PAR);
        *data = i;
        sem_post(sems + SEM_CONS);
    } 
}

//Generador de impares
void Impares(int* data, sem_t* sems) {
    int i;

    for (i = 1; i <= PROD; i+= 2) {
        sem_wait(sems + SEM_IMPAR);
        *data = i;
        sem_post(sems + SEM_CONS);
    }
    
    exit(0);
}

//Consumidor
void Consumidor(int* data, sem_t* sems) {
    int i;

    for (i = 0; i < PROD; i++) {
        sem_wait(sems + SEM_CONS);
        printf("Dato: %d\n", *data);
        if (*data % 2 == 0)
            sem_post(sems + SEM_PAR);
        else 
            sem_post(sems + SEM_IMPAR);
    }
}

int main(void) {
    void * sh_mem;
    sem_t * p_sems;
    int* p_data;

    sh_mem = mmap(NULL, 3 * sizeof(sem_t) + sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);

    p_sems = sh_mem;
    p_data = sh_mem + 3 * sizeof(sem_t);

    //Se da el turno al impar
    sem_init(p_sems + SEM_PAR, 1, 0);
    sem_init(p_sems + SEM_IMPAR, 1, 1);
    sem_init(p_sems + SEM_CONS, 1, 0);

    if (!fork())
        Pares(p_data, p_sems);
    else if (!fork())
        Impares(p_data, p_sems);
    else {
        Consumidor(p_data, p_sems);
        sem_destroy(p_sems + SEM_PAR);
        sem_destroy(p_sems + SEM_IMPAR);
        sem_destroy(p_sems + SEM_CONS);
        munmap(sh_mem, 4 * sizeof(sem_t) + sizeof(int));
    }
    exit(0);
}
