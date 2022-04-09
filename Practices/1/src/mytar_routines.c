#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
	int total = 0, letra = 0;
	if (origin == NULL || destination == NULL) return -1;

	//Mientras que se lea un caracter. Hay que checkear que a la hora de putc y getc no hay ningun error
	while (letra != EOF && total < nBytes) {
		letra = getc(origin);
		putc((char) letra, destination);
		total++;
	}

	return total;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
	int letra = (int) 'a';
	int cont = 0;

	while (letra != '\0' && letra != EOF) {
		letra = fgetc(file);
		cont++;
	}

	if (letra == EOF) return NULL;

	char *l = malloc(cont);
	fseek(file, -cont, SEEK_CUR);

	for (int i = 0; i < cont; i++)
		l[i] = fgetc(file);

	return l;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	stHeaderEntry* array = NULL;
	/* Read the number of files (N) and store it in nr_files*/
	fread(nFiles, sizeof(int), 1, tarFile);

	/* Allocate memory for the array */
	array = malloc(sizeof(stHeaderEntry) * (*nFiles));

	/* Read the (pathname, size) pairs from tarFile and store them in the array */
	for (int i = 0; i < *nFiles; i++) {
		array[i].name = loadstr(tarFile);
		array[i].size = getc(tarFile);
	}

	for (int i = 0; i < *nFiles; i++) {
		printf("%s\n", array[i].name);
		printf("%d\n", array[i].size);
	}

	return array;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	//1. Se crea un archivo llamado test.mtar en disco y se abre con el flag de escritura
	FILE* tarFile;

	tarFile = fopen(tarName, "w");
	if (tarFile == NULL)
		return EXIT_FAILURE;
	
	//2. Se reserva en memoria un array de tipo stHeaderEntry. El array tendrá tantos elementos como ficheros
	stHeaderEntry* header = malloc(sizeof(stHeaderEntry) * nFiles);
	
	//3. Se posiciona en el byte del fichero donde comienza la región de datos 
	int offData = sizeof(int) + nFiles * sizeof(unsigned int);
	for (int i = 0; i < nFiles; i++) 
		offData += strlen(fileNames[i] + 1);
	fseek(tarFile, offData, SEEK_SET);

	FILE* origin;

	for (int i = 0; i < nFiles; i++) {
		origin = fopen(fileNames[i], "r");

		header[i].name = malloc(sizeof(fileNames[i]) + 1);
		strcpy(header[i].name, fileNames[i]);

		header[i].size = copynFile(origin, tarFile, INT_MAX);

		if (header[i].size == -1) {
			printf("Fichero no encontrado\n");
			return EXIT_FAILURE;
		}

		fclose(origin);
	}

	fseek(tarFile, 0, SEEK_SET);
	fwrite(&nFiles, sizeof(int), 1, tarFile);

	for (int i = 0; i < nFiles; i++) {
		fwrite(header[i].name, strlen(header[i].name), 1, tarFile);
		fwrite("\0", sizeof(char), 1, tarFile);
		fwrite(&header[i].size, sizeof(int), 1, tarFile);
	}

	for (int i = 0; i < nFiles; i++)
		free(header[i].name);

	free(header);
	if (fclose(tarFile) != 0) 
		return EXIT_FAILURE;

	printf("File tar created\n");
	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
	//Se abre el fichero tar
	FILE* tarFile = fopen(tarName, "r");
	if (tarFile == NULL) {
		printf("No existe este fichero\n");
		return EXIT_FAILURE;
	}

	printf("Fichero abierto\n");

	//Se lee la cabecera para saber cuantos ficheros hay en el paquete mtar
	int nFiles = 0;
	stHeaderEntry* header = readHeader(tarFile, &nFiles);
	if (header == NULL) {
		printf("No se ha podido leer el fichero");
		return EXIT_FAILURE;
	}

	//Se abren los respectivos ficheros copiando el contenido de estos
	FILE* destination;
	int copy;
	for (int i = 0; i < nFiles; i++) {
		
		destination = fopen(header[i].name, "w");
		copy = copynFile(tarFile, destination, header[i].size);
		
		if (copy == -1) {
			printf("No se ha podido extraer el fichero tar");
			fclose(tarFile);
			return EXIT_FAILURE;
		}
	}

	// Se libera la memoria
	for (int i = 0; i < nFiles; i++)
		free(header[i].name);
	
	free(header);
	if (fclose(tarFile) != 0)
		return EXIT_FAILURE;
	printf("Ficheros creados\n");
	return EXIT_SUCCESS;
}
