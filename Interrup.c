/********************************************************************************/
/*								INTERRUPCIONES									*/
/********************************************************************************/
extern bank2 unsigned char pos_ps2,lect_ps2;
extern void Interpretar_Teclado(void);
void interrupt isr(void)
{
	unsigned char resp;
	CLRWDT();
	if(INT0IF && INT0IE)			// Interrupción generada por el teclado PS2.
	{
		if(pos_ps2>=3&&pos_ps2<=10)	// Los bit 3 a 10 se consideran datos.
		{							// Paridad, start y stop son ignorados.
			lect_ps2=(lect_ps2>>1);	// Desplazo los bits un lugar
			if(DATO_PS2)			// Dependiendo del dato que leo en el pin del pic
				lect_ps2=(lect_ps2|0x80);	// escribo un 1 en el bit mas significativo.
		}
		pos_ps2--;					// Voy al siguiente bit.
	   	if(!pos_ps2)				// Final de la cadena de bits??
		{
			Interpretar_Teclado();
			lect_ps2=0;				// y vacio la variable lectura.	
		}
		INT0IF=0;					// Bajo la bandera de la interrupción externa.
		return;
	}
	if(RCIF && RCIE)				// Interrupcion por RS232?
	{
		resp=RCREG;					// Vacío el buffer del nódulo RS232.
		if(resp<=31&&pos)			// Condición de salida.
		{
			RS232_OFF;
			rs_str[pos]=0;			// Marco final de cadena.
			pos=0;					// Reseteo el lugar de llenado de la cadena.
			return;					// Salgo de la interrupción.
		}
		if(resp>=32)				// Comienzo el llenado.
		{
			rs_str[pos]=resp;		// Lleno la cadena con los caracteres recibidos.
			if(pos<15)				// Queda lugar en la cadena?
				pos++;
		}
		return;
	}
	if(TMR0IE && TMR0IF)			// Interrupción por TMR0 (temporizador).
	{
		TMR0IF=0;					// Bajo la bandera de la interrupción.
		TMR0IE=0;					// Deshabilito la interrupción.
		return;
	}
	if(TMR1IE && TMR1IF)			// Interrupción por TMR1 (BUZZER).
	{
		TMR1=65200;
		if(!--audio)
		{
			audio=tono_audible;		// Cargo la frecuencia del sonido.
			BUZZER=!BUZZER;
		}
		if(!--duracion)
		{
			TMR1_OFF;				// Deshabilito la interrupción por TMR1.
			BUZZER=0;				// Me aseguro que el parlante quede en estado bajo.
		}
		TMR1IF=0;
		return;
	}
	if(TMR2IE && TMR2IF)			// Interrupción por TMR2.
	{
		if(bienvenida++>(102*tiempo_r))
		{
			bienvenida=0;
			bandera=1;				// Aviso el reenvio de la orden.
		}
		TMR2IF=0;					// Bajo la bandera de la interrupción.
		return;						// Salgo de la interrupción.
	}
	if(TMR3IE && TMR3IF)			// Interrupción por TMR3.
	{
		TMR3IF=0;					// Bajo la bandera de la interrupción.
		TMR3IE=0;
		return;						// Salgo de la interrupción.
	}
	return;							// Salgo de las interrupciones.
}
