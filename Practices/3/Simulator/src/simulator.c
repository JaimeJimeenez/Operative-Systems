#include <stdlib.h>
#include <stdio.h>

#define N_PARADAS 5
#define EN_RUTA 0
#define EN_PARADA 1
#define MAX_USUARIOS 40
#define USUARIOS 4

int estado = EN_RUTA;
int parada_actual = 0;
int n_ocupantes = 0;

int espaerando_parada[N_PARADAS];

int esperando_bajar[N_PARADAS];

void* thread_autobus(void* args) {
	while (/* condicion*/) {

		//Esperar a que los viajeros suban y bajen

		Autobus_En_Parada();

		Conducir_Hasta_Siguiente_Parada();
	}
}

void * thread_usuario(void* arg) {
	int id_usuario;
	//obtener el id del usuario
	while (/*condicion*/) {
		a = rand() % N_PARADAS;
		do {
			b = rand() % N_PARADAS;
		} while (a == b);
		Usuario (id_usuario, a, b);
	}
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
}

void Conducir_Hasta_Siguiente_Parada() {
	// Establecer un Retado que simule el trayecto y actualizar número de parada
}

void Bajar_Autobus(int id_usuario, int destino) {
	// El usuario indicará que quiere bajar en la parada 'destino' esperará a que
	// el autobús se pare en dicha parada y bajara. El id_usuario puede utilizarse para
	// proporcionar información de depuración
}

int main(int argc, char* argv[])
{
	int i;
	//Definicion de variables locales a main
	// Opcional: obtener de los argumentos del programa la capacidad del
	// autobús, el número de usuarios y el número de paradas

	//Crear el thread Autobús

	for (i = 0; ...) {
		// Crear thread para el usuario i

		// Esperar terminación de los hilos

		return 0; 
	}

}

