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

	//Mientras que se lea un caracter. Hay que checkear que a la hora de putc y getc no hay ningun error
	while (total < nBytes) {
		letra = getc(origin);
		if (letra == EOF)
			break;
		putc(letra, destination);
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
	fopen(file, "r");
	char leido;
	int i = 0;
	leido = getc(file);

	//Se ve cuanto ocupa el nombre del fichero 
	while (leido != '\0') {
		i++;
		leido = getc(file);
		printf("%c", leido);
	}
	i++;
	//Se vuelve a la direccion de memoria donde comenzaba el nombre del fichero
	fseek(file, -i, SEEK_CUR);

	//Se reserva en memoria un char* que ocupa el tamaño del nombre del fichero * el tamaño del char
	char* name = malloc(i * sizeof(char));
	for (int j = 0; j < i; i++)
		name[i] = getc(file);

	return name;
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
	int nr_files = *nFiles;
	/* Read the number of files (N) and store it in nr_files*/

	/* Allocate memory for the array */
	array = malloc(sizeof(stHeaderEntry) * nr_files);

	/* Read the (pathname, size) pairs from tarFile and store them in the array */
	for (int i = 0; i < nr_files; i++) {
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
	stHeaderEntry* header = malloc(sizeof(unsigned int) * nFiles);
	
	//3. Se posiciona en el byte del fichero donde comienza la región de datos 
	int offData = sizeof(int) + nFiles * sizeof(unsigned int);
	for (int i = 0; i < nFiles; i++) 
		offData += strlen(fileNames[i] + 1);
	fseek(tarFile, offData, SEEK_SET);

	FILE* origin;

	for (int i = 0; i < nFiles; i++) {
		origin = fopen(fileNames[i], "r");

		header[i].name = fileNames[i];
		header[i].size = copynFile(origin, tarFile, INT_MAX);

		if (fclose(origin) != 0)
			return EXIT_FAILURE;
	}

	fseek(tarFile, 0, SEEK_SET);
	fwrite(&nFiles, sizeof(int), 1, tarFile);

	for (int i = 0; i < nFiles; i++) {
		fwrite(header[i].name, strlen(header[i].name) + 1, 1, tarFile);
		fwrite(header[i].size, sizeof(int), 1, tarFile);
	}

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
	printf("Fichero abierto\n");

	//Se lee la cabecera para saber cuantos ficheros hay en el paquete mtar
	int nFiles = getc(tarFile);
	stHeaderEntry* header = readHeader(tarFile, &nFiles);

	//Se abren los respectivos ficheros copiando el contenido de estos
	FILE* destination;
	for (int i = 0; i < nFiles; i++) {
		
		destination = fopen(header[i].name, "w");
		copynFile(tarFile, destination, header[i].size);
		
		if (fclose(destination) != 0)
			return EXIT_FAILURE;
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
