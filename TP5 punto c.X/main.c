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
//funciones//
void muestroLCD(int, int);
void escriboEE(unsigned int, unsigned char);
unsigned int leoEE(unsigned char);
void caractercerradura(void);
void caracterapertura(void);
void creacioncaracter(void);
void configuracion(void);


//variables//
unsigned int valor, uni = 0;
unsigned int memoria = 0;
unsigned int tiempo = 0;
unsigned char flag2 = 0;

//maquina de estado//

typedef enum Cerradura {
    LECTURA,
    ESCRITURA,
    NEUTRO,
} ESTADOCERRADURAX;
ESTADOCERRADURAX L_EX = NEUTRO; //estado del boton

/*variables globales ********************************************************/
void main(void) {
    //variables main/
    unsigned int flag, flag1 = 1;

    unsigned int contrasena, validacion = 0, valorguardado = 0;
    unsigned char cont = 0, cont1;
    unsigned int abertura, cerradura, confirmacion;
    unsigned int principal1 = 0, principal2 = 0, apertura1 = 0, apertura2 = 0;
    contrasena = leoEE(2); //leo la contraseña guardada en la EEProm
    configuracion();


    while (1) {

        valor = tecla();
        if (valor != NO_TECLA) { // si la tecla esta presionada
            if (uni < 1000) {
                uni = (uni * 10) + valor;
            }

        }
        switch (L_EX) {
            case NEUTRO: //estado neutro
                tiempo = 0;
                if (contrasena == leoEE(3) && flag1 == 1) { //si no hay contraseña
                    clear_LCD(); //limpia lcd
                    flag1 = 0; // flag ingreso contraseña
                    while (tiempo < 2000) { //se mantiene 2seg
                        //escribo enter password:
                        set_CURSOR(0x00);
                        msg2LCD("Enter");
                        set_CURSOR((0x40));
                        msg2LCD("Password");

                    }
                    clear_LCD();
                    validacion = 1;
                    valorguardado = 0;
                    flag = 0;
                }
                if (validacion == 0) { //si no entro en validacion
                    muestroLCD(uni, 0x0B);
                    set_CURSOR(0x00);
                    msg2LCD("Password:");
                    set_CURSOR(0x40);
                    msg2LCD("presione *");
                }
                if (valor == '*') { //si clickeo accion guardo dato del teclado
                    clear_LCD();
                    valorguardado = uni;
                    uni = 0;
                    tiempo = 0;
                    flag = 1;

                } //comparo contraseña
                if (contrasena == valorguardado) { // si son iguales
                    clear_LCD();
                    while (tiempo < 2000) {//se mantiene 2seg
                        set_CURSOR(0x00);
                        //escribo correct password
                        msg2LCD("Correct");
                        set_CURSOR(0x40);
                        msg2LCD("Password");
                    }
                    clear_LCD();
                    validacion = 1;
                    valorguardado = 0;
                    flag = 0;

                }
                if (contrasena != valorguardado && flag == 1) { //si no son iguales
                    clear_LCD();
                    while (tiempo < 2000) {//se mantiene 2seg
                        set_CURSOR(0x00);
                        //escribo incorrect password
                        msg2LCD("Incorrect");
                        set_CURSOR(0x40);
                        msg2LCD("Password");
                    }
                    clear_LCD();
                    flag = 0;
                }

                if (validacion == 1) { //si la contraseÃ±a P es confirmada

                    set_CURSOR(0x05);
                    msg2LCD("A=Escritura");
                    set_CURSOR(0x47);
                    msg2LCD("B=Lectura");
                    if (valor == 'A') { //entro en modo escritura   
                        clear_LCD();
                        uni = 0;
                        cont = 0;
                        L_EX = ESCRITURA;
                    }

                    if (valor == 'B') { //entro en lectura
                        clear_LCD();
                        uni = 0;
                        L_EX = LECTURA;
                    }

                }
                if (valor == '#') { //borro LCD
                    clear_LCD();
                    uni = 0;

                }
                break;
            case ESCRITURA: //modo escritura

                muestroLCD(uni, 0x00);
                switch (cont) {
                    case 0: //ingreso contraseña principal
                        set_CURSOR(0x06);
                        msg2LCD("Principal");
                        if (valor == '*') { //presiono la tecla de accion y guardo contraseÃ±a P
                            principal1 = uni;
                            clear_LCD();
                            uni = 0;
                            cont = 1;
                        }
                        if (valor == '#') { //salteo paso

                            clear_LCD();
                            uni = 0;
                            cont = 2;
                        }
                        break;
                    case 2: //ingreso contraseña de apertura

                        set_CURSOR(0x07);
                        msg2LCD("Apertura");
                        if (valor == '*') { // presiono la tecla de accion y guado apertura
                            tiempo=0;
                            apertura1 = uni;
                            clear_LCD();
                            uni = 0;
                            cont=3;

                        }
                        if (valor == '#') {//salteo paso

                            clear_LCD();
                            uni = 0;
                            L_EX = NEUTRO;
                        }
                        break;
                    case 1:
                        set_CURSOR(0x06);
                        msg2LCD("confirmar");
                        if (valor == '*') { //presiono la tecla de accion y guardo contraseÃ±a P
                            tiempo=0;
                            principal2 = uni;
                            clear_LCD();
                            uni = 0;
                            if (principal1 == principal2) {
                                escriboEE(principal1, 2);
                                cont = 2;
                            } else {
                                cont = 0;
                                 while (tiempo < 2000){
                                set_CURSOR(0x00);
                                msg2LCD("vuelva a ingresar");}
                                clear_LCD();
                            }
                        }
                        if (valor == '#') { //salteo paso

                            clear_LCD();
                            uni = 0;
                            cont = 2;
                        }



                        break;

                    case 3:
                        
                        set_CURSOR(0x07);
                        msg2LCD("confirmar");
                        if (valor == '*') { // presiono la tecla de accion y guado apertura
                            tiempo=0;
                            apertura2 = uni;
                            clear_LCD();
                            uni = 0;
                            if (apertura1 == apertura2) {
                            escriboEE(apertura1, 1);
                            L_EX = NEUTRO;

                        } else {
                            cont = 2;
                            while (tiempo < 2000){
                                set_CURSOR(0x00);
                                msg2LCD("vuelva a ingresar");}
                        }
                            clear_LCD();
                        }
                        if (valor == '#') {//salteo paso

                            clear_LCD();
                            uni = 0;
                            L_EX = NEUTRO;
                        }
                        

                        break;


                }
                break;

            case LECTURA: //modo lectura
                abertura = leoEE(1); //leo contraseña
                set_CURSOR(0x00);
                msg2LCD("Password:");
                muestroLCD(uni, 0x0B);
                set_CURSOR(0x40);
                msg2LCD("presione *");


                if (valor == '*') { //gurado el valor del teclado
                    clear_LCD();
                    confirmacion = uni;
                    tiempo = 0;
                    flag2 = 1;
                }
                if (confirmacion == abertura) { //comparo contraseñas


                    clear_LCD();


                    while (tiempo < 2000) { //se mantiene abierto
                        set_CURSOR(0x00);
                        msg2LCD("Abierto");
                        caracterapertura();

                        LATCbits.LC0 = 1;
                    }
                    tiempo = 0;
                    while (tiempo < 2000) { //despues de un tiempo se cierra
                        set_CURSOR(0x00);
                        msg2LCD("Cerrado");
                        caractercerradura();
                        confirmacion = 0;
                        flag2 = 1;
                        uni = 0;

                        LATCbits.LC0 = 0;


                    }

                }
                if (flag2 == 1 && confirmacion != abertura) { //contraseña incorrecta
                    while (tiempo < 2000) {
                        set_CURSOR(0x00);
                        msg2LCD("");
                        msg2LCD("Incorrect");
                        set_CURSOR(0x40);
                        msg2LCD("Password");
                    }
                    clear_LCD();
                    flag2 = 0;

                }




                if (valor == '#') { //vuelvo a neutro modo inicial
                    L_EX = NEUTRO; //vuelvo a neutro
                    clear_LCD();
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

        tiempo++;
        tic_LCD();
        tic_teclado();


    }
    if (RBIF) {
        marca_tecla();
        RBIF = 0;
    }
    if (PIR1bits.TMR1IF == 1) { //interrupcion timer1
        TMR1L = 0xFE;
        TMR1H = 0x89;
        PIR1bits.TMR1IF = 0;
        tic_LCD();




    }
}

void muestroLCD(int contador, int pos) {
    unsigned char umil = 0, centena = 0, decena = 0, unidad = 0;
    //division de unidades
    centena = (contador % 1000) / 100;
    decena = ((contador % 1000) % 100) / 10;
    unidad = ((contador % 1000) % 100) % 10;
    umil = contador / 1000;

    //solo muestro *
    set_CURSOR(pos);
    if (unidad > 0) {
        char2LCD('*');
    } else char2LCD(0 + 48);
    if (decena > 0) {
        char2LCD('*');
    } else char2LCD(0 + 48);
    if (centena > 0) {
        char2LCD('*');
    } else char2LCD(0 + 48);
    if (umil > 0) {
        char2LCD('*');
    } else char2LCD(0 + 48);


}
//funcion para escribir  16 bits en memoria

void escriboEE(unsigned int aux, unsigned char addrs) {
    unsigned char Memh, Meml, Memh2, Meml2, Memh3, Meml3, Meml4, Memh4;
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
    }
    if (addrs == 3) {
        Memh3 = ((aux >> 8) & 0xFF);
        Meml3 = (aux & 0xFF);
        EEwrite2(5, Meml3);
        EEwrite2(6, Memh3);
    }
    if (addrs == 4) {
        Memh4 = ((aux >> 8) & 0xFF);
        Meml4 = (aux & 0xFF);
        EEwrite2(7, Meml4);
        EEwrite2(8, Memh4);
    }
}
//funcion para leer 16 bits en memoria

unsigned int leoEE(unsigned char adr) {
    unsigned char memoriaL, memoriaH, memoriaL2, memoriaH2;
    unsigned char memoriaH3, memoriaL3, memoriaH4, memoriaL4;
    unsigned int Mem;
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
        memoriaL3 = EEread2(5);
        memoriaH3 = EEread2(6);
        Mem = ((unsigned int) memoriaH3 << 8) + memoriaL3;

    }
    if (adr == 3) {
        memoriaL4 = EEread2(5);
        memoriaH4 = EEread2(6);
        Mem = ((unsigned int) memoriaH4 << 8) + memoriaL4;

    }

    return Mem;
}
//caracter candado cerrado

void caractercerradura(void) {
    set_CURSOR(0x0B);
    char2LCD(0);
}
//caracter candado abierto

void caracterapertura(void) {
    set_CURSOR(0x0B);
    char2LCD(1);
}
//funcion para crear caracteres

void creacioncaracter(void) {
    write_CMD(0b01000000);
    read_BUSY();
    //candado cerrado
    char2LCD(0b00000000);
    char2LCD(0b00001110);
    char2LCD(0b00001010);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    char2LCD(0b00011011);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    //candado abierto
    char2LCD(0b00000000);
    char2LCD(0b00001110);
    char2LCD(0b00000010);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
    char2LCD(0b00011011);
    char2LCD(0b00011111);
    char2LCD(0b00011111);
}
//configuracion inicial

void configuracion(void) {
    PicIni21();
    TIMER0_INI();
    TIMER1_INI();
    tecladoIni13();
    INTCONbits.GIE = 1;
    T0CONbits.TMR0ON = 1;
    T1CONbits.TMR1ON = 1;
    TRISCbits.RC0 = 0;
    LCD_init();
    clear_LCD();
    ret_HOME();
    creacioncaracter();









}