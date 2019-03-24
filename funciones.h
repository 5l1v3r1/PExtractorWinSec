void InicializaEXTRAER(EXTRAER *p)
{
	p->nArchivos=0;
	p->nExtraidos=0;
	p->nPEs=0;
	p->nDLLs=0;
	p->nSYSs=0;
	p->nDESCs=0;
	p->pBase=FALSE;
	p->ExArchivos=FALSE;
	InicializarCola(&p->ColaArchivos);

	return;
}

void InicializarARCHIVO(ARCHIVO *t)
{
	t->PE=FALSE;
	t->Seguro=FALSE;
	t->Tipo=0;
	t->Fin=0;
	t->Inicio=0;
	t->Tam=0;
	//t->TamFOrig=0;

	return;
}

BOOL ExisteArchivo(char *Ruta)
{
	BOOL ret;
	FILE *f=fopen(Ruta,"rb");
	
	if(f!=NULL)
	{
		ret=TRUE;
		fclose(f);
	}else
	ret=FALSE;		
	
	return ret;
}

BOOL ExtraerArchivo(FILE *f, ARCHIVO Archivo)
{
	char	Buffer;
	BOOL	ret;
	FILE	*b;

	b=fopen(Archivo.Nombre,"wb");
	ret=FALSE;
	if(b!=NULL)
	{
		fseek(f,Archivo.Inicio,SEEK_SET);//nos ponemos al comienzo del archivo
		while((unsigned int)ftell(f)<Archivo.Fin)
		{
			fread(&Buffer,sizeof(char),1,f);
			fwrite(&Buffer,sizeof(char),1,b);
		}
		fclose(b);

		printf("\n    + Fichero: \"%s\"",Archivo.Nombre);
		printf("\n      - Comienza en:..................%.3f (Bytes)",(float)Archivo.Inicio/1024);
		printf("\n      - Finaliza en:..................%.3f (Bytes)",(float)Archivo.Fin/1024);
		printf("\n      - Tam. en archivo:..............%.3f (Bytes)",(float)Archivo.Tam/1024);
		printf("\n      - Tam. del archivo original:....%.3f (Bytes)",(float)Archivo.TamFOrig/1024);
		if(Archivo.PE==TRUE)
		{
			printf("\n      - Tipo:.........................PE(");
			switch(Archivo.Tipo)
			{
			case IMAGE_FILE_DLL:
				printf("DLL");
				break;

			case IMAGE_FILE_SYSTEM:
				printf("SYS");
				break;

			case IMAGE_FILE_EXECUTABLE_IMAGE:
				if(Archivo.Seguro==TRUE)
					printf("EXE");
				else
					printf("EXE?");
				break;

			default:
				printf("DESCONOCIDO");
				break;
			}
			printf(")");
		}
		printf("\n      - Compilado en:.................%s",ctime(&Archivo.TimeStamp));
		ret=TRUE;
	}else
		printf("\n[!] Error al crear el archivo \"%s\"",Archivo.Nombre);

	return ret;
}

void ExtraerCola(char *Ruta,EXTRAER *datos,BOOL Base/*extraer solo el programa base*/)
{
	FILE	*f;
	NODO	*aux;

	f=fopen(Ruta,"rb");

	if(f!=NULL)
	{
		if(ColaVacia(datos->ColaArchivos)==FALSE)
		{
			aux=datos->ColaArchivos.Frente;
			if(Base==FALSE)
			{
				if(datos->pBase==TRUE)
					aux=aux->ant;

				while(aux!=NULL)
				{
					if(ExtraerArchivo(f,aux->p)==TRUE)
						datos->nExtraidos++;

					aux=aux->ant;
				}
			}else{
				if(ExtraerArchivo(f,aux->p)==TRUE)
					datos->nExtraidos++;
			}
		}
	}else{
		printf("\n[!] Error al abrir el archivo\"%s\"",Ruta);
	}

	return;
}

int PosSigArchivo(FILE *a,unsigned int Tam,ARCHIVO *Tipo)
{
	unsigned int		posicion[3];
	BOOL				encontrado;
	IMAGE_DOS_HEADER	CabeceraMSDOS;
	IMAGE_NT_HEADERS	CabeceraNT;
	unsigned int		PosFinal;
	
	encontrado=FALSE;
	posicion[0]=-1;
	posicion[1]=ftell(a);
		
	//mostramos la posición de la siguiente cabecera PE
	do
	{
		posicion[2]=ftell(a);

		/*BUSCAMOS FICHEROS PE*/
		//TamLectura=(sizeof(IMAGE_DOS_HEADER)+sizeof(IMAGE_NT_HEADERS));//asignamos el tamaño de la lectura
		fread(&CabeceraMSDOS,sizeof(IMAGE_DOS_HEADER),1,a);
		if(EsCabeceraMSDOS(CabeceraMSDOS))
		{
			fseek(a,posicion[2]+CabeceraMSDOS.e_lfanew,SEEK_SET);
			fread(&CabeceraNT,sizeof(IMAGE_NT_HEADERS),1,a);
			if(EsCabeceraNT(CabeceraNT))
			{
				encontrado=TRUE;
				posicion[0]=posicion[2];

				Tipo->PE=TRUE;
				//analizamos qué tipo de PE es.
				TipoPE(Tipo,&CabeceraNT.FileHeader);
				//cojemos su timestamp
				TimeStamp(CabeceraNT.FileHeader,Tipo);
			}
		}

		PosFinal=Tam-(ftell(a)-posicion[2]);
		fseek(a,posicion[2]+1,SEEK_SET);	
	}while(!((unsigned int)ftell(a)==PosFinal || encontrado==TRUE));

	fseek(a,posicion[1],SEEK_SET);
	
	return posicion[0];
}

void CrearNombreArchivo(ARCHIVO *Archivo)
{
	unsigned int	LenNombre,i;
	
	LenNombre=0;
	i=1;
	while(LenNombre>Archivo->ID)
	{
		LenNombre+=10^i;
		i++;
	}
	LenNombre+=5;
	if(Archivo->ID==0)
		LenNombre+=strlen("_prog.base");
	
	Archivo->Nombre=(char*)malloc(LenNombre*sizeof(char));
	memset(Archivo->Nombre,'\0',LenNombre);
	
	itoa(Archivo->ID,Archivo->Nombre,10);
	if(Archivo->ID==0)
		strcat(Archivo->Nombre,"_prog.base");

	switch(Archivo->Tipo)
	{
	case IMAGE_FILE_DLL:
		strcat(Archivo->Nombre,".dll");
		break;

	case IMAGE_FILE_SYSTEM:
		strcat(Archivo->Nombre,".sys");
		break;

	case IMAGE_FILE_EXECUTABLE_IMAGE:
		if(Archivo->Seguro==TRUE)
			strcat(Archivo->Nombre,".exe");
		else
			strcat(Archivo->Nombre,".desc.exe");
		break;

	default:
		strcat(Archivo->Nombre,".des");
		break;
	}
	
	return;
}

//devuelve el tamaño de un archivo
unsigned int FileSize(FILE *f)
{
	unsigned int 	tam,pos;
	char 			Buffer[1024];
	
	pos=ftell(f);	
	//nos vamos al final del archivo
	while(!feof(f))
	fread(Buffer,sizeof(char),sizeof(Buffer),f);
		
	//obtenemos el tamaño
	tam=ftell(f);
	fseek(f,pos,SEEK_SET);
	
	return tam;
}

void Avanza(FILE *f,ARCHIVO Tipo)
{
	if(Tipo.PE==TRUE)
		fseek(f,sizeof(IMAGE_DOS_HEADER),SEEK_CUR);
	else
		switch(Tipo.Tipo)
		{
		default:
			break;
		}

	return;
}

void AnalizarArchivos(char const *Ruta,EXTRAER *param)
{
	ARCHIVO				Archivo[2];
	int 				PosArchivo[2];
	unsigned int		TamArchivo;
	IMAGE_DOS_HEADER	a;
	FILE 				*f;
	
	f=fopen(Ruta,"rb");	
	if(f!=NULL)
	{
		TamArchivo=FileSize(f);

		printf("\n[+] Buscando ficheros en \"%s\"...\n",Ruta);
		
		if(param->pBase==FALSE)//nos saltamos el programa base
		{
			fread(&a,sizeof(IMAGE_DOS_HEADER),1,f);
			param->nArchivos++;
		}

		Archivo[0].TamFOrig=TamArchivo;
		do
		{
			InicializarARCHIVO(&Archivo[0]);
			InicializarARCHIVO(&Archivo[1]);

			PosArchivo[0]=PosSigArchivo(f,TamArchivo,&Archivo[0]);
			if(PosArchivo[0]>=0)
			{
				fseek(f,PosArchivo[0],SEEK_SET);
				Archivo[0].Inicio=PosArchivo[0];

				if(param->nArchivos>0)
				{
					printf("\n[+] Fichero ");

					printf("PE (");
					switch(Archivo[0].Tipo)
					{
					case IMAGE_FILE_DLL:
						printf("DLL");
						param->nDLLs++;
						break;

					case IMAGE_FILE_SYSTEM:
						printf("SYS");
						param->nSYSs++;
						break;

					case IMAGE_FILE_EXECUTABLE_IMAGE:
						if(Archivo[0].Seguro==TRUE)
							printf("EXE");
						else
							printf("EXE?");
						param->nPEs++;
						break;

					default:
						param->nDESCs++;
						break;
					}
					printf(")");
	
					printf(" Encontrado en: 0x%hx",Archivo[0].Inicio);
					printf("\n    - N. de archivos encontrados: %d\n",param->nArchivos);
				}

				Avanza(f,Archivo[0]);
				PosArchivo[1]=PosSigArchivo(f,TamArchivo,&Archivo[1]);
			
				//tenemos más PEs?
				if(PosArchivo[1]>=0)//si hay archivos y tenemos que extraerlo
				{
					Archivo[0].Fin=PosArchivo[1];
					//ExtraerContenido(f,PosArchivo[0],PosArchivo[1],param->nArchivos,Tipo[0]);
				}else if(param->nArchivos>0){//para que si tenemos activada la opcion "-efb" y no hay archivos, no lo "copie"
					Archivo[0].Fin=TamArchivo;
				}
				Archivo[0].Tam=Archivo[0].Fin-Archivo[0].Inicio;
				Archivo[0].ID=param->nArchivos;
				
				CrearNombreArchivo(&Archivo[0]);
				InsertarNodo(Archivo[0],&param->ColaArchivos);
				param->nArchivos++;

				//miramos si solo queremos el programa base
				if(!(param->pBase==TRUE && param->ExArchivos==FALSE))
					PosArchivo[0]=PosArchivo[1];
				else
					PosArchivo[0]=-1;
			}
		}while(PosArchivo[0]>0);		
		fclose(f);
	}else{
		printf("\n[!] Error al abrir el archivo \"%s\"\n",Ruta);
	}

	param->nArchivos--;
	return;
}
