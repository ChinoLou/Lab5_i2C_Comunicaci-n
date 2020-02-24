/*
 * File:   i2C_Slave2_COUNT.c
 * Author: Josué Castillo Lou / Carné: 17169
 * Slave No. 2: envia los datos del contador al PIC MASTER! (Listo!!)
 * Created on 24 de febrero de 2020, 12:08 PM
 */

// CONFIG1 (DEBE IR DEBJO DEL #include <xc.h>)
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
#include "I2C.h"                //Librería I2X del Slave 2  

#define _XTAL_FREQ 8000000      // 8MHz

//------- Definición de Variables Globales ----// 
uint8_t ESTADO  = 0x00;
uint8_t ESTADO2 = 0x00;
uint8_t       z = 0;      //variable para el I2C


//------- Prototipos de Funciones ------------// 
void init_config(void);


// ------  Código de Interrupción ------------//
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
        
        /*
        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
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
            SSPBUF = PORTD;
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
}
//*****************************************************************************

void main(void) {
    init_config();
    I2C_Slave_Init(0x60);               //Le asignamos la dirección 0x60 al Slave 2
    
    while (1){
        
        if (PORTBbits.RB6 == 0){        //Secuencia de Incremento Puerto D 
            ESTADO = 1;
            __delay_ms(80);
            
            if (ESTADO == 1 && PORTBbits.RB6 == 1) {        //Antirebote Revisa que ya se halla dejado de presionar el PUSH RB6 (pull-up "normalmente en 1)
                PORTD = PORTD + 1 ;
                if (PORTD == 0x10){
                    PORTD = 0x0F;
                } 
            }
            ESTADO = 0;   
        }
            
       
        if (PORTBbits.RB7 == 0){       //Secuencia de decremento Puerto D 
            ESTADO2 = 1;
            __delay_ms(80);

            if (ESTADO2 == 1 && PORTBbits.RB7 == 1) {        //Antirebote Revisa que ya se halla dejado de presionar el PUSH RB6 (pull-up "normalmente en 1)
                PORTD = PORTD - 1 ;
                if (PORTD == 0xFF){
                    PORTD = 0x00;
                } 
            }
            ESTADO2 = 0;
        }
             
    
  }   
    return;
}


// Funciones de Inicialización de Puertos 
void init_config(void){
    ANSEL = ANSELH = 0;         //Ningun puerto analógico!
    TRISD = 0x00;               //Puerto de Salida para leds del contador
    TRISB = 0b11000000;         //Se configuran los pines como entradas asociadas a las resistencias WPUB´s correspondientes. 
    WPUB  = 0b11000000;         //Se habilitan las resistencias WPUB6 y WPUB7
    OPTION_REGbits.nRBPU = 0;   //Se habilitan las resistencias PULL-UP´s del´Puerto B 
    
    PORTA = 0x00;          //Inicialización de Puertos 
    PORTD = 0x00;
    
    INTCONbits.GIE  = 1;        // Habilitamos interrupciones
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones PEIE
}