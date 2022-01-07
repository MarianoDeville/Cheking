/********************************************************************************/
/*		MANEJO DE DOS MODULOS DE MEMORIA 24LC256 CON PAGINACION DE 64 BYTES		*/
/*..............................................................................*/
/*				Revisión:				1.00									*/
/*				Tipo de comunicación:	I2C										*/
/*				Memoria EEPROM:			24LC256									*/
/*				Compilador:				MPLAB IDE 8 - HI-TECH 9.60				*/
/*				Fecha de creación:		19/01/2009								*/
/*				Autor:					Mariano Ariel Deville					*/
/********************************************************************************/
/*						PROTOTIPO DE FUNCIONES									*/
/*..............................................................................*/
void Write_24LC(unsigned int pagina,unsigned char dato);
unsigned char Read_24LC(unsigned int pagina);
unsigned int Alma_Paquete_24LC(const char *dato);
void Rec_Paquete_24LC(unsigned int mem,unsigned char *dato);
void Borrar_24LC256(unsigned int pagina);
unsigned int Buscar_Tarjeta(unsigned char *dato);
/********************************************************************************/
/*			GUARDO EN LA 24LC256 UN BYTE UTILIZANDO I2C							*/
/*..............................................................................*/
void Write_24LC(unsigned int direccion,unsigned char dato)
{
	unsigned char high_byte,low_byte,chip_select;
	CLRWDT();
	chip_select=0b00000000;				// Selecciono por defecto el primier chip de memoria.
	low_byte=direccion&0xff;			// Obtengo la parte baja de la dir.
	high_byte=(direccion>>8)&0x7f;		// Obtengo la parte alta de la dir.
	I2C_Start();						// Comienzo la comunicación I2C.
	I2C_Write(0b10100000|chip_select);	// Envio el byte de control.
	I2C_Write(high_byte);				// Escribo la parte alta de la direccion de memoria.
	I2C_Write(low_byte);				// Escribo la parte baja de la direccion de memoria.
	I2C_Write(dato);					// Escribo el dato en memoria.
	I2C_Stop();							// Termino la comunicación I2C.
	DelayMs(5);							// 
	return;
}
/********************************************************************************/
/*			RECUPERO DE LA 24LC256 UN BYTE UTILIZANDO I2C						*/
/*..............................................................................*/
unsigned char Read_24LC(unsigned int direccion)
{
	unsigned char high_byte,low_byte,dato,chip_select;
	CLRWDT();
	chip_select=0b00000000;				// Selecciono por defecto el primier chip de memoria.
	low_byte=direccion&0xff;			// Obtengo la parte baja de la dir.
	high_byte=(direccion>>8)&0x7f;		// Obtengo la parte alta de la dir.
	I2C_Start();						// Comienzo la cominicación I2C.
	I2C_Write(0b10100000|chip_select);	// Envio el byte de control.
	I2C_Write(high_byte);				// Escribo la parte alta de la direccion de memoria.
	I2C_Write(low_byte);				// Escribo la parte baja de la direccion de memoria.
	I2C_RepStart();						// Reinicio la comunicación I2C.
	I2C_Write(0b10100001|chip_select);	// Envio el byte de control.
	dato=I2C_Read(0);					// Obtengo el dato guardado.
	I2C_Stop();							// Termino la comunicación I2C.
	return(dato);						// Devuelvo el valor leido.
}
/********************************************************************************/
/*	GUARDO EN LA MEMORIA 24LC256 UNA CADENA DE CARACTERES CON UN NULL AL FINAL	*/
/*..............................................................................*/
unsigned int Alma_Paquete_24LC(const char *dato)
{
// pagina tiene un valor de 0 a 2048, es la direccion de la pagina
// cada pagina de memoria tiene 16 bytes.
	unsigned char temp;
	unsigned int dir=0,pag=1;		// Direccion de memmoria y pagina.
	CLRWDT();
	do								// Busco el ultimo lugar libre.
	{
		temp=Read_24LC(pag*16);		// Cargo en un buffer el contenido de la memoria.
		if(temp!='#')				// Es el lugar donde debo escribir?
			break;					// Voy a la posición que sigue.
		pag++;
	}while(pag<2046);				// Lo hago mientras haya algo en la memoria (256Kb).
	dir=(pag*16)+1;					// Direccion de la pagina libre.
	do
	{
		Write_24LC(dir++,*dato++);	// Recorro la pagina escribiendo la cadena.
		if(dir>=(pag*16)+14)		// Exedí el espacio destinado?
			return 0;				// Salgo y devulvo un cero.
	}while(*dato&&dir<32767);		// Mientras tenga algo que grabar y no exceda la mem.
	Write_24LC(pag*16,'#');			// Marco el comienzo de la linea.
	Write_24LC(dir,0);				// Marco con un cero el final de la cadena.
	return (pag*16);				// Devuelvo la dirección de la página en donde escribí.
}
/********************************************************************************/
/*			RECUPERO DE LA MEMORIA 24LC256 UNA CADENA DE CARACTERES				*/
/*..............................................................................*/
void Rec_Paquete_24LC(unsigned int pagina,unsigned char *dato)
{
// pagina tiene un valor de 0 a 2048, es la direccion de la pagina
// cada pagina de memoria tiene 16 bytes.
	unsigned int dir;
	if(pagina>2047)					// Excedo la capacidad de la memoria.
	{
		*dato=0;					// Le agrego el final de cadena que no tiene.
		return;
	}
	dir=pagina*16;					// Calculo la dirección de la memoria.
	if(Read_24LC(dir)!='#')
	{
		*dato=0;					// Le agrego el final de cadena que no tiene.
		return;
	}
	dir++;							// Omito el caracter '#'.
	do
	{
		(*dato)=Read_24LC(dir++);	// Obtengo el valor almacenado en la mem.
		if(dir>(pagina*16)+13)		// Solo leo el número de tarjeta.
			break;
	}while(*dato++);				// Sigo hasta encontrar la marca de final (NULL).
	*dato=0;
	return;
}
/********************************************************************************/
/*		BORRO TODO EL CONTENIDO DE UNA PAGINA DE LA MEMORIA 24LC256.			*/
/*..............................................................................*/
void Borrar_24LC256(unsigned int pagina)
{
	unsigned char i=0;
	do
	{
			Write_24LC(pagina+i,0);			// Borrro el contenido de la linea.
	}while(i++<15);
	return;
}
/********************************************************************************/
/*					*/
/********************************************************************************/
unsigned int Buscar_Tarjeta(unsigned char *dato)
{
	unsigned int pag=1;
	do
	{
		Rec_Paquete_24LC(pag,ps2_str);
		if(!strcmp(ps2_str,rs_str))		// Son iguales las cadenas?
			return pag*16;				// Devuelvo la posición en memoria.
	}while(pag++<2047);
	return 0;							// Tarjeta no encontrada.
}
