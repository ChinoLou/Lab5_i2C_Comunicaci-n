/*
 * File:   Lib_LCD.c
 * Author: Josue
 *
 * Created on 23 de febrero de 2020, 02:22 PM
 */

#define RS RA3
#define EN RA4
#define D0 RB0
#define D1 RB1
#define D2 RB2
#define D3 RB3
#define D4 RB4
#define D5 RB5
#define D6 RB6
#define D7 RB7
#define puerto PORTB
#define _XTAL_FREQ 4000000
#include <stdint.h>
#include <xc.h>
#include "Library_LCD.h"


void LCD_iniciar(void){
    __delay_ms(20);
    LCD_DATO(0x30);
    __delay_ms(5);
    LCD_DATO(0x30);
    __delay_us(100);
    LCD_DATO(0x30);
    __delay_us(100);
    LCD_DATO(0x38);
    __delay_us(53);
    LCD_DATO(0x08);
    __delay_us(53);
    LCD_DATO(0x01);
    __delay_ms(4);
    LCD_DATO(0x06);
    __delay_us(53);
    LCD_DATO(0x0C);
    __delay_us(53);
}

void LCD_DATO(int var){
    RS = 0;
    __delay_ms(5);
    EN = 1;
    puerto = var;
    __delay_ms(5);
    EN = 0;
    __delay_ms(5);
}

void LCD_CLEAR(void){
    LCD_DATO(0x00);
    LCD_DATO(0x01);
    __delay_ms(5);
}
    

void LCD_CURSOR(int a, int b)
{
    int d;
	if(a == 1)
	{
        d = 0x80 + b - 1;
		LCD_DATO(d);
	}
	else if(a == 2)
	{
		d = 0xC0 + b - 1;
		LCD_DATO(d);
	}
}



void LCD_CHAR(char a)
{   
    RS = 1;
    // LCD_Port(caracter);
    PORTB = a;
    EN = 1;
    __delay_us(40);
    EN = 0;
}

void LCD_STRING(char *a)
{
    int n;
    for(n = 0; a[n] != '\0'; n++){
        LCD_CHAR(a[n]);
    }
}

char uint_to_char(uint8_t numero){
    char numChr = 214;
    switch (numero){
        case 0:
            numChr = 48;     // 48 ASCII => 0
            break;
        case 1:
            numChr = 49;     // 49 ASCII => 1
            break;
        case 2:
            numChr = 50;     // 50 ASCII => 2
            break;
        case 3:
            numChr = 51;     // 51 ASCII => 3
            break;
        case 4:
            numChr = 52;     // 52 ASCII => 4
            break;
        case 5:
            numChr = 53;     // 53 ASCII => 5
            break;
        case 6:
            numChr = 54;     // 54 ASCII => 6
            break;
        case 7:
            numChr = 55;     // 55 ASCII => 7
            break;
        case 8:
            numChr = 56;     // 56 ASCII => 8
            break;
        case 9:
            numChr = 57;     // 57 ASCII => 9
            break;
        case 10:
            numChr = 65;     // 58 ASCII => A
            break;
        case 11:
            numChr = 66;     // 66 ASCII => B
            break;
        case 12:
            numChr = 67;     // 67 ASCII => C
            break;
        case 13:
            numChr = 68;     // 68 ASCCI => D
            break;
        case 14:
            numChr = 69;     // 69 ASCCI => E
            break;
        case 15:
            numChr = 70;     // 70 ASCCI => F
            break;
        default:
            numChr = 214;
    }
    return(numChr);
}
