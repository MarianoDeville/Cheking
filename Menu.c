/********************************************************************************/
/*			MENU AUXILIAR  - CARGA Y MANEJO DE LOS DATOS EN LA MEMORIA.			*/
/*..............................................................................*/
/*			Revisión:			1.00											*/
/*			PIC:				PIC18F4620										*/
/*			Memoria:			24LC256.										*/
/*			Display:			LCD 20 caracteres por 2 lineas.					*/
/*			Teclado:			PS2												*/
/*			Comunicación:		RS232 - I2C.									*/
/*			Compilador:			MPLAB IDE 8.70 - HI-TECH PICC-18 9.50 PL3.		*/
/*			Autor:				Mariano Ariel Deville							*/
/********************************************************************************/
/*						PROTOTIPOS DE FUNCIONES									*/
/*..............................................................................*/
void Menu_Teclado(void);
void Comandos(void);
void Borrado_Completo(void);
void Prompt(const unsigned char *mensaje);
void Descargo_Memoria(void);
unsigned char AsciiToInt(void);
void IntToStr(unsigned int origen,register unsigned char *destino);
void RS232_Habilitada(void);
void Leer_RS_PS2(void);
unsigned char Check_Pass(unsigned char *pass);
/********************************************************************************/
/*						VARIABLES GLOBALES										*/
/*..............................................................................*/
bank2 bit consola;						// Bandera.								*/
/********************************************************************************/
/*		CAPTURA DE TECLADO Y VISUALIZACION EN DISPLAY							*/
/*..............................................................................*/
void Menu_Teclado(void)
{
	consola=1;
	LED_V=0;
	LED_R=1;
	Reseteo_Teclado();
	while(consola)						// Mientras este sigo en el menu.
	{
		while(INT0IE)					// Espero un enter del teclado
		{								// para procesar el comando.
			if(pos_str_ps2<vis-1)		// Se perdió la variable vis?
				vis=pos_str_ps2;
			if(pos_str_ps2==vis)		// Actualizo la pantalla?
			{
				Prompt("");
				Lcd_Puts(ps2_str);		// Muestro lo que se va escribiendo.
				vis++;					// 
			}
			CLRWDT();
		}
		Comandos();						// Busco y ejecuto el comando ingresado.
		if(consola)
		{
			Reseteo_Teclado();
			while(!pos_str_ps2&&INT0IE)	// Espero a que se teclee algo.
				CLRWDT();
			vis=pos_str_ps2;
		}
	}
	LED_R=0;
	LED_V=1;
	return;
}
/********************************************************************************/
/*									COMANDOS 									*/
/*------------------------------------------------------------------------------*/
/*	CAMBIAR CONTRASEÑA	- Cambio de la contraseña para entrar al menu.			*/
/*	BANDEJA				- Comando para abrir/cerrar la bandeja del reproductor.	*/
/*	POWER				- Prendo/apago el reproductor.							*/
/*	PLAY				- Comienzo la reproducción del DVD.						*/
/*	STOP				- Paro la reproducción del DVD.							*/
/*	ENTER				- Comando ENTER del reproductor de DVD.					*/
/*	NEXT				- Comando NEXT del reproductor de DVD.					*/
/*	CANTIDAD?			- Cantidad de tarjetas cargadas en memoria.				*/
/*	LEER TARJETA		- Leo una tarjeta RFID y muestro por display el número.	*/
/*	LISTAR				- Muestro en dislplay las tarjetas cargadas en memoria.	*/
/*	CARGAR				- Cargo una nueva tarjeta								*/
/*	DESCARGAR			- Descargo la info por RS232 en la PC.					*/
/*	RS232 CRUDO			- Descargo la EEPROM tal cual está por RS232.			*/
/*	EDITAR				- Busco una tarjeta y cambio la ubicación y el saludo.	*/
/*	BORRAR				- Me permite borrar una tarjeta.						*/
/*	BORRADO COMPLETO	- Borro todo el contenido de la memoria.				*/
/*	TIEMPO SALUDO		- Tiempo que dura el saludo (de 0 a 99 seg.).			*/
/*	TIEMPO MESA			- Tiempo que dura el msj. de la mesa (de 0 a 99 seg.).	*/
/*	TIEMPO REPETICION	- Tiempo que se reproduce el capitulo 1.				*/
/*	SALIR				- Salgo del menú ROOT.									*/
/********************************************************************************/
void Comandos(void)
{
	static bit bis;
	volatile unsigned int pag,e;
	unsigned char i,cad_temp[40];
	CLRWDT();
	bis=0;
	if(!pos_str_ps2)
	{
		Prompt("");
		return;
	}
/*..............................................................................*/
/*	CAMBIAR CONTRASEÑA - Cambio de la contraseña para entrar al menu.			*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"CAMBIAR CONTRASEÑA"))
	{
		Reseteo_Teclado();
		while(INT0IE)								// Espero un enter del teclado
		{											// para procesar el comando.
			if(pos_str_ps2<vis-1)					// Se perdió la variable vis?
				vis=pos_str_ps2;
			if(pos_str_ps2==vis)					// Actulizo la pantalla?
			{
				Linea_Lcd("contraseña vieja:",1);	// Muestro lo que se va escribiendo.
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
		if(!Check_Pass(ps2_str))					// Ingresaron bien la contraseña anterior?
		{
			Prompt("INCORRECTA");
			return;
		}
		Reseteo_Teclado();
		while(INT0IE)								// Espero un enter del teclado
		{											// para procesar el comando.
			if(pos_str_ps2<vis-1)					// Se perdió la variable vis?
				vis=pos_str_ps2;
			if(pos_str_ps2==vis)					// Actulizo la pantalla?
			{
				Linea_Lcd("contraseña nueva:",1);	// Muestro lo que se va escribiendo.
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
		strcpy(rs_str,ps2_str);
		Reseteo_Teclado();
		while(INT0IE)								// Espero un enter del teclado
		{											// para procesar el comando.
			if(pos_str_ps2<vis-1)					// Se perdió la variable vis?
				vis=pos_str_ps2;
			if(pos_str_ps2==vis)					// Actulizo la pantalla?
			{
				Linea_Lcd("repita la contraseña",1);	// Muestro lo que se va escribiendo.
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
		if(strcmp(rs_str,ps2_str))
		{
			Prompt("No coinciden.");
			return;
		}
		Guardo_Contrasena(ps2_str);
		Prompt("Contraseña guardada.");
		return;
	}
/*..............................................................................*/
/*	BANDEJA - Comando para abrir/cerrar la bandeja del reproductor.				*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"BANDEJA"))
	{
		Transmitir_IR(0,OPEN);			// Botón OPEN
		Prompt("");
		return;
	}
/*..............................................................................*/
/*	POWER - Prendo/apago el reproductor.										*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"POWER"))
	{
		Transmitir_IR(0,POWER);			// Botón POWER.
		Prompt("");
		return;
	}
/*..............................................................................*/
/*	PLAY - Comienzo la reproducción del DVD.									*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"PLAY"))
	{
		Transmitir_IR(0,PLAY);			// Botón PLAY.
		Prompt("");
		return;
	}
/*..............................................................................*/
/*	STOP - Paro la reproducción del DVD.										*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"STOP"))
	{
		Transmitir_IR(0,STOP);			// Botón STOP.
		Prompt("");
		return;
	}
/*..............................................................................*/
/*	ENTER - Comando ENTER del reproductor de DVD.								*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"ENTER"))
	{
		Transmitir_IR(0,ENTER);			// Botón ENTER.
		Prompt("");
		return;
	}
/*..............................................................................*/
/*	NEXT - Comando NEXT del reproductor de DVD.									*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"NEXT"))
	{
		Transmitir_IR(0,NEXT);			// Botón NEXT.
		Prompt("");
		return;
	}
/*..............................................................................*/
/*	CANTIDAD - Muestro en pantalla la cantidad de tarjetas cargadas.			*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"CANTIDAD?"))
	{
		e=0;
		Imprimir_Lcd("Contando tarjetas","cargadas",1);
		for(pag=0;pag<2048;pag++)
		{
			if(Read_24LC(pag*16)=='#')
				e++;
		}
		IntToStr(e,cad_temp);
		Imprimir_Lcd("Cantidad de tarjetas",cad_temp,1);
		return;
	}
/*..............................................................................*/
/*	LEER TARJETA - Leo una tarjeta RFID y muestro por display el número.		*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"LEER TARJETA"))
	{
		Imprimir_Lcd("Acerque la tarjeta","",1);
		RS232_Habilitada();						// Habilito la recepción por RS232.
		Reseteo_Teclado();
		while(RCIE)								// Espero que pasen una tarjeta.
		{
			CLRWDT();
			if(!INT0IE)							// Espero un enter del teclado.
			{
				RS232_OFF;
				PS2_OFF;
				Prompt("");
				return;
			}
		}
		RS232_OFF;								// Apago la recepción RS232.
		PS2_OFF;								// Apago la interrupción del teclado.
		Imprimir_Lcd("Tarjeta nro.:",rs_str,1);	// Muestro lo que se escribió.
		PutStr(rs_str);							// Mando por RS232 el número leido.
		return;
	}
/*..............................................................................*/
/*	LISTAR - Muestro en dislplay las tarjetas cargadas en memoria.				*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"LISTAR"))
	{
		pag=1;
		rs_str[0]=0;
		Reseteo_Teclado();
		while(INT0IE)							// Espero un enter del teclado
		{										// para procesar el comando.
			if(pos_str_ps2==vis)				// Actulizo la pantalla?
			{
				pos_str_ps2=0;
				vis=1;							// Solo cuando se presione una tecla avanzo.
				Rec_Paquete_24LC(pag,ps2_str);
				if(ps2_str[0])					// Es un dato válido?
				{
					e=Read_24LC((pag*16)+13);
					i=strlen(ps2_str);
					ps2_str[i++]=' ';
					IntToStr(e,cad_temp);
					if(!cad_temp[2])
						ps2_str[i++]='0';
					ps2_str[i++]=cad_temp[0];
					ps2_str[i++]=cad_temp[1];
					if(cad_temp[2])
						ps2_str[i++]=cad_temp[2];
					e=Read_24LC((pag*16)+14);
					ps2_str[i++]=' ';
					IntToStr(e,cad_temp);
					if(!cad_temp[2])
						ps2_str[i++]='0';
					ps2_str[i++]=cad_temp[0];
					ps2_str[i++]=cad_temp[1];
					if(cad_temp[2])
						ps2_str[i++]=cad_temp[2];
					ps2_str[i]=0;					// Final de la cadena
					Imprimir_Lcd(rs_str,ps2_str,1);	// Muestro lo que voy recuperando de la memoria.
					strcpy(rs_str,ps2_str);			// Copio el dato viejo.
				}
				else
					pos_str_ps2=vis;			// Lugar sin datos, busco el siguiente.
				if(pag++>2046)					// Llegué al final de la memoria?
				{
					PS2_OFF;
					Imprimir_Lcd(rs_str,"Fin de la lista",1);
					return;
				}
			}
			CLRWDT();
		}
		Prompt("");
		return;
	}
/*..............................................................................*/
/*	CARGAR - Cargo una nueva tarjeta.											*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"CARGAR"))
	{
		Leer_RS_PS2();							// Leo lo que entra pos PS2 y por RS232 y lo almaceno en ps2_str.
		RS232_OFF;
		pag=Alma_Paquete_24LC(ps2_str);			// Página en la que está almacenado el dato.
		if(!pag)
		{
			Prompt("Error al cargar.");
			return;
		}
		Reseteo_Teclado();
		while(INT0IE)							// Espero un enter del teclado
		{										// para procesar el comando.
			if(pos_str_ps2<vis-1)				// Se perdió la variable vis?
				vis=pos_str_ps2;
			if(pos_str_ps2==vis)				// Actulizo la pantalla?
			{
				Imprimir_Lcd("Nro. saludo 1 - 250:",ps2_str,1);	// Muestro lo que se va escribiendo.
				vis++;							// 
			}
			CLRWDT();
		}
		i=AsciiToInt();
		Write_24LC(pag+13,i);
		Reseteo_Teclado();
		while(INT0IE)							// Espero un enter del teclado
		{										// para procesar el comando.
			if(pos_str_ps2<vis-1)				// Se perdió la variable vis?
				vis=pos_str_ps2;
			if(pos_str_ps2==vis)				// Actulizo la pantalla?
			{
				Imprimir_Lcd("Nro. mesa 1 - 250:",ps2_str,1);	// Muestro lo que se va escribiendo.
				vis++;							// 
			}
			CLRWDT();
		}
		i=AsciiToInt();
		Write_24LC(pag+14,i);
		Write_24LC(pag+15,0);
		Prompt("Tarjeta cargada.");
		return;
	}
/*..............................................................................*/
/*	DESCARGAR - Descargo la info por RS232 en la PC.							*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"DESCARGAR"))
	{
		Descargo_Memoria();
		Prompt("");
		return;
	}
/*..............................................................................*/
/*	RS232 CRUDO - Descargo la EEPROM tal cual está por RS232.					*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"RS232 CRUDO"))
	{
		for(e=0;e<32766;e++)
		{
			i=Read_24LC(e);
			PutCh(i);
		}
		Prompt("");
		return;
	}
/*..............................................................................*/
/*	EDITAR - Busco una tarjeta y cambio la ubicación y el saludo.				*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"EDITAR"))				// Solo el ROOt lo puede hacer.
	{
		Leer_RS_PS2();							// Leo lo que entra pos PS2 y por RS232 y lo almaceno en ps2_str.
		pag=Buscar_Tarjeta(ps2_str);			// busco en la memoria si la tarjeta está cargada.
		if(!pag)
		{
			Imprimir_Lcd("M: Tarjeta","no encontrada.",1);
			return;
		}
		Reseteo_Teclado();
		while(INT0IE)							// Espero un enter del teclado
		{										// para procesar el comando.
			if(pos_str_ps2<vis-1)				// Se perdió la variable vis?
				vis=pos_str_ps2;
			if(pos_str_ps2==vis)				// Actulizo la pantalla?
			{
				Imprimir_Lcd("Nro. saludo 1 - 250:",ps2_str,1);	// Muestro lo que se va escribiendo.
				vis++;							// 
			}
			CLRWDT();
		}
		i=AsciiToInt();
		Write_24LC(pag+13,i);
		Reseteo_Teclado();
		while(INT0IE)							// Espero un enter del teclado
		{										// para procesar el comando.
			if(pos_str_ps2<vis-1)				// Se perdió la variable vis?
				vis=pos_str_ps2;
			if(pos_str_ps2==vis)				// Actulizo la pantalla?
			{
				Imprimir_Lcd("Nro. mesa 1 - 250:",ps2_str,1);	// Muestro lo que se va escribiendo.
				vis++;							// 
			}
			CLRWDT();
		}
		i=AsciiToInt();
		Write_24LC(pag+14,i);
		Write_24LC(pag+15,0);
		Prompt("Tarjeta editada.");
		return;
	}
/*..............................................................................*/
/*	BORRAR - Me permite borrar una tarjeta.										*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"BORRAR"))
	{
		Leer_RS_PS2();							// Leo lo que entra pos PS2 y por RS232 y lo almaceno en ps2_str.
		pag=Buscar_Tarjeta(ps2_str);			// busco en la memoria si la tarjeta está cargada.
		if(!pag)
		{
			Imprimir_Lcd("M: Tarjeta","no encontrada.",1);
			return;
		}
		Imprimir_Lcd("Desea borrar:",ps2_str,1);
		Reseteo_Teclado();						// Reinicio la cauptura por teclado PS2.
		while(INT0IE)							// Espero que defina si borro.
		{
			if(ps2_str[0]=='S'||ps2_str[0]=='N')
				PS2_OFF;						// Deshabilito la interrupción del teclado.
			CLRWDT();
		}
		if(ps2_str[0]=='S')
		{
			Borrar_24LC256(pag);
			Prompt("Tarjeta borrada");
		}
		else
			Prompt("Operacion cancelada");
		return;
	}
/*..............................................................................*/
/*	BORRADO COMPLETO - Borro todo el contenido de la memoria.					*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"BORRADO COMPLETO"))
	{
		Prompt("");
		PutStr("\r\nBorrando memoria EEPROM...\r\n");
		Borrado_Completo();
		PutStr("Memoria borrada en su totalidad.\r\n");
		Prompt("Memoria borrada");
		return;
	}
/*..............................................................................*/
/*	TIEMPO SALUDO - Tiempo que dura el saludo (de 0 a 99 seg.).					*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"TIEMPO SALUDO"))
	{
		Reseteo_Teclado();
		ps2_str[2]=0;
		while(INT0IE)							// Espero un enter del teclado
		{										// para procesar el comando.
			if(pos_str_ps2<vis-1)				// Se perdió la variable vis?
				vis=pos_str_ps2;
			if(pos_str_ps2==vis)				// Actulizo la pantalla?
			{
				if(ps2_str[2])					// Escribí más de dos caracteres?
				{
					ps2_str[1]=ps2_str[2];
					ps2_str[2]=0;
					pos_str_ps2=1;
					vis=1;
				}
				Imprimir_Lcd("Tiempo saludo 1-99:",ps2_str,1);	// Muestro lo que se va escribiendo.
				Lcd_Goto(0x43);
				Lcd_Puts("seg.");
				vis++;							// 
			}
			CLRWDT();
		}
		i=AsciiToInt();							// Convierto lo escrito en un número.
		Eeprom_Write(1,i);						// Guardo el valor en la memoria.
		Prompt("Tiempo configurado");
		return;
	}
/*..............................................................................*/
/*	TIEMPO MESA - Tiempo que dura el msj. de la mesa (de 0 a 99 seg.).			*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"TIEMPO MESA"))
	{
		Reseteo_Teclado();
		ps2_str[2]=0;
		while(INT0IE)							// Espero un enter del teclado
		{										// para procesar el comando.
			if(pos_str_ps2<vis-1)				// Se perdió la variable vis?
				vis=pos_str_ps2;
			if(pos_str_ps2==vis)				// Actulizo la pantalla?
			{
				if(ps2_str[2])					// Escribí más de dos caracteres?
				{
					ps2_str[1]=ps2_str[2];
					ps2_str[2]=0;
					pos_str_ps2=1;
					vis=1;
				}
				Imprimir_Lcd("Tiempo mesa 1-99:",ps2_str,1);	// Muestro lo que se va escribiendo.
				Lcd_Goto(0x43);
				Lcd_Puts("seg.");
				vis++;							// 
			}
			CLRWDT();
		}
		i=AsciiToInt();							// Convierto lo escrito en un número.
		Eeprom_Write(2,i);						// Guardo el valor en la memoria.
		Prompt("Tiempo configurado");
		return;	
	}
/*..............................................................................*/
/*	TIEMPO REPETICION - Tiempo que se reproduce el capitulo 1.					*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"TIEMPO REPETICION"))
	{
		Reseteo_Teclado();
		ps2_str[2]=0;
		while(INT0IE)							// Espero un enter del teclado
		{										// para procesar el comando.
			if(pos_str_ps2<vis-1)				// Se perdió la variable vis?
				vis=pos_str_ps2;
			if(pos_str_ps2==vis)				// Actulizo la pantalla?
			{
				if(ps2_str[2])					// Escribí más de dos caracteres?
				{
					ps2_str[1]=ps2_str[2];
					ps2_str[2]=0;
					pos_str_ps2=1;
					vis=1;
				}
				Imprimir_Lcd("Tiempo rep. 1-99:",ps2_str,1);	// Muestro lo que se va escribiendo.
				Lcd_Goto(0x43);
				Lcd_Puts("seg.");
				vis++;							// 
			}
			CLRWDT();
		}
		i=AsciiToInt();							// Convierto lo escrito en un número.
		Eeprom_Write(3,i);						// Guardo el valor en la memoria.
		Prompt("Tiempo configurado");
		return;	
	}
/*..............................................................................*/
/*	SALIR - Salgo del menú ROOT.												*/
/*..............................................................................*/
	if(!strcmp(ps2_str,"SALIR"))
	{
		consola=0;
		Imprimir_Lcd(":","",1);
		return;
	}
	Prompt("Comando inexistente.");				// Si llegue hasta aca es porque no existe el comando.
	return;
}
/********************************************************************************/
/*				Elimino todo el contenido de la memoria externa.				*/
/*..............................................................................*/
void Borrado_Completo(void)
{
	volatile unsigned int i,porcent,comparar=200;
	unsigned char cadena[3];
	Imprimir_Lcd("Borrando memoria","   %",1);
	for(i=0;i<32767;i++)
	{
		Write_24LC(i,0);					// Escribo un cero (vacio).
		porcent=i/327;
		if(comparar!=porcent)				// Debo actualizar el porcentaje?
		{
			comparar=porcent;
			IntToStr(comparar,cadena);
			Lcd_Goto(0x40);
			Lcd_Puts(cadena);
		}
	}
	return;
}
/********************************************************************************/
/*				Muestro en display el promp.									*/
/*..............................................................................*/
void Prompt(const unsigned char *mensaje)
{
	Lcd_Clear();		// Limpio el display.
	Lcd_Goto(0);		// Voy a la primer linea, primer lugar.
	Lcd_Puts("#:");		// Identifico al ROOT.
	if(*mensaje)		// Hay algo más para mostrar?
	{
		Lcd_Goto(0x40);	// Voy al primer lugar de la segunda linea.
		Lcd_Puts(mensaje);
	}
	return;
}
/********************************************************************************/
/*				Bajo el contenido de la memoria por RS232.						*/
/*..............................................................................*/
void Descargo_Memoria(void)
{
	volatile unsigned int i,e,a,porcent,comparar=20;
	unsigned char cadena[5],cambiar;
	Imprimir_Lcd("Descargando memoria","   %",1);
	PutStr("\r\nDescargando\r\n");
	cambiar=0;
	bandera=0;
	a=0;
	for(i=0;i<32767;i++)
	{
		e=Read_24LC(i);
		if(e=='#' && !cambiar)
		{
			bandera=1;
			IntToStr(++a,cadena);
			PutStr(cadena);					// Cadena correspondiente al número de orden.
			PutCh(' ');						// Dejo un espacio.
		}
		if(bandera && e && cambiar)
		{
			if(cambiar>12)					// Saludo personal, mesa y otro.
			{
				IntToStr(e,cadena);			// Convierto en cadena el número.
				PutCh(' ');					// Dejo un espacio.
				PutStr(cadena);				// Cadena correspondiente al número leido.
			}
			else
				PutCh(e);
		}
		if(cambiar>12 && !e && bandera)
			PutStr(" 00");
		if(cambiar++==15)					// Llegué al final de la linea?
		{
			if(bandera)
				PutStr("\r\n");				// Retorno de carro y fin de linea.
			cambiar=0;
			bandera=0;
		}
		porcent=i/327;
		if(comparar!=porcent)				// Debo actualizar el porcentaje?
		{
			comparar=porcent;
			IntToStr(comparar,cadena);
			Lcd_Goto(0x40);
			Lcd_Puts(cadena);
		}
	}
	PutStr("\r\n");
	return;
}
/********************************************************************************/
/*				Convierto un ASCII en un número entero.							*/
/*..............................................................................*/
unsigned char AsciiToInt(void)
{
	unsigned char numero;
	if(!ps2_str[0])
		return 0;
	if(ps2_str[1])
	{
		if(ps2_str[2])
			numero=(ps2_str[0]-48)*100+(ps2_str[1]-48)*10+(ps2_str[2]-48);
		else
			numero=(ps2_str[0]-48)*10+(ps2_str[1]-48);
	}
	else
		numero=ps2_str[0]-48;
	return numero;
}
/********************************************************************************/
/*			Convierto la lectura en una cadena de caracteres.					*/
/*..............................................................................*/
void IntToStr(unsigned int origen,register unsigned char *destino)
{
	unsigned int aux1,aux2,aux3,aux4;			// Variables auxiliares para la conversion.
	CLRWDT();
	aux1=(origen/1000);							// Calculo la unidad de mil.
	aux2=(origen-aux1*1000)/100;				// Calculo la centena.
	aux3=(origen-aux1*1000-aux2*100)/10;		// Calculo la decena.
	aux4=(origen-aux1*1000-aux2*100-aux3*10);	// Calculo la unidad.
	if(aux1)
		(*destino++)=aux1+48;					// Unidad de mil.
	if(aux2||aux1)
		(*destino++)=aux2+48;					// Centena.
	(*destino++)=aux3+48;						// Decena.
	(*destino++)=aux4+48;						// Unidad.
	(*destino++)=0;								// Final cadena.
	return;
}
/********************************************************************************/
/*	Habilito la recepción por RS232 y reseteo las variables que intervienen.	*/
/*..............................................................................*/
void RS232_Habilitada(void)
{
	CLRWDT();
	pos=0;
	rs_str[0]=0;				// Reseteo la cadena de recepción.
	RS232_ON;
	return;
}
/********************************************************************************/
/*				Leo desde el teclado o lo que ingresa por RS232					*/
/*..............................................................................*/
void Leer_RS_PS2(void)
{
	Reseteo_Teclado();
	RS232_Habilitada();
	while(INT0IE)							// Espero un enter del teclado.
	{
		if(!RCIE)							// Llegó algo por RS232?
		{
				pos_str_ps2=strlen(rs_str);	// Obtengo el largo de la cadena.
				vis=pos_str_ps2;			// Fuerzo la visualización en display.
				strcpy(ps2_str,rs_str);		// Copio lo llegado por RS232 a la cadena del PS2.
				RS232_Habilitada();			// Habilito el RS232 por si llega otra cadena.
		}
		if(pos_str_ps2<vis-1)				// Se perdió la variable vis?
			vis=pos_str_ps2;
		if(pos)								// Los datos entraron por RS232?
			Imprimir_Lcd("Nro tarjeta:",ps2_str,1);	// Muestro lo que se va escribiendo.
		else if(pos_str_ps2==vis)			// Actulizo la pantalla?
		{
			Imprimir_Lcd("Nro tarjeta:",ps2_str,1);	// Muestro lo que se va escribiendo.
			vis++;							// 
		}
		CLRWDT();
	}
	RS232_OFF;
	strcpy(rs_str,ps2_str);
	return;
}
/****************************************************************************/
/*			Comparo la contraseña del ROOT o del ADMIN con las guardadas.	*/
/*..........................................................................*/
unsigned char Check_Pass(unsigned char *pass)
{
	unsigned char dato[20];
	unsigned int i;
	if(Eeprom_Read(20)!='@')	// Hay contraseña guardada?
		return 2;				// Aviso que no hay contraseña.
	i=0;
	do
	{
		dato[i]=Eeprom_Read(i+21);
	}while(dato[i++]);			// Recorro la memoria hasta encontrar un NULL.
	if(strcmp(pass,dato))
		return 0;				// Salgo y aviso que la contraseña es incorrecta.
	return 1;					// Devuelvo el valor de memoria en donde termina la cadena.
}
