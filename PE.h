WORD TiposPE[] = 
{
	IMAGE_FILE_SYSTEM,
	IMAGE_FILE_DLL,
	IMAGE_FILE_EXECUTABLE_IMAGE,
	//IMAGE_FILE_RELOCS_STRIPPED,
	//IMAGE_FILE_LINE_NUMS_STRIPPED,
	//IMAGE_FILE_LOCAL_SYMS_STRIPPED,
	//IMAGE_FILE_AGGRESIVE_WS_TRIM,
	//IMAGE_FILE_LARGE_ADDRESS_AWARE,
	//IMAGE_FILE_BYTES_REVERSED_LO,
	//IMAGE_FILE_32BIT_MACHINE,
	//IMAGE_FILE_DEBUG_STRIPPED,
	//IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP,
	//IMAGE_FILE_NET_RUN_FROM_SWAP,
	//IMAGE_FILE_UP_SYSTEM_ONLY,
	//IMAGE_FILE_BYTES_REVERSED_HI
};

BOOL EsCabeceraMSDOS(IMAGE_DOS_HEADER Cabecera)
{
	BOOL ret=FALSE;

	if(Cabecera.e_magic==IMAGE_DOS_SIGNATURE)
		ret=TRUE;

	return ret;
}

BOOL EsCabeceraNT(IMAGE_NT_HEADERS Cabecera)
{
	BOOL ret=FALSE;

	if(Cabecera.Signature==IMAGE_NT_SIGNATURE)
		ret=TRUE;

	return ret;
}

//detecta si un fichero es ejecutable o no
BOOL EsPE(char const *Ruta)
{
	BOOL				ret;
	IMAGE_DOS_HEADER	Cabecera1;
	IMAGE_NT_HEADERS	Cabecera2;
	FILE				*f;

	ret=FALSE;
	
	f=fopen(Ruta,"rb");
	if(f!=NULL)
	{
		fread(&Cabecera1,sizeof(IMAGE_DOS_HEADER),1,f);
		if(EsCabeceraMSDOS(Cabecera1))
		{
			fseek(f,Cabecera1.e_lfanew,SEEK_SET);
			fread(&Cabecera2,sizeof(IMAGE_NT_HEADERS),1,f);
			if(EsCabeceraNT(Cabecera2))
				ret=TRUE;
		}
		fclose(f);
	}
	
	return ret;
}

void TipoPE(ARCHIVO *Tipo,IMAGE_FILE_HEADER *Cabecera)
{
	unsigned int	i;
	BOOL			Sigue;

	Sigue=TRUE;
	for(i=0;i<sizeof(TiposPE) && Sigue==TRUE;i++)
	{
		if(Cabecera->Characteristics & TiposPE[i])
		{
			switch(TiposPE[i])
			{
			case IMAGE_FILE_DLL:
				Tipo->Tipo=IMAGE_FILE_DLL;
				Tipo->Seguro=TRUE;
				Sigue=FALSE;
				break;

			case IMAGE_FILE_SYSTEM:
				Tipo->Tipo=IMAGE_FILE_SYSTEM;
				Tipo->Seguro=TRUE;
				Sigue=FALSE;
				break;

			case IMAGE_FILE_EXECUTABLE_IMAGE:
				Tipo->Tipo=IMAGE_FILE_EXECUTABLE_IMAGE;
				Tipo->Seguro=TRUE;
				Sigue=FALSE;
				break;
			}
		}
	}

	if(Tipo->Tipo==0)
	{
		Tipo->Tipo=IMAGE_FILE_EXECUTABLE_IMAGE;
		Tipo->Seguro=FALSE;
	}

	return;
}

void TimeStamp(IMAGE_FILE_HEADER Cabecera, ARCHIVO *a)
{
	a->TimeStamp=Cabecera.TimeDateStamp;
	
	return;
}
