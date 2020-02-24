/*
 * File:   i2C_Slave1_POT.c
 * Author: Josué Castillo Lou / Carné: 17169
 * Slave No.3: envia los datos del ADC del LM35 al PICMaster através del i2C Listo!!!
 * Created on 23 de febrero de 2020, 03:54 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_CLKOUT// Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//-------- Librerías a utilizar ---------------//  
#include <xc.h>
#include <stdint.h>
#include "ADC_CH.h"             //Librería del cambio del ADC  
#include "I2C.h"                //Librería I2X del Slave 1  

#define _XTAL_FREQ 8000000      // 8MHz

//------- Definición de Variables Globales ----// 
uint8_t ADC_Temp = 0;
uint8_t z = 0;      //variable para el I2C

//------- Prototipos de Funciones ------------// 
void init_config(void);


// Código de Interrupción 
//*****************************************************************************
void __interrupt() isr(void){
   if(PIR1bits.SSPIF == 1){ 

        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }

        /*if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
            //__delay_us(7);
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            //__delay_us(2);
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupción recepción/transmisión SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepción se complete
            PORTD = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepción
            __delay_us(250); }*/
            
        else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            BF = 0;
            SSPBUF = ADC_Temp;
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
}


void main(void) {
    init_config();
    I2C_Slave_Init(0x70);       //Le asignamos la dirección 0x50 al Slave 3
    // Loop infinito
    while(1){
   
        ADC_chanel(0);          // Seleccionamos el Canal 0 del ADC con la Libreria del ADC
        PIR1bits.ADIF = 0;
        ADCON0bits.GO = 1;       
        ADC_Temp = ADRESH;

       
        __delay_ms(5);          // Delay recomendado para hacer cambio de canal
    }
    return;
}


void init_config(void){ 
    TRISA = 0b00000001;         // RA0 como entrada
    ANSEL = 0b00000001;         // AN0 como entrada analógica asociadas a RA0 
    TRISB = 0x00;
    ANSELH = 0x00;              //ANSELH deshabilitado        
    ADCON1 = 0;
    
    PORTA = 0;                  //Inicializamos los puertos utilizados 
    PORTB = 0x00; 

    INTCONbits.GIE  = 1;        // Habilitamos interrupciones
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones PEIE
  
   }