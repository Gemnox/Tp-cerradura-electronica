/****************************************************************************
 * File:   main.c                                                           *
 * Author: Esteban Lemos                                                    *
 * Created on 5 de febrero de 2014, 20:49                                   *
 * En caso de necesitar el proyecto completo ingresar en                    *
 * https://github.com/EEST1/Plantilla_6to                                   *
 ****************************************************************************/

#include <xc.h>
#include "confbits.h"       //no se debe de modificar este archivo para
//garantizar el funcionamiento del BOOTLOADER
#include "teclado.h"
#include "Tecnica1.h"
#include "lcd.h"
#include "ConfShield2_1.h"  
//segun el shield habilita distintas librerias

//  defino una funcion NO BLOQUEANT
void muestroLCD(int, int);
void escriboEE(unsigned int, unsigned char);
unsigned int leoEE(unsigned char);
unsigned int hola;
int dato1 = 0;
unsigned int valor, uni = 0;
unsigned int memoria = 0;
unsigned int abertura = 0, abertura2 = 0;

typedef enum Cerradura {
    LECTURA,
    ESCRITURA,
    NEUTRO,
} ESTADOCERRADURAX;
ESTADOCERRADURAX L_EX = NEUTRO; //estado del boton

/*variables globales ********************************************************/
void main(void) {
    unsigned char hola1, hola2;
    unsigned int flag;
    PicIni21();
    TIMER0_INI();
    TIMER1_INI();
    tecladoIni13();
    INTCONbits.GIE = 1;
    T0CONbits.TMR0ON = 1;
    T1CONbits.TMR1ON = 1;
    LCD_init();
    clear_LCD();
    ret_HOME();
    unsigned int contrasena = 0, validacion = 0, valorguardado = 0;
    
    contrasena= leoEE(3);


    while (1) {
        valor = tecla();
        if (valor != NO_TECLA) { // si la tecla esta presionada
            if (uni < 1000) {
                uni = (uni * 10) + valor;
            }}
            switch (L_EX){
                case NEUTRO:
                if (valor == '*')
                    valorguardado = uni;
              

            if (contrasena == valorguardado) validacion = 1;
            if (validacion == 1) {

                if (valor == 'A') L_EX = ESCRITURA;
                if (valor == 'B') L_EX = LECTURA;
            }
            break;

            }
        }
    }
    // muestro el valor del teclado en el lcd


void __interrupt myISR(void) {
    if (TMR0IF) {
        TMR0L = 209;
        TMR0H = 0xFF;
        INTCONbits.TMR0IF = 0;

        tic_LCD();
        tic_teclado();

    }
    if (RBIF) {
        marca_tecla();
        RBIF = 0;
    }
    if (PIR1bits.TMR1IF == 1) {
        TMR1L = 0xFE;
        TMR1H = 0x89;
        PIR1bits.TMR1IF = 0;
        tic_LCD();




    }
}

void muestroLCD(int contador, int pos) {
    unsigned char umil = 0, centena = 0, decena = 0, unidad = 0;

    centena = (contador % 1000) / 100;
    decena = ((contador % 1000) % 100) / 10;
    unidad = ((contador % 1000) % 100) % 10;
    umil = contador / 1000;


    set_CURSOR(pos);
    char2LCD(umil + 48);
    char2LCD(centena + 48);
    char2LCD(decena + 48);
    char2LCD(unidad + 48);


}

void escriboEE(unsigned int aux, unsigned char addrs) {
    unsigned char Memh = 0, Meml = 0, Memh2 = 0, Meml2 = 0,Memh3,Meml3;
    if (addrs == 1) {
        Memh = ((aux >> 8) & 0xFF);
        Meml = (aux & 0xFF);
        EEwrite2(1, Meml);
        EEwrite2(2, Memh);
    }
    if (addrs == 2) {
        Memh2 = ((aux >> 8) & 0xFF);
        Meml2 = (aux & 0xFF);
        EEwrite2(3, Meml2);
        EEwrite2(4, Memh2);
    if (addrs == 3) {
        Memh3 = ((aux >> 8) & 0xFF);
        Meml3 = (aux & 0xFF);
        EEwrite2(3, Meml3);
        EEwrite2(4, Memh3);    
    }
}
}
unsigned int leoEE(unsigned char adr) {
    unsigned char memoriaL = 0, memoriaH = 0, memoriaL2 = 0, memoriaH2 = 0,
    unsigned char memoriaH3=0,memorial3=0;
    unsigned int Mem = 0;
    if (adr == 1) {
        memoriaL = EEread2(1);
        memoriaH = EEread2(2);
        Mem = ((unsigned int) memoriaH << 8) + memoriaL;
    }
    if (adr == 2) {
        memoriaL2 = EEread2(3);
        memoriaH2 = EEread2(4);
        Mem = ((unsigned int) memoriaH2 << 8) + memoriaL2;
    }
     if (adr == 3) {
        memoriaL3 = EEread2(3);
        memoriaH3 = EEread2(4);
        Mem = ((unsigned int) memoriaH3 << 8) + memoriaL3;
    return Mem;
}
}