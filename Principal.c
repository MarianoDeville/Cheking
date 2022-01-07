/********************************************************************************/
/*				CHECKING - Sistema de bienvenida para eventos.					*/
/*..............................................................................*/
/*			Revisión:				1.00										*/
/*			PIC:					PIC18F4620									*/
/*			Memoria:				24LC256.									*/
/*			Display:				LCD 20 caracteres por 2 lineas.				*/
/*			Teclado:				PS2											*/
/*			Comunicación:			RS232 - I2C.								*/
/*			Compilador:				MPLAB IDE 8.70 - HI-TECH PICC-18 9.50 PL3.	*/
/*			Checksum:				0Xe39e										*/
/*			Fecha de creación:		09/09/2011									*/
/*			Autor:					Mariano Ariel Deville						*/
/********************************************************************************/
/*				ORGANIZACIÓN DE LA MEMORIA 24LC256								*/
/*..............................................................................*/
/*		La memoria está dividida en páginas de 16 bytes, cada página comienza	*/
/*	con un caracter '#' y a continuación contiene el número de tarjeta (10 B)	*/
/*	seguido del saludo personalizado, posición (mesa que le corresponde) y  un	*/
/*	tercer byte para proximos usos.												*/
/*		Con páginas de 16 bytes tenemos un total de 2047 páginas para utilizar,	*/
/*	la primer página no la utilizo.												*/
/********************************************************************************/
/*				MACROS Y CONFIGURACIÓN DE HARDWARE								*/
/*..............................................................................*/
#define	PIC_CLK		20000000			// Velocidad del clock.					*/
#define	ENTRADA		1					//										*/
#define	SALIDA		0					//										*/
#define	RS232_ON	CREN=1;RCIE=1		// Prendo la recepción RS232.			*/
#define	RS232_OFF	RCIE=0;CREN=0		// Apago la recepción RS232.			*/
#define	PS2_ON		INT0IF=0;INT0IE=1	// Interrupción para PS2 habilitada.	*/
#define	PS2_OFF		INT0IE=0;INT0IF=0	// Interrupción para PS2 deshabilitada.	*/
#define	TMR0_ON		TMR0IF=0;TMR0IE=1	// Interrupción para TMR0 habilitada.	*/
#define TMR0_OFF	TMR0IE=0;TMR0IF=0	// Interrupción para TMR0 deshabilitada.*/
#define	TMR1_ON		TMR1IF=0;TMR1IE=1	// Interrupción para TMR1 habilitada.	*/
#define TMR1_OFF	TMR1IE=0;TMR1IF=0	// Interrupción para TMR1 deshabilitada.*/
#define	TMR2_ON		TMR2IF=0;TMR2IE=1	// Interrupción para TMR2 habilitada.	*/
#define TMR2_OFF	TMR2IE=0;TMR2IF=0	// Interrupción para TMR2 deshabilitada.*/
#define	TIEMPO_S	15					// Tiempo del saludo en segundos.		*/
#define	TIEMPO_M	5					// Tiempo que se visualiza la mesa.		*/
#define	TIEMPO_R	30					// Tiempo entre rep. del capitulo 1.	*/
/*------------------------------------------------------------------------------*/
/*				DEFINO LOS NOMBRES DE LOS PINES DE E/S							*/
/*..............................................................................*/
#define	CLOCK_PS2	RB0				// Clock desde un teclado PS2.				*/
#define	DATO_PS2	RB1				// Datos desde un teclado PS2.				*/
#define	LED_R		RB2				// LED de debug rojo.						*/
#define	LED_V		RB3				// LED de debug verde.						*/
#define	AUX_16		RB4				// Sin uso.									*/
#define	AUX_17		RB5				// Sin uso.									*/
#define	AUX_8		RC0				// Sin uso.									*/
#define	AUX_9		RC1				// Sin uso.									*/
#define	AUX_10		RC2				// Sin uso.									*/
#define	SENAL_IR	RC5				// Manejo del oscilador para generar IR.	*/
#define	AUX_11		RD0				// Sin uso.									*/
#define	AUX_12		RD1				// Sin uso.									*/
#define	AUX_13		RD2				// Sin uso.									*/
#define	BUZZER		RD3				// Aviso sonoro de la lectura.				*/
#define	AUX_4		RD4				//											*/
#define	AUX_3		RD5				//											*/
#define	AUX_2		RD6				//											*/
#define	AUX_1		RD7				//											*/
#define	AUX_5		RE0				//											*/
#define	AUX_6		RE1				//											*/
#define	AUX_7		RE2				//											*/
/********************************************************************************/
/*						VARIABLES GLOBALES										*/
/*..............................................................................*/
bank1 unsigned char rs_str[40],pos;				// Necesario para el RS232.		*/
bank1 unsigned char tiempo_r;					// 								*/
bank1 unsigned char tono_audible,audio;			// Tono de aviso.				*/
bank1 unsigned int bienvenida,duracion;			// Tiempo de los mensajes.		*/
bank1 unsigned int pagina;						//								*/
bank2 bit bandera,actualizo;					// Banderas.					*/
/********************************************************************************/
/*						PROTOTIPO DE FUNCIONES									*/
/*..............................................................................*/
void Interpreto_Comando(void);
void Aviso(unsigned int tiempo, unsigned char tono);
void Entrada_Llavero(void);
void Pausa(unsigned char segundos);
void Actualizo_Vis(void);
/********************************************************************************/
/*							LIBRERIAS											*/
/*..............................................................................*/
#include	"htc.h"				// Necesario para el compilador.				*/
#include	"string.h"			// Necesario para strcmp().						*/
#include	"RS232.c"			// Configuración y comunicación puerto serie.	*/
#include	"I2C.c"				// Manejo del módulo I2C interno del PIC.		*/
#include	"EEPROM.c"			// Manejo de la memoria EEPROM del PIC.			*/
#include	"Interrup.c"		// Manejo de interrupciones.					*/
#include	"Delay.c"			// Rutinas de demoras.							*/
#include	"Lcd.c"				// Rutina de manejo de un display LCD.			*/
#include	"TecladoPS2.c"		// Rutina de interpretación PS2.				*/
#include	"24LC256.c"			// Manejo de la memoria externa I2C 24LC256.	*/
#include	"ControlIR.c"		// Generación del código IR.					*/
#include	"Menu.c"			// Menu administrativo y de mantenimiento.		*/
/********************************************************************************/
__CONFIG(1,IESODIS & FCMDIS & HS);							//					*/
__CONFIG(2,BOREN & BORV43 & PWRTEN & WDTEN & WDTPS512);		//					*/
__CONFIG(3,MCLREN & LPT1EN & PBDIGITAL & PBANDIS);			//					*/
__CONFIG(4,XINSTDIS & DEBUGDIS & LVPDIS & STVREN);			//					*/
__CONFIG(5,UNPROTECT);										//					*/
__CONFIG(6,WRTEN);											//					*/
/********************************************************************************/
void main(void)
{
/********************************************************************************/
/*			Configuración de los puertos										*/
/*..............................................................................*/
	ADCON1=0b00001111;	// Sin entradas analógicas.								*/
	PORTA=0;			// Reseteo el puerto.									*/
	PORTB=0;			// Reseteo el puerto.									*/
	PORTC=0;			// Reseteo el puerto.									*/
	PORTD=0;			// Reseteo el puerto.									*/
	PORTE=0;			// Reseteo el puerto.									*/
/********************************************************************************/
/*				Configuración de los puertos									*/
/*..............................................................................*/
	TRISA0=SALIDA;		// Salida para el LCD RS.								*/
	TRISA1=SALIDA;		// Salida para el LCD E.								*/
	TRISA2=SALIDA;		// Salida para el LCD AD4.								*/
	TRISA3=SALIDA;		// Salida para el LCD AD5.								*/
	TRISA4=SALIDA;		// Salida para el LCD AD6.								*/
	TRISA5=SALIDA;		// Salida para el LCD AD7.								*/
/*..............................................................................*/
	TRISB0=ENTRADA;		// PS2 - CLOCK.											*/
	TRISB1=ENTRADA;		// PS2 - DATOS.											*/
	TRISB2=SALIDA;		// Salida a LED rojo de debug.							*/
	TRISB3=SALIDA;		// Salida a LED verde de debug.							*/
	TRISB4=ENTRADA;		// Entrada auxiliar 16.									*/
	TRISB5=ENTRADA;		// Entrada auxiliar 17.									*/
	TRISB6=ENTRADA;		// ICSP - PGC.											*/
	TRISB7=ENTRADA;		// ICSP - PGD.											*/
/*..............................................................................*/
	TRISC0=ENTRADA;		// Entrada auxiliar 8.									*/
	TRISC1=ENTRADA;		// Entrada auxiliar 9.									*/
	TRISC2=ENTRADA;		// Entrada auxiliar 10.									*/
	TRISC3=ENTRADA;		// I2C - SCL serial clock.								*/
	TRISC4=ENTRADA;		// I2C - SDA serial data.								*/
	TRISC5=SALIDA;		// Salida al oscilador generador IR.					*/
	TRISC6=SALIDA;		// RS232 - Salida TX.									*/
	TRISC7=ENTRADA;		// RS232 - Entrada RX.									*/
/*..............................................................................*/
	TRISD0=ENTRADA;		// Entrada auxiliar 11.									*/
	TRISD1=ENTRADA;		// Entrada auxiliar 12.									*/
	TRISD2=ENTRADA;		// Entrada auxiliar 13.									*/
	TRISD3=SALIDA;		// Salida de pulsos al BUZZER.							*/
	TRISD4=ENTRADA;		// Entrada auxiliar 4.									*/
	TRISD5=ENTRADA;		// Entrada auxiliar 3.									*/
	TRISD6=ENTRADA;		// Entrada auxiliar 2.									*/
	TRISD7=ENTRADA;		// Entrada auxiliar 1.									*/
/*..............................................................................*/
	TRISE0=SALIDA;		// Salida auxiliar 5.									*/
	TRISE1=ENTRADA;		// Entrada auxiliar 6.									*/
	TRISE2=ENTRADA;		// Entrada auxiliar 7.									*/
/********************************************************************************/
/*			TIMER 0 - UTILIZADO PARA TEMPORIZAR LA SEÑALES DE SALIDA.			*/
/*------------------------------------------------------------------------------*/
	T08BIT=0;			// Timer configurado en 16 bits.						*/
	T0CS=0;				// Oscilador interno.									*/
	T0SE=0;				// Flanco ascendente.									*/
	PSA=0;				// Asigno el preescaler al TMR0.						*/
	T0PS0=1;			// Configuro el preescaler a 1:256.						*/
	T0PS1=1;			//														*/
	T0PS2=1;			//														*/
	TMR0IF=0;			// Bajo la bandera de la interrupción.					*/
	TMR0ON=1;			// Timer apagdo.										*/
/********************************************************************************/
/*			TIMER 1 - BUZZER.													*/
/*..............................................................................*/
	TMR1=0;				// Configuro el tiempo que tarda en generar				*/
	T1RD16=1;			// Registro de 16 bits habilitado.						*/
	T1CKPS0=0; 			// Preescaler del TMR1.									*/
	T1CKPS1=0; 			//														*/
	T1OSCEN=0;			// Oscilador deshabilitado, uso Fosc/4.					*/
	T1SYNC=0;			// No sincronizo con clock externo.						*/
	TMR1CS=0;  			// Reloj interno Fosc/4.								*/
	TMR1ON=1;			// Prendo el TMR1.										*/
	TMR1IP=0;			// Prioridad de la interrupción del Timer 1.			*/
/********************************************************************************/
/*			TIMER 2 - TEMPORIZADOR PARA RETRANSMISIÓN DEL CAPITULO UNO.			*/
/*..............................................................................*/
	TMR2ON=1;			// Timer 2 apagado.										*/
	T2CKPS0=1;			// Configuro el Preescaler.								*/
	T2CKPS1=1;			// 														*/
	T2OUTPS0=1;			// Configuro el Postscaler.								*/
	T2OUTPS1=1;			//														*/
	T2OUTPS2=0;			//														*/
	T2OUTPS3=1;			//														*/
	TMR2IF=0;			// Bajo la bandera de la interrupción.					*/
/********************************************************************************/
/*			TIMER 3 - NO UTILIZADO.												*/
/*..............................................................................*/
	TMR3CS=0;			// Selección de la fuente de oscilación.				*/
	TMR3ON=0;			// Timer 3 apagado.										*/
	T3CKPS0=0;			// Configuro el Preescaler.								*/
	T3CKPS1=0;			// 														*/
	TMR2IF=0;			// Bajo la bandera de la interrupción.					*/
/********************************************************************************/
/*			Configuración de las interrupciones.								*/
/*..............................................................................*/
	IPEN=0;				// Deshabilito las prioridades para las int.			*/
	GIE=1;				// Utilizo interrupciones.								*/
	PEIE=1;				// Interrupcion externa habilitada.						*/
	INT0IE=0;			// Interrupcion RB0/INT deshabilitada.					*/
	INT1IE=0;			// Interrupcion RB1/INT deshabilitada.					*/
	INT2IE=0;			// Interrupcion RB2/INT deshabilitada.					*/
	INTEDG0=0;			// Interrupcion en el flanco descendente de RB0.		*/
	INTEDG1=0;			// Interrupcion en el flanco descendente de RB1.		*/	
 	INTEDG2=0;			// Interrupcion en el flanco descendente de RB2.		*/
	TMR0IE=0;			// Interrupcion desborde TMR0 deshabilitada.			*/
	TMR1IE=0;			// Interrupcion desborde TMR1 deshabilitada.			*/
	TMR2IE=1;			// Interrupcion desborde TMR2 habilitada.				*/
	TMR3IE=0;			// Interrupción desborde TMR3 deshabilitada.			*/
	CCP1IE=0;			// CCP1 Interrupt disable.								*/
	CCP2IE=0;			// CCP2 Interrupt disable.								*/
	CMIE=0;				// Interrupción del comparador deshabilitada.			*/
	EEIE=0;				// EEPROM Write Operation Interrupt disable.			*/
	SSPIE=0;			// Interrupcion por comunicacion I2C.					*/
	PSPIE=0;			// Slave Port Read/Write Interrupt disable.				*/
	BCLIE=0;			// Interrupción por colisión de bus deshabilitada.		*/
	ADIE=0;				// Interrupcion del conversor AD deshabilitada.			*/
	RBIE=0;				// Interrupcion por RB deshabilitada.					*/
	RCIE=0;				// Interrupcion recepcion USART habilitada.				*/
	OSCFIE=0;			// Interrupción por falla del oscilador deshabilitada.	*/
	HLVDIE=0;			// Interrupción por ALTA/BAJA TENSIÓN deshabilitada.	*/
	RBPU=1;				// RB pull-ups estan deshabilitadas.					*/
/********************************************************************************/
	Lcd_Setup();							// Inicializo el LCD.
	Imprimir_Lcd("CHECKING","inicializando...",1);
	I2C_Setup(400);							// Configuro la comunicacion I2C.
	Serial_Setup(9600);						// Setea el puerto serie.
	PutStr("\r\nCHEKING, inicializando...\r\n");
	LED_V=1;
	DelayS(1);								// Tiempo para que arranque el teclado.
	RS232_Habilitada();						// Prendo la recepción RS232.
	Reseteo_Teclado();						// Reinicio la cauptura por teclado PS2.
	bandera=1;								// Fuerzo ir al primer capitulo.
	for(;;)
	{
		CLRWDT();
		if(bandera)							// Se cumplió el tiempo?
		{
			LED_R=1;
			bandera=0;
			Comando(1);						// Repito la bienvenida.
			tiempo_r=Eeprom_Read(3);		// Recupero de la memória el tiempo de repetición.
			if(tiempo_r<1||tiempo_r>99)
				tiempo_r=TIEMPO_R;
			LED_R=0;
		}
		if(!RCIE)							// Entró una cadena por el RS232?
		{
			Entrada_Llavero();
			vis=pos_str_ps2;				// Actualizo el display.
		}
		Actualizo_Vis();
		if(tic)								// Aviso sonoro de la presión de una tecla?
		{
			Aviso(15,20);	
			tic=0;
		}
		if(pos_str_ps2==vis)				// Actualiazo la pantalla?
		{
			Lcd_Clear();
			Lcd_Goto(0);
			Lcd_Putch(':');					// Promp del sistema.
			Lcd_Puts(ps2_str);				// Muestro lo que se va escribiendo.
			vis++;							// 
		}
		if(pos_str_ps2>18)					// Límite de caracteres a escribir.
		{
			pos_str_ps2=18;
			vis=19;
		}
		if(!INT0IE)							// Presionaron ENTER?
		{
			RS232_OFF;						// Apago la recepción RS232.
			Interpreto_Comando();			// Ingresaron comando por teclado PS2.
			Reseteo_Teclado();				// Reinicio la cauptura por teclado PS2.
			RS232_Habilitada();				// Prendo la recepción RS232.
		}
	}
}
/********************************************************************************/
/*				Interpreto el comando leido por teclado.						*/
/*..............................................................................*/
void Interpreto_Comando(void)
{
	unsigned int i,vehiculo,e;
	unsigned char alma;
	i=0;
	if(!pos_str_ps2)						// La cadena no tiene nada?
	{
		Linea_Lcd(":",1);					// Muestro el prompt
		RS232_ON;							// Habilito la recepción RS232.
		return;
	}
	while(ps2_str[i])						// Convierto todo a mayusculas.
	{
		if(ps2_str[i]>96&&ps2_str[i]<123)
			ps2_str[i]=ps2_str[i]-32;
		i++;
	}
	if(!strcmp(ps2_str,"MENU"))				// Entro al menú.
	{
		TMR2_OFF;
		Reseteo_Teclado();							// Reinicio la cauptura por teclado PS2.
		while(INT0IE)								// Espero un enter del teclado
		{											// para procesar el comando.
			if(pos_str_ps2<vis-1)					// Se perdió la variable vis?
				vis=pos_str_ps2;
			if(pos_str_ps2==vis)					// Actulizo la pantalla?
			{
				Linea_Lcd("Ingrese contraseña:",1);	// Muestro lo que se va escribiendo.
				Lcd_Goto(0x40);
				i=0;
				while(i<pos_str_ps2&&pos_str_ps2)	// Escondo lo que se escribe.
				{
					i++;
					Lcd_Putch('*');					// Muestro en display *.
				}
				vis++;								// 
			}
			CLRWDT();
		}
		if(Check_Pass(ps2_str))
			Menu_Teclado();
		else
		{
			Imprimir_Lcd(":","Contraseña invalida.",1);
			actualizo=1;
		}
		bienvenida=0;
		bandera=1;							// Ir al primer capítulo.
		RS232_ON;
		TMR2_ON;							// Repeticion del msg de bienvenida.
		return;
	}
	Aviso(100,3);							// Aviso sonoro.
	Imprimir_Lcd(":","Comando inexistente.",1);
	actualizo=1;
	return;
}
/********************************************************************************/
/*			Alarma sonora para avisar que hubo una lectura.						*/
/*..............................................................................*/
void Aviso(unsigned int tiempo, unsigned char tono)
{
	tono_audible=tono;			// Tono del sonido generado.
	audio=tono_audible;
	duracion=tiempo*12;			// Tiempo que dura el sonido.
	TMR1_ON;					// Habilito la interrupcion del TMR1.
	return;
}
/********************************************************************************/
/*			Entró una tarjeta, muestro los mensajes correspodientes.			*/
/*..............................................................................*/
void Entrada_Llavero(void)
{
	unsigned int dire,repeticion;
	unsigned char lugar,tiempo;
	PS2_OFF;						// Deshabilito el teclado.
	LED_R=1;
	Aviso(300,2);					// Aviso sonoro de lectura.
	Imprimir_Lcd("Buscando tarjeta",rs_str,1);
	dire=Buscar_Tarjeta(rs_str);	// Busco en la memoria si la tarjeta está cargada.
	RS232_Habilitada();				// Habilito por si hay una nueva lectura.
	if(dire && RCIE)				// Encontré la tarjeta?
	{
		TMR2_OFF;					// Bajo la interrupción del reenvio.
		lugar=Read_24LC(dire+13);	// Cargo el número de capitulo correspondiente a la bienvenida.
		if(lugar && RCIE)
		{
			Imprimir_Lcd("Mostrando saludo","",1);
			Comando(lugar);			// Mensaje personalizado.
			tiempo=Eeprom_Read(1);	// Recupero de la memória el tiempo del msg.
			if(tiempo<1||tiempo>99)	// Excedo los límites de tiempo?
				tiempo=TIEMPO_S;	// Valor predeterminado.
			Pausa(tiempo);
		}
		lugar=Read_24LC(dire+14);	// Cargo el número de capitulo correspondiente a la mesa.
		if(lugar && RCIE)
		{
			Imprimir_Lcd("Mostrando mesa","",1);
			Comando(lugar);			// Mesa que le corresponde.
			tiempo=Eeprom_Read(2);	// Recupero de la memória el tiempo del msg.
			if(tiempo<1||tiempo>99)	// Excedo los límites de tiempo?
				tiempo=TIEMPO_M;	// Valor predeterminado.
			Pausa(tiempo);
		}
	}
	LED_R=0;
	if(!RCIE)						// Entró una nueva cadena?
		return;
	bienvenida=0;
	bandera=1;
	TMR2_ON;						// Habilito el timer para calcular el tiempo de repetición.
	Reseteo_Teclado();				// Reinicio la cauptura por teclado PS2.
	return;
}
/********************************************************************************/
/*		Equivale al DelayS, pero sigue atendiendo a la interrupción por RS232.	*/
/*..............................................................................*/
///////////////////////////////////////////////////////////////////////////////////////////
void Pausa(unsigned char segundos)
{
	bandera=0;
	bienvenida=0;					// Reseteo el contador.
	tiempo_r=segundos;
	TMR2_ON;						// Bajo la bandera de la interrupción.
	while(!bandera && RCIE)			// Espero que se cumpla el tiempo.
		CLRWDT();
	TMR2_OFF;						// Apago el timer.
	return;	
}
/********************************************************************************/
/*		Actualizo el display.													*/
/*..............................................................................*/
void Actualizo_Vis(void)
{
	if((pos_str_ps2<vis-1)||(pos_str_ps2>vis+1))	// Se perdió la variable vis?
		vis=pos_str_ps2;
	if(actualizo)
	{
		vis=pos_str_ps2+1;
		actualizo=0;
	}
	return;	
}
