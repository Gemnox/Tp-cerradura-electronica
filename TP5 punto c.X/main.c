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
void caractercerradura(void);
void caracterabertura(void);
unsigned int hola;
int dato1 = 0;
unsigned int valor, uni = 0;
unsigned int memoria = 0;
unsigned int tiempo = 0;

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
    unsigned char cont = 0;
    unsigned int abertura, cerradura, confirmacion;
    escriboEE(1111, 3);
    contrasena = leoEE(3);


    while (1) {
        valor = tecla();
        if (valor != NO_TECLA) { // si la tecla esta presionada
            if (uni < 1000) {
                uni = (uni * 10) + valor;
            }
        }
        switch (L_EX) {
            case NEUTRO: //estado neutro
                if (valor == '*')
                    valorguardado = uni;


                if (contrasena == valorguardado) validacion = 1;
                if (validacion == 1) { //si la contraseña P es confirmada

                    if (valor == 'A') L_EX = ESCRITURA; //entro en escritura
                    if (valor == 'B') L_EX = LECTURA; //entro en lectura 
                }
                break;
            case ESCRITURA:
                cont = 0;
                switch (cont) {
                    case 0:
                        if (valor == '*') { //presiono la tecla de accion y guardo contraseña P
                            escriboEE(uni, 3);
                            cont = 1;
                        }
                        if (valor == '#') cont = 1; //salteo paso

                        break;
                    case 1:
                        if (valor == '*') { // presiono la tecla de accion y guado abertura
                            escriboEE(uni, 1);
                            cont = 2;
                        }
                        if (valor == '#') cont = 2; //salteo paso
                        break;
                    case 2:
                        if (valor == '*') //presiono la tecla de accion y guado guardo cerradura
                            escriboEE(uni, 2);
                        L_EX = NEUTRO; //vuelvo a neutro
                }
                if (valor == '#') L_EX = NEUTRO; //vuelvo a neutro

                break;

                break;
            case LECTURA:
                abertura = leoEE(1);
                cerradura = leoEE(2);

                if (valor == '*') { //detecto la clave
                    confirmacion = uni;
                }
                if (confirmacion == abertura) { //confirmacion de abertura
                    //carcterabertura();//
                    confirmacion = 0;
                    if (tiempo > 30000) { //si pasa 30s abertura se cierra
                        //caractercerradura//
                    }

                } else
                    if (confirmacion == cerradura) { //confirmo cerradura
                    //caractercerradura();//
                    confirmacion = 0;
                } else
                    confirmacion = 0;
                if (valor == '#') L_EX = NEUTRO; //vuelvo a neutro


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
    unsigned char Memh = 0, Meml = 0, Memh2 = 0, Meml2 = 0, Memh3, Meml3;
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
    unsigned char memoriaL = 0, memoriaH = 0, memoriaL2 = 0, memoriaH2 = 0;
    unsigned char memoriaH3 = 0, memoriaL3 = 0;
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

void caractercerradura(void) {

    write_CMD(0b01000000);
    read_BUSY();
    //cuadrado izquierda
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    //cuadrado derecha
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    //llave derecha
    char2LCD(0b00000000);
    char2LCD(0b00000000);
    char2LCD(0b00000000);
    char2LCD(0b00010000);
    char2LCD(0b00011000);
    char2LCD(0b00011100);
    char2LCD(0b00001110);
    char2LCD(0b00000110);
    //llave izquierda
    0b00000,
    char2LCD(0b00000000);
    char2LCD(0b00000000);
    char2LCD(0b00000011);
    char2LCD(0b00000111);
    char2LCD(0b00001110);
    char2LCD(0b00001100);

    set_CURSOR(0x4D);
    char2LCD(0);
    set_CURSOR(0x4E);
    char2LCD(1);
    set_CURSOR(0x0E);
    char2LCD(2);
    set_CURSOR(0x0D);
    char2LCD(3);
}