/**********************************************************************************/
/* DEFINICION DE LAS ESTRUCTURAS NECESARIAS PARA CREAR LA COLA DE ARCHIVOS (FIFO) */
/**********************************************************************************/
typedef struct ARCHIVO
{
	unsigned int	Inicio;//posicion de inicio
	unsigned int	Fin;//posicion de fin
	unsigned int	Tam;//tamaño
	unsigned int	TamFOrig;//Tamaño del fichero origen
	////////////////////////////////////////////////////////////
	BOOL			PE;//es PE?
	WORD			Tipo;//qué tipo de pe?
	time_t			TimeStamp;
	BOOL			Seguro;//Seguro que es ese tipo de archivo?
	////////////////////////////////////////////////////////////
	char			*Nombre;
	////////////////////////////////////////////////////////////
	unsigned int	ID;
	////////////////////////////////////////////////////////////
}ARCHIVO;

typedef struct NODO
{
  ARCHIVO		p;
  struct NODO	*sig;
  struct NODO	*ant;
}NODO;

typedef struct COLA_ARCHIVOS
{
  NODO 		*Frente;
  NODO 		*Final;
}COLA_ARCHIVOS;

/************************************/
/* FUNCION PARA INICIALIZAR LA COLA */
/************************************/
void InicializarCola(COLA_ARCHIVOS *c)
{
	c->Frente=c->Final=NULL;
	return;
}

/*********************************************************/
/* FUNCION QUE DEVUELVE TRUE/FALSE SI LA COLA ESTA VACIA */
/*********************************************************/
BOOL ColaVacia(COLA_ARCHIVOS c)
{
	BOOL ret;
	
	if(c.Frente==NULL || c.Final==NULL)
	{
		ret=TRUE;
	}else{
		ret=FALSE;
	}
	
	return ret;
}

/**************************************************/
/* FUNCION PARA INSERTAR UN NUEVO NODO EN LA COLA */
/**************************************************/
void InsertarNodo(ARCHIVO p,COLA_ARCHIVOS *c)
{
	NODO *nuevo;
	
	nuevo=(NODO*)malloc(sizeof(NODO));
	memset(nuevo,0,sizeof(NODO));
	
	nuevo->p=p;
	nuevo->ant=NULL;

	if(ColaVacia(*c)==TRUE)
	{
		nuevo->sig=NULL;
		c->Frente=c->Final=nuevo;
	}else{
		c->Final->ant=nuevo;
		nuevo->sig=c->Final;
		c->Final=nuevo;
	}
	
	return;
}

/********************************************/
/* FUNCION PARA ELIMINAR UN NODO DE LA COLA */
/********************************************/
void EliminarNodo(COLA_ARCHIVOS *c)
{
	NODO *aux;
	NODO *ultimo;
	
	if(ColaVacia(*c)==FALSE)
	{
		//si solo hay un nodo
		if(c->Frente==c->Final)
		{
			aux=c->Frente;//guardamos la direccion del nodo a eliminar
			c->Frente=c->Final=NULL;//inicializamos la cola para desenlazar el nodo
		}else{
			aux=c->Frente;//guardamos la direccion del nodo a eliminar
			
			//modificamos el penúltimo nodo y el puntero Frente
			ultimo=c->Frente->ant;
			ultimo->sig=NULL;
			c->Frente=ultimo;
		}
		
		free(aux);
	}
	
	return;
}

unsigned int Elementos(COLA_ARCHIVOS c)
{
	unsigned int	ret;
	NODO			*aux;

	ret=0;
	if(ColaVacia(c)==FALSE)
	{
		aux=c.Frente;
		while(aux!=NULL)
		{
			ret++;
			aux=aux->ant;
		}
	}

	return ret;
}