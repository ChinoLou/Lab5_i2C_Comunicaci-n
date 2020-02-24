/*
 * File:   i2C_Master.c
 * Author: Josué Castillo Lou
 * Laboratorio No.5: Comunicación I2C Master-LCD a 3 Pic´s Esclavos. (2Esclavos Listos!)
 * Created on 23 de febrero de 2020, 02:13 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_CLKOUT// Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// -------- PUERTOS DE LA LCD -----------------
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

//-------- Librerías a utilizar ----------------// 
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "I2C.h"
#include "Library_LCD.h"

#define _XTAL_FREQ 8000000   //8MHz

//------- Definición de Variables Globales -----// 
uint8_t  Val_ADC      = 0;
uint8_t  ADC_Temp     = 0;
uint8_t  Val_Cont     = 0;
uint16_t *voltaje_map = 0; // Definicion del Array de Mapeo para el Pot con Punteros!
uint16_t *voltaje_temp = 0; 




// ---------- Prototipos de Funciones  ---------//
void setup(void);
uint16_t *mapeo(uint8_t valor, uint8_t limPIC, uint8_t limFisico); //Funcion de 16bits usada par mapear el valor del Potenciómetro para la LCD con Punteros!

//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    
    LCD_iniciar();                      //Inicializamos la LCD
    LCD_CLEAR();
    LCD_CURSOR(1,1);
    LCD_STRING ("S1    S2   S3");

    while(1){                           
                                        //Comunicación con el Slave 1 (Potenciómetro)
        I2C_Master_Start();             //Leectura de datos "porque el último bit del 0x50 esta en 1 --> 0x51!        
        I2C_Master_Write(0x51);         //Dirección del esclavo con el bit de lectura de datos 0x51 !
        Val_ADC = I2C_Master_Read(0);   //porque manda ACKDT = 1 (SIGNIFICA Not Acknowledge) ??
        I2C_Master_Stop();
        __delay_ms(10);

        
                                        //Comunicación con el Slave 2 (Contador)
        I2C_Master_Start();             //Leectura de datos "porque el último bit del 0x50 esta en 1 --> 0x61!        
        I2C_Master_Write(0x61);         //Dirección del esclavo con el bit de lectura de datos 0x51 !
        PORTD = I2C_Master_Read(0);     //porque manda ACKDT = 1 (SIGNIFICA Not Acknowledge) ??
        I2C_Master_Stop();
        __delay_ms(10);
        
        Val_Cont = PORTD; 
        
        I2C_Master_Start();             //Leectura de datos "porque el último bit del 0x50 esta en 1 --> 0x71!        
        I2C_Master_Write(0x71);         //Dirección del esclavo con el bit de lectura de datos 0x51 !
        ADC_Temp = I2C_Master_Read(0);  //porque manda ACKDT = 1 (SIGNIFICA Not Acknowledge) ??
        I2C_Master_Stop();
        __delay_ms(10);

        ADC_Temp = ADC_Temp*10;         //Se multiplica por 10 para tener el valor en °C (LM35 10mV /°C) 
        
                                                           
        voltaje_map = mapeo(Val_ADC, 255, 5);               //Se muestra en la LCD el Sensor 1
        LCD_CURSOR(2,1);                                    // Mapeo del Sensor 1 (Potenciometro)
        LCD_CHAR(uint_to_char(voltaje_map[0]));
        LCD_CHAR('.');
        LCD_CHAR(uint_to_char(voltaje_map[1]));
        LCD_CHAR(uint_to_char(voltaje_map[2]));
        LCD_CHAR('V');
        
        LCD_CURSOR(2,7);                                    //Se muestra en la LCD el Sensor 2
        LCD_CHAR(uint_to_char(Val_Cont));
        
        
        voltaje_temp = mapeo(ADC_Temp, 255, 5);             // Mapeo del Sensor 3 (LM35)
        LCD_CURSOR(2,12);
        LCD_CHAR(uint_to_char(voltaje_temp[0]));            //Se muestra en la LCD el Sensor 2
        LCD_CHAR(uint_to_char(voltaje_temp[1]));
        LCD_CHAR('.');
        LCD_CHAR(uint_to_char(voltaje_temp[2]));
        LCD_CHAR('C');
       // LCD_STRING(lcd_valor);
        
    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISAbits.TRISA3 = 0;           //PIN RS (LCD)
    TRISAbits.TRISA4 = 0;           //PIN EN (LCD)
    TRISB = 0;                      //Puerto para la LCD
    TRISD = 0;                      //Puerto para la LCD
                    
                    
    PORTA = 0;
    PORTB = 0;  
    PORTD = 0;  

    I2C_Master_Init(100000);        // Inicializar Comuncación I2C (Frec. mas utilizada para el CLOCK = 100kHz)
}

/*Funcion que mapea un valor en una escala a un valor en otra escala y luego guarda los digitos en un array */   
 // Tiene como parámetro de entrada el Valor del ADC, Los 255 (8bits) y el valor a mapear VCC
                                                                                
uint16_t *mapeo(uint8_t valor, uint8_t limPIC, uint8_t limFisico){                 //Valor = ValADC , LimPIC es el límite que entiende el PIC 255, limFisico = 5Volts alimentacion max!
    uint16_t result[3] = {0,0,0};  // u.d1.d2  [u, d1, d2]   
    uint16_t dividendo = valor*limFisico;
    while (limPIC <= dividendo){
        result[0] = result[0] + 1;
        dividendo = dividendo - limPIC;
    }
    dividendo = dividendo *10;
    while (limPIC <= dividendo){
        result[1] = result[1] +1;
        dividendo = dividendo - limPIC;
    }
    dividendo = dividendo *10;
    while (limPIC <= dividendo){
        result[2] = result[2] +1;
        dividendo = dividendo - limPIC;
    }
    
    return(result);
}