/********************************************************************************/
/*			RUTINAS PARA INTERPRETAR UN TECLADO ESTANDAR PS2 DE PC.				*/
/*..............................................................................*/
/*			Revisión:				1.00										*/
/*			PIC:					PIC18F4620									*/
/*			Teclado:				PS2											*/
/*			Compilador:				MPLAB IDE 8.7 - HI-TECH PIC18 9.50 PL3.		*/
/*			Autor:					Mariano Ariel Deville						*/
/********************************************************************************/
/*						ARCHIVOS CABECERA										*/
/*------------------------------------------------------------------------------*/
#include	"Teclado.h"				// Tabla para interpretar teclado PS2.
/********************************************************************************/
/*		LLENADO DE CARATERES EN UNA CADENA RECIBIDOS DESDE EL TECLADO.			*/
/********************************************************************************/
void Interpretar_Teclado(void)
{											// entonces hago la conversión.
	pos_ps2=11;								// Necesario para la correcta captura de teclas.
	if(lect_ps2==0xf0)						// Solte la tecla que presionaba.
	{
		suelto_tecla_ps2=1;					// Levanto la bandera.
		return;								// Salgo de la interrupción.
	}
	if(suelto_tecla_ps2)					// Omito el caracter de la tecla que se solto.
	{
		suelto_tecla_ps2=0;					// Bajo la bandera.
		if(lect_ps2==0x12||lect_ps2==0x59)
			shift_ps2=0;					// Bajo la bandera del SHIFT.
		if(lect_ps2==0x5a)					// Si es un final de linea proceso el comando.
			PS2_OFF;						// Deshabilito la interrupción.
		return;								// Salgo de la interrupción.
	}
	tic=1;
	if(lect_ps2==0x66||(lect_ps2==0x71&&ctrl_ps2))	// Se presiono DEL O DELETE???
	{
		if(pos_str_ps2)						// Si 'pos' es distinto de cero
			pos_str_ps2--;					// vuelvo un lugar.
		ps2_str[pos_str_ps2]=0;				// Borro el contenido del lugar.
		ctrl_ps2=0;							// Bajo la bandera de control.
		return;								// Salgo de la interrupción.
	}
	if(lect_ps2==0xe0)						// Presionaron una tecla de control.
	{
		ctrl_ps2=1;							// Levanto la bandera de control.
		return;								// Salgo de la interrupción.
	}
	if(lect_ps2==0x58)						// Presionaron la tecla mayusculas.
	{
		mayus_ps2=!mayus_ps2;				// Cambio el balor de la bandera MAYUSCULAS.
		return;								// Salgo de la interrupción.
	}
	if(lect_ps2==0x05)						// Presionaron F1.
	{
		if(consola)
		{
			pos_str_ps2=6;					// Apunto al primer lugar de la cadena.
			vis=pos_str_ps2;				// Actualizo el display.
			ps2_str[0]='C';					//
			ps2_str[1]='A';					//
			ps2_str[2]='R';					//
			ps2_str[3]='G';					//
			ps2_str[4]='A';					//
			ps2_str[5]='R';					//
			ps2_str[pos_str_ps2]=0;			// Marco el final de la cadena.
		}
		else
		{
			pos_str_ps2=4;					// Apunto al primer lugar de la cadena.
			vis=pos_str_ps2;				// Actualizo el display.
			ps2_str[0]='M';					//
			ps2_str[1]='E';					//
			ps2_str[2]='N';					//
			ps2_str[3]='U';					//
			ps2_str[pos_str_ps2]=0;			// Marco el final de la cadena.
		}
		return;								// Salgo de la interrupción.
	}
	if(lect_ps2==0x06 && consola)			// Presionaron F2.
	{
		pos_str_ps2=6;						// Apunto al primer lugar de la cadena.
		vis=pos_str_ps2;					// Actualizo el display.
		ps2_str[0]='E';						//
		ps2_str[1]='D';						//
		ps2_str[2]='I';						//
		ps2_str[3]='T';						//
		ps2_str[4]='A';						//
		ps2_str[5]='R';						//
		ps2_str[pos_str_ps2]=0;				// Marco el final de la cadena.
		return;								// Salgo de la interrupción.
	}
	if(lect_ps2==0x04 && consola)			// Presionaron F3.
	{
		pos_str_ps2=6;						// Apunto al primer lugar de la cadena.
		vis=pos_str_ps2;					// Actualizo el display.
		ps2_str[0]='L';						//
		ps2_str[1]='I';						//
		ps2_str[2]='S';						//
		ps2_str[3]='T';						//
		ps2_str[4]='A';						//
		ps2_str[5]='R';						//
		ps2_str[pos_str_ps2]=0;				// Marco el final de la cadena.
		return;								// Salgo de la interrupción.
	}
	if(lect_ps2==0x0c && consola)			// Presionaron F4.
	{
		pos_str_ps2=6;						// Apunto al primer lugar de la cadena.
		vis=pos_str_ps2;					// Actualizo el display.
		ps2_str[0]='B';						//
		ps2_str[1]='O';						//
		ps2_str[2]='R';						//
		ps2_str[3]='R';						//
		ps2_str[4]='A';						//
		ps2_str[5]='R';						//
		ps2_str[pos_str_ps2]=0;				// Marco el final de la cadena.
		return;								// Salgo de la interrupción.
	}
	if(lect_ps2==0x03 && consola)			// Presionaron F5.
	{
		pos_str_ps2=9;						// Apunto al primer lugar de la cadena.
		vis=pos_str_ps2;					// Actualizo el display.
		ps2_str[0]='D';						//
		ps2_str[1]='E';						//
		ps2_str[2]='S';						//
		ps2_str[3]='C';						//
		ps2_str[4]='A';						//
		ps2_str[5]='R';						//
		ps2_str[6]='G';						//
		ps2_str[7]='A';						//
		ps2_str[8]='R';						//
		ps2_str[pos_str_ps2]=0;				// Marco el final de la cadena.
		return;								// Salgo de la interrupción.
	}
	if(lect_ps2==0x0b && consola)			// Presionaron F6.
	{
		pos_str_ps2=12;						// Apunto al primer lugar de la cadena.
		vis=pos_str_ps2;					// Actualizo el display.
		ps2_str[0]='L';						//
		ps2_str[1]='E';						//
		ps2_str[2]='E';						//
		ps2_str[3]='R';						//
		ps2_str[4]=' ';						//
		ps2_str[5]='T';						//
		ps2_str[6]='A';						//
		ps2_str[7]='R';						//
		ps2_str[8]='J';						//
		ps2_str[9]='E';						//
		ps2_str[10]='T';					//
		ps2_str[11]='A';					//
		ps2_str[pos_str_ps2]=0;				// Marco el final de la cadena.
		return;								// Salgo de la interrupción.
	}
	if(lect_ps2==0x83 && consola)			// Presionaron F7.
	{
		pos_str_ps2=9;						// Apunto al primer lugar de la cadena.
		vis=pos_str_ps2;					// Actualizo el display.
		ps2_str[0]='C';						//
		ps2_str[1]='A';						//
		ps2_str[2]='N';						//
		ps2_str[3]='T';						//
		ps2_str[4]='I';						//
		ps2_str[5]='D';						//
		ps2_str[6]='A';						//
		ps2_str[7]='D';						//
		ps2_str[8]='?';						//
		ps2_str[pos_str_ps2]=0;				// Marco el final de la cadena.
		return;	
	}
	if(lect_ps2==0x0a && consola)			// Presionaron F8.
	{
		pos_str_ps2=7;						// Apunto al primer lugar de la cadena.
		vis=pos_str_ps2;					// Actualizo el display.
		ps2_str[0]='B';						//
		ps2_str[1]='A';						//
		ps2_str[2]='N';						//
		ps2_str[3]='D';						//
		ps2_str[4]='E';						//
		ps2_str[5]='J';						//
		ps2_str[6]='A';						//
		ps2_str[pos_str_ps2]=0;				// Marco el final de la cadena.
		return;	
	}
	if(lect_ps2==0x01 && consola)			// Presionaron F9.
	{
		pos_str_ps2=5;						// Apunto al primer lugar de la cadena.
		vis=pos_str_ps2;					// Actualizo el display.
		ps2_str[0]='P';						//
		ps2_str[1]='O';						//
		ps2_str[2]='W';						//
		ps2_str[3]='E';						//
		ps2_str[4]='R';						//
		ps2_str[pos_str_ps2]=0;				// Marco el final de la cadena.
		return;	
	}
	if(lect_ps2==0x09 && consola)			// Presionaron F10.
	{
		pos_str_ps2=4;						// Apunto al primer lugar de la cadena.
		vis=pos_str_ps2;					// Actualizo el display.
		ps2_str[0]='P';						//
		ps2_str[1]='L';						//
		ps2_str[2]='A';						//
		ps2_str[3]='Y';						//
		ps2_str[pos_str_ps2]=0;				// Marco el final de la cadena.
		return;	
	}
	if(lect_ps2==0x78 && consola)			// Presionaron F11.
	{
		pos_str_ps2=4;						// Apunto al primer lugar de la cadena.
		vis=pos_str_ps2;					// Actualizo el display.
		ps2_str[0]='S';						//
		ps2_str[1]='T';						//
		ps2_str[2]='O';						//
		ps2_str[3]='P';						//
		ps2_str[pos_str_ps2]=0;				// Marco el final de la cadena.
		return;	
	}
	if(lect_ps2==0x07 && consola)			// Presionaron F12.
	{
		pos_str_ps2=4;						// Apunto al primer lugar de la cadena.
		vis=pos_str_ps2;					// Actualizo el display.
		ps2_str[0]='N';						//
		ps2_str[1]='E';						//
		ps2_str[2]='X';						//
		ps2_str[3]='T';						//
		ps2_str[pos_str_ps2]=0;				// Marco el final de la cadena.
		return;	
	}
	if(lect_ps2==0x76)						// Presionaron la tecla ESC borro la linea.
	{
		pos_str_ps2=0;						// Apunto al primer lugar de la cadena.
		ps2_str[0]=0;						// Marco el final de la cadena.
		vis=0;
		return;								// Salgo de la interrupción.
	}
	if(lect_ps2==0x12||lect_ps2==0x59)		// Presionaron SHIFT
	{
		shift_ps2=1;						// Levanto la bandera del SHIFT.
		return;								// Salgo de la interrupción.
	}
	if(lect_ps2==0x5a)						// Si es un final de linea proceso el comando.
	{
		ps2_str[pos_str_ps2]=0;				// Marco el final de la cadena.
		return;								// Salgo de la interrupción.
	}
	cont_ps2=0;
	do										// Recorro la tabla en busca del ASCII.
	{
		CLRWDT();
		if(lect_ps2==ps2[cont_ps2][0])		// Comparo el codigo leido con la tabla.
		{
			if(!mayus_ps2&&!shift_ps2)		// Estaba presionada la tecla MAYUSCULA???
			{
				ps2_str[pos_str_ps2]=ps2[cont_ps2][1];
				break;
			}
			if(mayus_ps2&&!shift_ps2)		// Estaba presionada la tecla SHIFT???
			{
				ps2_str[pos_str_ps2]=ps2[cont_ps2][2];
				break;
			}
			if(!mayus_ps2&&shift_ps2)		// Estaba presionada la tecla MAYUSCULA y SHIFT???
			{
				ps2_str[pos_str_ps2]=ps2[cont_ps2][3];
				break;
			}
			if(mayus_ps2&&shift_ps2)		// Estaba presionada la tecla MAYUSCULA y SHIFT???
				ps2_str[pos_str_ps2]=ps2[cont_ps2][4];
			break;							// Una vez encontrado salgo del bucle.
		}
	}while(++cont_ps2<64);					// Busco hasta el último caracter de la tabla.
	if(pos_str_ps2<33)						// Limito a 30 caracteres el comando.
		pos_str_ps2++;						// Voy al siguiente lugar en la cadena.
	ps2_str[pos_str_ps2]=0;					// Marco el final de la cadena
	return;									// Salgo de la interrupción.
}
/********************************************************************************/
/*										*/
/*------------------------------------------------------------------------------*/
void Reseteo_Teclado(void)
{
	DelayMs(20);	// Anti rebote.
	ps2_str[0]=0;	// Reseteo la cadena.
	pos_str_ps2=0;	// Posicion inicial de la cadena de caracteres.
	vis=0;			// Variable relacionada con la visualización.
	tic=0;			// Reseteo el sonido.
	lect_ps2=0;		// Vacío la variable lectura del teclado PS2.
	pos_ps2=11;		// Necesario para la correcta captura de teclas.
	mayus_ps2=1;	// Comienzo escribiendo con mayusculas.
	shift_ps2=0;	// Reseteo el SHIFT.
	PS2_ON;			// Habilito la interrupción del teclado.
	return;
}
