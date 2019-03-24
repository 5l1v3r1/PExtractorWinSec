#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#include "ColaFicheros.h"
#include "Estructuras.h"
#include "PE.h"
#include "funciones.h"

//Numero de parametros
#define N_OPCIONES	2

void Banner()
{
	printf("\n#####################################");
	printf("\n#   PExtractor v0.18b - by Lympex   #");
	printf("\n#=================================-=#");
	printf("\n#         Lympex@GMail.Com          #");
	printf("\n#         Lympex@L-Bytes.Es         #");
	printf("\n#        Lympex@ElHacker.Net        #");
	printf("\n#-----------------------------------#");
	printf("\n#         http://L-Bytes.Es         #");
	printf("\n#      http://Foro.ElHacker.Net     #");
	printf("\n#####################################\n");

	return;
}

void Uso(const char *prog)
{
	printf("\n[!] Uso: %s fichero.exe [opciones]",prog);
	printf("\n\n[+] Opciones:");
	printf("\n    -efb   ==> Extraer programa base");
	printf("\n    -efe   ==> Extraer ficheros encontrados\n");
	
	return;
}

int main(int argc, char *argv[])
{
	EXTRAER			datos;
	BOOL			ErrParam;
	int				i,j;

	Banner();
	if(argc<2)
	Uso((const char*)argv[0]);
	else
	{
		InicializaEXTRAER(&datos);
		ErrParam=FALSE;

		if(argc>2) //si hay opciones de ejecucion
		for(i=3;i<=argc;i++)
			for(j=0;j<N_OPCIONES;j++)
			{
				switch(i)
				{
					case 3:
						if(strcmp(argv[i-1],"-efb")==0)//extraer el programa base?
						datos.pBase=TRUE;						
						else if(strcmp(argv[i-1],"-efe")==0)//extraer ficheros encontrados?
						datos.ExArchivos=TRUE;
						else
						ErrParam=TRUE;
						break;
						
					case 4:
						if(strcmp(argv[i-1],"-efe")==0)//extraer ficheros encontrados?
						datos.ExArchivos=TRUE;
						else if(strcmp(argv[i-1],"-efb")==0)//extraer el programa base?
						datos.pBase=TRUE;
						else
						ErrParam=TRUE;
						break;							
				}
			}

		if(ErrParam==FALSE)
			if(ExisteArchivo(argv[1])==TRUE && EsPE(argv[1])==TRUE)
			{
				AnalizarArchivos(argv[1],&datos);
				printf("\n[+] %d Fichero(s) encontrado(s) en el interior de \"%s\"",datos.nArchivos,argv[1]);
	
				if(datos.ExArchivos==TRUE && datos.nArchivos>=0)
				{
					puts("");	
					if(datos.nPEs>0)
						printf("\n    - %d EXE(s)",datos.nPEs);
	
					if(datos.nDLLs>0)
						printf("\n    - %d DLL(s)",datos.nDLLs);
	
					if(datos.nSYSs>0)
						printf("\n    - %d SYS(s)",datos.nSYSs);
	
					if(datos.nDESCs>0)
						printf("\n    - %d Desconocido(s)",datos.nDESCs);

					printf("\n\n[+] Extrayendo archivos...\n");
					ExtraerCola(argv[1],&datos,FALSE);
				}

				if(datos.pBase==TRUE)//extraemos?
				{
					printf("\n\n[+] Extrayendo programa base...\n");
					ExtraerCola(argv[1],&datos,TRUE);
				}
				printf("\n[+] Extraidos: %d Archivo(s)",datos.nExtraidos);

				puts("");
	
			}
			else
			printf("\n[!] Error: El fichero \"%s\" no existe, o no es un ejecutable\n",argv[1]);
		else
		{
			printf("\n[!] Error de parametros\n");
			Uso(argv[0]);
		}
	}
  	
  return 0;
}
