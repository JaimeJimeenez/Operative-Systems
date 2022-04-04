#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define N_PARADAS 5
#define EN_RUTA 0
#define EN_PARADA 1
#define MAX_USUARIOS 40
#define USUARIOS 4

int estado = EN_RUTA;
int parada_actual = 0;
int n_ocupantes = 0;

int esperando_parada[N_PARADAS];

int esperando_bajar[N_PARADAS];

pthread_mutex_t mutex;

pthread_cond_t subir, bajar, avanzar;


void* thread_autobus(void* args) {
	printf("Creando autobus\n");

	while (1) {

		// Esperar a que los viajeros suban y bajen
		Autobus_En_Parada();

		// Conducir hasta la siguiente parada
		Conducir_Hasta_Siguiente_Parada();
	}

	pthread_exit(0);
}

void * thread_usuario(void* arg) {
	int id_usuario;
	//obtener el id del usuario
	int a, b;
	int j = 0;
	while (j < USUARIOS) {
		a = rand() % N_PARADAS;
		do {
			b = rand() % N_PARADAS;
		} while (a == b);
		Usuario (id_usuario, a, b);
		j++;
	}

	printf("Creando un hijo con id: %d y destino: %d\n", a, b);
	pthread_exit(0);
}

void Usuario(int id_usuario, int origen, int destino) {
	// Esperar a que el autobus este en parada origen para subir
	Subir_Autobus(id_usuario, origen);

	// Bajarse en estacion destino
	Bajar_Autobus(id_usuario, destino);
}

void Autobus_En_Parada() {
	// Ajustar el estado y bloquear al autobus hasta que no haya pasajeros que
	// quieran bajar y/o subir la parada actual. Después se pone en marcha
	pthread_mutex_lock(&mutex);

	while (esperando_parada[parada_actual] > 0) {
		pthread_cond_broadcast(&subir);
		pthread_cond_wait(&avanzar, &mutex);
	}

	while (esperando_bajar[parada_actual] > 0) {
		pthread_cond_broadcast(&bajar);
		pthread_cond_wait(&avanzar, &mutex);
	}

	printf("El autobus se encuentra en la parada: %d \n", parada_actual);
	pthread_mutex_unlock(&mutex);
}

void Conducir_Hasta_Siguiente_Parada() {
	// Establecer un Retado que simule el trayecto y actualizar número de parada
	pthread_mutex_lock(&mutex);
	estado = EN_RUTA;
	printf("El autbús se encuentra en ruta.\n");
	
	sleep(5);
	parada_actual++;

	if (parada_actual > N_PARADAS)
		parada_actual = 0;
	
	estado = EN_PARADA;
	printf("El autobús se encuentra en la parada: %d \n", parada_actual);
	pthread_mutex_unlock(&mutex);
}

void Subir_Autobus(int id_usuario, int origen) {
	/* El usuario indicara que quiere subir en la parada ’origen’, esperara a que
	el autobus se pare en dicha parada y subira. El id_usuario puede utilizarse para
	proporcionar informacion de depuracion */
	pthread_mutex_lock(&mutex);
	esperando_parada[origen]++;

	while (estado != EN_PARADA || parada_actual != origen)
		pthread_cond_wait(&subir, &mutex);

	esperando_parada[origen]--;

	//Era el último que se quería subir
	if (esperando_parada[origen] == 0)
		pthread_cond_signal(&avanzar);
	
	printf("El usuario con id: %d se ha subido en la parada: %d\n", id_usuario, origen);
	pthread_mutex_unlock(&mutex);

}

void Bajar_Autobus(int id_usuario, int destino) {
	// El usuario indicará que quiere bajar en la parada 'destino' esperará a que
	// el autobús se pare en dicha parada y bajara. El id_usuario puede utilizarse para
	// proporcionar información de depuración
	pthread_mutex_lock(&mutex);
	esperando_bajar[destino]++;

	while (estado != EN_PARADA || parada_actual != destino)
		pthread_cond_wait(&bajar, &mutex);
	
	esperando_bajar[destino]--;

	// Era el ultimo que se quería bajar
	if (esperando_bajar[destino] == 0)
		pthread_cond_signal(&avanzar);
	
	printf("El ususario %d se baja en la parada: %d\n", id_usuario, destino);

	pthread_mutex_unlock(&mutex);
}

int main(int argc, char* argv[])
{
	int i;
	//Definicion de variables locales a main
	// Opcional: obtener de los argumentos del programa la capacidad del
	// autobús, el número de usuarios y el número de paradas

	//Crear el thread Autobús
	pthread_t autobus;
	pthread_t usuarios[USUARIOS];

	// Mutex
	pthread_mutex_init(&mutex, NULL);

	// Variables de condicion
	pthread_cond_init(&subir, NULL);
	pthread_cond_init(&bajar, NULL);
	pthread_cond_init(&avanzar, NULL);

	for (i = 0; i < USUARIOS; i++) {
		// Crear thread para el usuario i
		pthread_create(&usuarios[i], NULL, thread_usuario, NULL);
		// Esperar terminación de los hilos
	}

	pthread_create(&autobus, NULL, thread_autobus, NULL);
	pthread_join(autobus, NULL);

	for (i = 0; i < USUARIOS; i++) 
		pthread_join(usuarios[i], NULL);

	//Destruccion de Mutex y VCs
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&subir);
	pthread_cond_destroy(&bajar);
	pthread_cond_destroy(&avanzar);

	return 0; 
}

