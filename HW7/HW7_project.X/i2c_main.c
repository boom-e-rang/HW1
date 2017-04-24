
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>  
#include "i2c_master_noint.h"
#include "ILI9163C.h"


#define SLAVE_ADDR 0x6b

// DEVCFG0
#pragma config DEBUG = 11 // no debugging
#pragma config JTAGEN = 0 // no jtag
#pragma config ICESEL = 11 // use PGED1 and PGEC1
#pragma config PWP = 111111111 // no write protect
#pragma config BWP = 0 // no boot write protect
#pragma config CP = 1 // no code protect

// DEVCFG1
#pragma config FNOSC = 011 // use primary oscillator with pll
#pragma config FSOSCEN = 0 // turn off secondary oscillator
#pragma config IESO = 0 // no switching clocks
#pragma config POSCMOD = 10 // high speed crystal mode
#pragma config OSCIOFNC = 1 // free up secondary osc pins
#pragma config FPBDIV = 00 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = 11 // do not enable clock switch
#pragma config WDTPS = 10100 // slowest wdt
#pragma config WINDIS = 1 // no wdt window
#pragma config FWDTEN = 0 // wdt off by default
#pragma config FWDTWINSZ = 11 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = 001 // (divide by 2) divide input clock to be in range 4-5MHz (divide by 2)
#pragma config FPLLMUL = 111 // (multiply by 24) multiply clock after FPLLIDIV
#pragma config FPLLODIV = 001 // (divide by 2) divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = 001 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = 0 // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = 0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 1 // USB BUSON controlled by USB module

void read_all(unsigned char * array, int address, int number);
void print_to_LCD(char * message, unsigned short color, unsigned short background, int x, int y);
void display_character(char character, unsigned short color, unsigned short background, int x, int y);

unsigned char array[14];

int main() {
  
  __builtin_disable_interrupts();
  
      // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    // turn off analog (do I need this?)
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    
    i2c_master_setup();                       // init I2C2, which we use as a master
    SPI1_init();
    LCD_init();
    
  __builtin_enable_interrupts();
  
    // clean screen and initialize variables
    LCD_clearScreen(GREEN);
    char message[15];
    int i=0;
    for (i=0;i<=13;i++) {
        array[i]=1;
    }

    // WHO AM I using functions
    read_all(array, 0x0F, 1);
    sprintf(message, "WHO AM I = %d", array[0]);
    print_to_LCD(message, BLACK, GREEN, 20, 32);
    
  int count=0;  
  while(1) {
    
    // set clock reading to 0  
    _CP0_SET_COUNT(0);  
 
    read_all(array, 0x28, 6);
    
    int acc_x = (unsigned int) array[0] + (signed int) (array[1] << 8);
    int acc_y = (unsigned int) array[2] + (signed int) (array[3] << 8);
    int acc_z = (unsigned int) array[4] + (signed int) (array[5] << 8);
    sprintf(message, "Acc in x = %d", acc_x);
    print_to_LCD(message, BLACK, GREEN, 20, 42);
    sprintf(message, "Acc in y = %d", acc_y);
    print_to_LCD(message, BLACK, GREEN, 20, 52);
    
    sprintf(message, "%d %d", array[0], array[1]);
    print_to_LCD(message, BLACK, GREEN, 20, 62);
    
    
    // wait to create a 5Hz loop
    while(_CP0_GET_COUNT()<48000000/2/5) {
        sprintf(message, "%d", count);
        print_to_LCD(message, BLACK, GREEN, 20, 82);
    } 
    count++;
  }
    
  return (0);
}


void display_character(char character, unsigned short color, unsigned short background, int x, int y) {
    
    int ix, iy;
    char binary;
    for (ix=0; ix<=4; ix++) {
        
        binary = ASCII[character][ix];
 
        for (iy=0; iy<=7; iy++) {
            
            if (((ix+x)<128) & ((iy+y)<128)) {
            
                if ((binary >> iy) & 1) {
                    LCD_drawPixel(ix+x, iy+y, color);
                } else {
                    LCD_drawPixel(ix+x, iy+y, background);
                }
            
            }
            
        }        
        
    }
}

void read_all(unsigned char * array, int address, int number) {
    
    int i=0;
    
    for (i=0; i<=(number-1); i++) { 
        i2c_master_start();                     // Begin the start sequence
        i2c_master_send((SLAVE_ADDR << 1) | 0); // send the slave address, left shifted by 1,
                                                // which clears bit 0, indicating a write
        i2c_master_send(address+i);      
        i2c_master_restart();                   // send a RESTART so we can begin reading
        i2c_master_send((SLAVE_ADDR << 1) | 1); // send slave address, left shifted by 1,
                                                // and then a 1 in lsb, indicating read
        array[i] = i2c_master_recv();           // receive a byte from the bus
        i2c_master_ack(1);                      // send NACK (1):  master needs no more bytes
        i2c_master_stop();
    }
    
}

void print_to_LCD(char * message, unsigned short color, unsigned short background, int x, int y) {
    int i=0;
    char ASCII_value;
    while(message[i]){
      ASCII_value = message[i] - 0x20;
      display_character(ASCII_value, color, background, x+6*i, y);
      i++;
    }
}