
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>  
#include "ILI9163C.h"

#define SLAVE_ADDR 0x20

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

void display_character(char character, unsigned short color, int x, int y);

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
    
    SPI1_init();
    LCD_init();
    
  __builtin_enable_interrupts();
  
  LCD_clearScreen(GREEN);
  
  /*char message[10];
  sprintf(message, "H");
  
  int i=0;
  char ASCII_value;
  while(message[i]){
      ASCII_value = message[i] - 0x20;
      display_character(ASCII_value, BLACK, 28+6*i, 32);
      i++;
  }*/
  
  while(1) {
      ;
  }
    
  return (0);
}


void display_character(char character, unsigned short color, int x, int y) {
    
    int ix, iy;
    char binary, shifted;
    for (ix=0; ix<=4; ix++) {
        
        binary = ASCII[character][ix+1];
 
        for (iy=0; iy<=7; iy++) {
            shifted = 0b1 << iy;
            if (binary && shifted != 0) {
                LCD_drawPixel(ix+x, iy+y, color);
            }
        }        
        
    }
}