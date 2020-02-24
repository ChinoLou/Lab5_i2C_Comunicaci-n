/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __LIBRARY_LCD_H
#define	__LIBRARY_LCD_H

#define _XTAL_FREQ 4000000
#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>
void LCD_DATO(int var);
void LCD_iniciar(void);
void LCD_CLEAR(void);
void LCD_CURSOR(int a, int b);
void LCD_CHAR(char a);
void LCD_STRING(char *a);
char uint_to_char(uint8_t numero);


#endif	/* XC_HEADER_TEMPLATE_H */

