/********************************************************************************/
/*				MANEJO DE DISPLAY CON INTERFACE DE 4 BITS						*/
/********************************************************************************/
#define LCD_RS	RA0				// Register select
#define LCD_EN	RA1				// Enable

void Lcd_Write(unsigned char);
void Lcd_Clear(void);
void Lcd_Puts(const char * s);
void Lcd_Goto(unsigned char pos);
void Lcd_Setup(void);
void Lcd_Putch(char c);
void Imprimir_Lcd(const char *LINEA_1,const char *LINEA_2,unsigned char BORRAR);
void Linea_Lcd(const char *linea,unsigned char borrar);

#define	LCD_STROBE	(LCD_EN=1);(LCD_EN=0)
/********************************************************************************/
/*			ESCRIBE UN BYTE EN EL DISPLAY.										*/
/*------------------------------------------------------------------------------*/
void Lcd_Write(unsigned char c)
{
	CLRWDT();
	if(c==241||c==209)		// Corrección para que imprima la ñ.
		c=238;
	PORTA &=0xC3;
	PORTA |=(c>>2)&0x3C;
	LCD_STROBE;
	PORTA &=0xC3;
	PORTA |=(c<<2)&0x3C;
	LCD_STROBE;
	DelayMs(2);
	return;
}
/********************************************************************************/
/*			LIMPIO EL LCD.														*/
/********************************************************************************/
void Lcd_Clear(void)
{
	LCD_RS=0;
	Lcd_Write(0x1);
	DelayMs(2);
}
/********************************************************************************/
/*			ESCRIBO UNA CADENA DE CARACTERES EM EL DISPLAY.						*/
/********************************************************************************/
void Lcd_Puts(const char *s)
{
	LCD_RS=1;				// write characters
	while(*s)
		Lcd_Write(*s++);
}
/********************************************************************************/
/*			VOY A UNA POSICION DEL LCD.											*/
/********************************************************************************/
void Lcd_Goto(unsigned char pos)
{
	LCD_RS=0;
	Lcd_Write(0x80+pos);
}
/********************************************************************************/
/*			CONFIGURACION DEL LCD EN MODO 4 BITS.								*/
/********************************************************************************/
void Lcd_Setup(void)
{
/************************************************************************/
/* 		initialise the LCD - put into 4 bit mode 						*/
/*		movlw		b'00001111'		;LCD on, Cursor On, Parpadeo On		*/
/*		movlw		b'00001110'		;LCD on, Cursor On, Parpadeo Off	*/
/*		movlw		b'00001101'		;LCD on, Cursor Off, Parpadeo On	*/
/*		movlw		b'00001100'		;LCD on, cursor off. Parpadeo OFF	*/
/************************************************************************/
	DelayMs(15);
	LCD_RS=0;				// write control bytes
	PORTA &=0xC3;			// Limpio el puerto.
	PORTA |=0B00001100;		// Inicializo.
	LCD_STROBE;
	DelayMs(5);
	LCD_STROBE;
	DelayUs(100);
	PORTA &=0xC3;			// Limpio el puerto.
	PORTA |=0B00001000;
	LCD_STROBE;
	DelayUs(40);
	Lcd_Write(0x28);		// 4 bit mode, 1/16 duty, 5x8 font
	Lcd_Write(0b00001100);	// configuro la visualización.
	Lcd_Write(0x1);			// Limpio el LCD.
	DelayMs(15);
	return;
}
/********************************************************************************/
/*			ESCRIBO UN CARACTER EN EL DISPLAY.									*/
/********************************************************************************/
void Lcd_Putch(char c)
{
	LCD_RS=1;				// write characters
	Lcd_Write(c);
	return;
}
/********************************************************************************/
/*						*/
/********************************************************************************/
void Imprimir_Lcd(const char *linea_1,const char *linea_2,unsigned char borrar)
{
	if(borrar)
		Lcd_Clear();
	Lcd_Goto(0x00);
	Lcd_Puts(linea_1);
	Lcd_Goto(0x40);
	Lcd_Puts(linea_2);
	return;
}
/********************************************************************************/
/*					*/
/********************************************************************************/
void Linea_Lcd(const char *linea,unsigned char borrar)
{
	unsigned char largo=0;
	if(borrar)
		Lcd_Clear();
	Lcd_Goto(0x00);
	while(*linea)
	{
		Lcd_Putch(*linea++);
		largo++;
		if(largo==20)
			Lcd_Goto(0x40);
	}
	return;
}
