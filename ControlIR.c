/****************************************************************************/
/*	ENVIO DE SEÑALES IR (CODIFICACIÓN NEK) PARA MANEJAR REPRODUCTOR DE DVD.	*/
/****************************************************************************/
/*						PROTOTIPOS DE FUNCIONES								*/
/*..........................................................................*/
void Comando(unsigned char capitulo);
void Transmitir_IR(unsigned char direccion_ir,unsigned char comando_ir);
void Escribir_IR(unsigned char valor);
unsigned char Codifico(unsigned char valor);
/****************************************************************************/
/*			MACROS - PALABRAS DE DATOS PARA EL EQUIPO.						*/
/*..........................................................................*/
#define		T0		0b00010000		// Tecla 0.								*/
#define		T1		0b10000010		// Tecla 1.								*/
#define		T2		0b10110010		// Tecla 2.								*/
#define		T3		0b10100010		// Tecla 3.								*/
#define		T4		0b01000010		// Tecla 4.								*/
#define		T5		0b01110010		// Tecla 5.								*/
#define		T6		0b01100010		// Tecla 6.								*/
#define		T7		0b11000010		// Tecla 7.								*/
#define		T8		0b11110010		// Tecla 8.								*/
#define		T9		0b11100010		// Tecla 9.								*/
#define		T10		0b00000000		// Tecla 10.							*/
#define		IR_A	0b00011000		// Tecla GOTO.							*/
#define		MENU	0b11011000		// Tecla MENU.							*/
#define		ENTER	0b00000010		// Tecla ENTER.							*/
#define		PLAY	0b11101000		// Tecla PLAY/PAUSE.					*/
#define		OPEN	0b01101000		// Tecla OPEN/CLOSE.					*/
#define		POWER	0b01001000		// Tecla POWER.							*/
#define		STOP	0b11100000		// Tecla STOP.							*/
#define		RETURN	0b11000000		// Tecla RETURN.						*/
#define		TITLE	0b11111000		// Tecla TITLE.							*/
#define		REPEAT	0b01100000		// Tecla REPEAT.						*/
#define		USBDVD	0b00100000		// Tecla USB/DVD.						*/
#define		SLOW	0b00111000		// Tecla SLOW.							*/
#define		PREV	0b01000000		// Tecla PREV.							*/
#define		NEXT	0b01010000		// Tecla NEXT.							*/
/****************************************************************************/
/*			Busco la tarjeta y ejecuto el comando que corresponda.			*/
/*..........................................................................*/
void Comando(unsigned char capitulo)
{
	unsigned char aux;
	Transmitir_IR(0,IR_A);			// Botón GOTO.
	if(capitulo>9)					// El capítulo es mayor a nueve?
	{
		aux=capitulo/10;			// Obtengo la decena.
		capitulo-=(aux*10);			// Solo dejo las unidades.
		do
		{
			Transmitir_IR(0,T10);
		}while(--aux);				// Mando el 10+ las veces que haga falta.
	}
	capitulo=Codifico(capitulo);	// Transformo el número en un código para el DVD.
	Transmitir_IR(0,capitulo);		// Segundo dígito del número de capitulo.
	Transmitir_IR(0,ENTER);
	return;
}
/****************************************************************************/
/*			GENERO LOS CÓDIGOS IR PARA MANEJAR EL REPRODUCTOR				*/
/*..........................................................................*/
void Transmitir_IR(unsigned char direccion_ir,unsigned char comando_ir)
{
	unsigned char bits;
	Escribir_IR('I');
	bits=15;
	do						// Transmito la dirección.
	{
		if(bits>7)
			Escribir_IR(0b00000001&(direccion_ir>>(bits-8)));
		else
			Escribir_IR(!(0b00000001&(direccion_ir>>bits)));
	}while(bits--);
	bits=15;
	do						// Transmito el comando.
	{
		if(bits>7)
			Escribir_IR(0b00000001&(comando_ir>>(bits-8)));
		else
			Escribir_IR(!(0b00000001&(comando_ir>>bits)));
	}while(bits--);
	Escribir_IR(0);			// Fin de transmisión.
	Escribir_IR('R');		// Orden de repetición.
	return;
}
/****************************************************************************/
/*		GENERO LOS BITS DE CÓDIGOS IR PARA MANEJAR EL REPRODUCTOR			*/
/*..........................................................................*/
void Escribir_IR(unsigned char valor)
{
	if(valor=='I')			// Inicializo la transmisión?
	{
		SENAL_IR=1;			// Prendo el oscilador de 38 KHz.
		DelayMs(8);			// Son 9 milisegundos.
		Delay10Us(85);
		SENAL_IR=0;			// Apago el oscilador de 38 KHz.
		DelayMs(4);			// Son 4.5 milisegundos.
		Delay10Us(45);
		return;
	}
	if(valor=='R')			// Botón aun apretado?
	{
		DelayMs(39);
		SENAL_IR=1;			// Prendo el oscilador de 38 KHz.
		DelayMs(8);			// Son 9 milisegundos.
		Delay10Us(90);
		SENAL_IR=0;			// Apago el oscilador de 38 KHz.
		Delay10Us(210);		// Pausa de 2.2 ms.
		Escribir_IR(0);		// Fin de transmisión.
		DelayMs(95);		// Pausa establecida por el protocolo.
		return;
	}
	SENAL_IR=1;				// Prendo el oscilador de 38 KHz.
	Delay10Us(52);			// Son 560 microsegundos.
	SENAL_IR=0;				// Apago el oscilador de 38 KHz.
	Delay10Us(51);			// Son 560 microsegundos.
	if(valor)				// Debo escribir un uno?
		Delay10Us(103);		// Demora de 1130 microsegundos para que sea un uno.
	return;
}
/****************************************************************************/
/*					*/
/*..........................................................................*/
unsigned char Codifico(unsigned char valor)
{
	switch(valor)			// Codifico el número.
	{
	case 0:
		valor=T0;
		break;
	case 1:
		valor=T1;
		break;
	case 2:
		valor=T2;
		break;
	case 3:
		valor=T3;
		break;
	case 4:
		valor=T4;
		break;
	case 5:
		valor=T5;
		break;
	case 6:
		valor=T6;
		break;
	case 7:
		valor=T7;
		break;
	case 8:
		valor=T8;
		break;
	case 9:
		valor=T9;
		break;
	}
	return valor;
}
