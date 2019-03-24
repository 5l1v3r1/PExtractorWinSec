typedef struct EXTRAER
{
	BOOL			pBase;//indica si extraemos el programa base
	BOOL			ExArchivos;//indica si extraemos los archivos encontrados o no
	COLA_ARCHIVOS	ColaArchivos;//Estructura con los punteros a la cola de los archivos
	unsigned int	nArchivos;//Numero de archivos encontrados
	unsigned int	nExtraidos;
	/////////////////////////////////////////////////////////////////
	unsigned int	nPEs;//numero de PEs extraidos
	unsigned int	nDLLs;//numero de DLLs extraidas
	unsigned int	nSYSs;//numero de SYSs extraidos
	unsigned int	nEXEs;//numero de EXEs extraidos
	unsigned int	nDESCs;
}EXTRAER;