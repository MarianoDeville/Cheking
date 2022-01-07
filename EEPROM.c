/****************************************************************************/
/*				MANEJO DE LA MEMÓRIA EEPROM DEL PIC 18F4620					*/
/*--------------------------------------------------------------------------*/
/*						PROTOTIPOS DE FUNCIONES								*/
/*..........................................................................*/
unsigned char Eeprom_Read(unsigned int address);
void Eeprom_Write(unsigned int address,unsigned char value);
void Guardo_Contrasena(unsigned char *pass);
/****************************************************************************/
/*		DEVUELVO EL BYTE LEIDO EN LA DIRECCIÓN PASADA COMO ARGUMENTO		*/
/*..........................................................................*/
unsigned char Eeprom_Read(unsigned int address)
{
	CLRWDT();
	FREE=0;
	EEADR=address & 0xFF;		// Paso la dirección en la que voy a leer.
	EEADRH=(address>>8) & 0xFF;	// Paso la dirección en la que voy a leer.
	EEPGD=0;					// Point to EE memory
	CFGS=0;
	RD=1;						// Inicio el ciclo de lectura.
	while(RD)					// Espero a que termine la lectura.
		continue;				// Si demora mucho actua el WDT.
	return EEDATA;				// Devuelvo el valor leido.
}
/****************************************************************************/
/*				ESCRIBO UN BYTE EN LA MEMORIA EEPROM						*/
/*..........................................................................*/
void Eeprom_Write(unsigned int address,unsigned char value)
{
 	while(WR)					// Si hay una escritura en curso espero.
		continue;				// Si demora mucho actua el WDT.
	CLRWDT();
	FREE=0;
	EEADR=address & 0xFF;		// Paso la dirección en la que voy a leer.
	EEADRH=(address>>8) & 0xFF;	// Paso la dirección en la que voy a leer.
	EEDATA=value; 				// Paso el byte que deseo escribir.
	EEPGD=0;
	CFGS=0;
 	WREN=1;						// EEPROM modo escritura.
	EECON2=0x55;
	EECON2=0xAA;
	WR=1;						// Escribo en memoria.
 	while(WR)					// Espero que termine de escribir.
		continue;				// Si demora mucho actua el WDT.
	WREN=0;						// Deshabilito EEPROM modo escritura.
	return;
}
/****************************************************************************/
/*				GUARDO EN LA MEMORIA LA CONTRASEÑA ESCRITA						*/
/*..........................................................................*/
void Guardo_Contrasena(unsigned char *pass)
{
	unsigned int i=20;
	Eeprom_Write(i++,'@');		// Marco la contraseña.
	do
	{
		Eeprom_Write(i++,*pass);
	}while(*pass++);
	return;
}
