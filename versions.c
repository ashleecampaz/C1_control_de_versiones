/**
 * @file
 * @brief Implementacion del API de gestion de versiones
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 * @copyright MIT License
*/

#include "versions.h"

/**
 * @brief Crea una version en memoria del archivo
 * Valida si el archivo especificado existe y crea su hash
 * @param filename Nombre del archivo
 * @param hash Hash del contenido del archivo
 * @param comment Comentario
 * @param result Nueva version en memoria
 *
 * @return Resultado de la operacion
 */
return_code create_version(char * filename, char * comment, file_version * result);

/**
 * @brief Verifica si existe una version para un archivo
 *
 * @param filename Nombre del archivo
 * @param hash Hash del contenido
 *
 * @return 1 si la version existe, 0 en caso contrario.
 */
int version_exists(char * filename, char * hash);

/**
 * @brief Obtiene el hash de un archivo.
 * @param filename Nombre del archivo a obtener el hash
 * @param hash Buffer para almacenar el hash (HASH_SIZE)
 * @return Referencia al buffer, NULL si ocurre error
 */
char *get_file_hash(char * filename, char * hash);

/**
 * @brief Copia un archivo
 *
 * @param source Archivo fuente
 * @param destination Destino
 *
 * @return
 */
int copy(char * source, char * destination);

/**
* @brief Almacena un archivo en el repositorio
*
* @param filename Nombre del archivo
* @param hash Hash del archivo: nombre del archivo en el repositorio
*
* @return
*/
int store_file(char * filename, char * hash);

/**
* @brief Almacena un archivo en el repositorio
*
* @param hash Hash del archivo: nombre del archivo en el repositorio
* @param filename Nombre del archivo
*
* @return
*/
int retrieve_file(char * hash, char * filename);

/**
 * @brief Adiciona una nueva version de un archivo.
 *
 * @param filename Nombre del archivo.
 * @param comment Comentario de la version.
 * @param hash Hash del contenido.
 *
 * @return 1 en caso de exito, 0 en caso de error.
 */
int add_new_version(file_version * v);


return_code create_version(char * filename, char * comment, file_version * result) {

	// Llena a estructura result recibida por referencia.
		
	// Debe validar:
		
	// 1. Que el archivo exista y sea un archivo regular
	struct stat s;
	
	int result_s = stat(filename,&s);
	if (result_s<0 || !S_ISREG(s.st_mode)){
		return VERSION_ERROR;
	}
	// 2. Obtiene y guarda en la estructura el HASH del archivo
	char hash;
	
	strcpy(result->filename,filename);
	strcpy(result->hash,get_file_hash(filename,&hash));
	strcpy(result->comment,comment);
	// Llena todos los atributos de la estructura y retorna VERSION_CREATED
	// En caso de fallar alguna validacion, retorna VERSION_ERROR

	return VERSION_CREATED;

}

return_code add(char * filename, char * comment) {
	printf("1.I reach here");
	file_version v;

	// 1. Crea la nueva version en memoria
	// Si la operacion falla, retorna VERSION_ERROR
	
	create_version(filename, comment, &v);

	// 2. Verifica si ya existe una version con el mismo hash
	// Retorna VERSION_ALREADY_EXISTS si ya existe
	if(version_exists(filename, v.hash)==1)
		return VERSION_ALREADY_EXISTS; 
	printf("2.I reach here");
	// 3. Almacena el archivo en el repositorio.
	// El nombre del archivo dentro del repositorio es su hash (sin extension)
	// Retorna VERSION_ERROR si la operacion falla
	if(store_file(filename, v.hash)==0)
		return VERSION_ERROR; 

	// 4. Agrega un nuevo registro al archivo versions.db
	// Si no puede adicionar el registro, se debe borrar el archivo almacenado en el paso anterior
	// Si la operacion falla, retorna VERSION_ERROR
	add_new_version(&v);

	// Si la operacion es exitosa, retorna VERSION_ADDED
	return VERSION_ADDED;
}

int add_new_version(file_version * v) {
	FILE * fp = fopen(".versions/versions.db","a");
	if(fp  == NULL ){
		printf("5. I reach here\n");
		return 0;
	}
	
	fwrite(v, sizeof(file_version), 1, fp);
	// Adiciona un nuevo registro (estructura) al archivo versions.db
	return 1;
}


void list(char * filename) {

	//Abre el la base de datos de versiones (versions.db)
	//Muestra los registros cuyo nombre coincide con filename.
	//Si filename es NULL, muestra todos los registros.
    FILE * fp = fopen(".versions/versions.db", "r");
	file_version  r;
	if(fp  == NULL ){
		return;
	}

	//Leer hasta el fin del archivo 
	int cont = 1;
	while(!feof(fp)){
		
		//Realizar una lectura y retornar
		if(fread(&r, sizeof(file_version), 1, fp) != 1){
			break;
		}

		if(filename == NULL){
			//Si filename es NULL, muestra todos los registros.
			printf("%d %s %s  %.5s \n", cont, r.filename, r.comment, r.hash);
			cont = cont + 1;
		
		}else if(strcmp(r.filename,filename)==0){
			printf("%d %s %s  %.5s \n", cont, r.filename, r.comment, r.hash);
			cont = cont + 1;
		}
		//Si el registro corresponde al archivo buscado, imprimir
		//Muestra los registros cuyo nombre coincide con filename.
	}	
	fclose(fp);
}

char *get_file_hash(char * filename, char * hash) {
	char *comando;
	FILE * fp;

	struct stat s;

	//Verificar que el archivo existe y que se puede obtener el hash
	if (stat(filename, &s) < 0 || !S_ISREG(s.st_mode)) {
		perror("stat");
		return NULL;
	}

	sha256_hash_file_hex(filename, hash);

	return hash;

}

int copy(char * source, char * destination) {
	// Copia el contenido de source a destination (se debe usar open-read-write-close, o fopen-fread-fwrite-fclose)
	char c;
	FILE *f = fopen(source,"r");
	FILE *df = fopen(destination,"w");
		if (f==NULL || df==NULL)
				return 0;
	printf("4.I reach here\n");
	while(!feof(f)){
		int num_bytes_read = fread(&c, sizeof(char), 1, f);
		
		
		int num_bytes_wrote = fwrite(&c, num_bytes_read, 1, df);
	}
	printf("4.1 I reach here");
	fclose(f);
	fclose(df);
	return 1;
}

int version_exists(char * filename, char * hash) {

	
	file_version r;
	FILE *fp = fopen(".versions/version.db","r");
		if (fp==NULL)
				return 0;
	while(!feof(fp)){
		if(fread(&r, sizeof(file_version), 1, fp) != 1)
			//la lectura fallo
			break;
		// Verifica si en la bd existe un registro que coincide con filename y hash
		if(strcmp(r.filename,filename)==0 && strcmp(r.hash,hash)==0) {
				return 1;
		}
	}
	return 0;
}

int get(char * filename, int version) {

	//1. Abre la BD y busca el registro r que coincide con filename y version
	//retrieve_file(r.hash, r.filename)
	//abre la base de datos de versiones .versions/versions.db
	//y validamos que se haya abierto correctamente
	file_version r;
	FILE * fp = fopen(".versions/versions.db", "rb");

	if( fp == NULL)
		return 0;
	//Leer hasta el fin del archivo verificando si el registro coincide con filename y version
	int cont = 1;
	while(!feof(fp)){
		if(fread(&r, sizeof(file_version), 1, fp) != 1)
			break;
		//Si el registro corresponde al archivo buscado, lo restauramos
		if(strcmp(r.filename,filename)==0){
			if(cont == version){
				if(!retrieve_file(r.hash, r.filename));
					return 1;
			}
			cont++;		
		}
	}
	fclose(fp);

	return 0;
}


int store_file(char * filename, char * hash) {
	int path_size = sizeof(filename) + sizeof(hash) +2;
	char dst_filename[path_size];
	snprintf(dst_filename, path_size , "%s/%s", VERSIONS_DIR, hash);
	printf("3.I reach here");
	return copy(filename, dst_filename);
}

int retrieve_file(char * hash, char * filename) {
	int path_size = sizeof(filename) + sizeof(hash) +2;
	char src_filename[path_size];
	snprintf(src_filename, path_size, "%s/%s", VERSIONS_DIR, hash);
	return copy(src_filename, filename);
}

