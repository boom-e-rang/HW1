
#include <stdio.h>
#include <math.h>
#include "NU32.h" 

// Demonstrates SPI
// PIC is the master, DAC is the slave
// Uses microchip MCP4902 chip 
// pin 1 (VDD) -> 3.3V
// pin 2 (NC) -> --
// pin 3 (CS) -> pin 17 B8
// pin 4 (SCK) -> pin 25 B14
// pin 5 (SDI) -> pin 12 RA4
// pin 6 (NC) -> --
// pin 7 (NC) -> --
// pin 8 (VOUTA) -> Ch1 of NScope
// pin 9 (VREFA) -> GND
// pin 10 (VSS) -> GND
// pin 11 (VREFB) -> GND
// pin 12 (VOUTB) -> Ch2 of NScope
// pin 13 (SHDN) -> --
// pin 14 (LDAC) -> --

#define CS LATBbits.LATB8       // chip select pin
#define NUMSAMPS 100            // number of points in waveform
#define PI 3.14159265
#define DELAY 40

unsigned char spi_io(unsigned char o);
void initSPI1(void);
void setVoltage(char channel, char voltage);
void makeSinWave(void);
void makeTriangleWave(void);


int main(void) {
  
  initSPI1(); 
  RPA4Rbits.RPA4R=0b0011;
  i=0;
          
  while(1) {
    
    _CP0_SET_COUNT(0);
    setVoltage(0, SinWave[i]);
    setVoltage(1, TriangleWave[i]);
    
    while(_CP0_GET_COUNT()<DELAY){
        ;
    }
    
    if (i==100){
        i=0;
    } else {
        i=i+1;
    }
  
  }
  return (0);
}


unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
}

// initialize SPI1
void initSPI1(void) {
  // set up the chip select pin as an output
  // the chip select pin is used by the slave to indicate
  // when a command is beginning (clear CS to low) and when it
  // is ending (set CS high)
  TRISBbits.TRISB8 = 0;
  CS = 1;

  // Master - SPI1, pins are: SDI4(F4), SDO4(F5), SCK4(F13).  
  // we manually control SS4 as a digital output (F12)
  // since the pic is just starting, we know that SPI is off. We rely on defaults here
 
  // setup SPI1
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1BRG = 0x3;            // baud rate to 10 MHz [SPI2BRG = (80000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on SPI1
}


void setVoltage(char channel, char voltage) {
    
    unsigned char something1, something2;
    
    CS = 0;
    if (channel==0) {
        something1 = (voltage >> 4) + 0b01110000;
    } else if (channel==1) {
        something1 = (voltage >> 4) + 0b11110000;
    }
    something2 = (voltage << 4);
   
    spi_io(something1);
    spi_io(something2);
    CS = 1;   
}

void makeSinWave(void) {
  int i = 0;
  for (i = 0; i < NUMSAMPS; ++i) {
    SinWave[i] = sin(i/NUMSAMPS*2*PI)*255;
  }
}

void makeTriangleWave(void) {
  int i = 0;
  for (i = 0; i < NUMSAMPS; ++i) {
    TriangleWave[i] = (i/NUMSAMPS)*255;
  }
}