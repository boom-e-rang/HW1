
#include "i2c_master_noint.h"
#include <stdio.h>
#include <stdlib.h>

#include <xc.h>  
// Demonstrate I2C by having the I2C1 talk to a chip
// Master will use SDA2 (pin6, RB2) and SCL2 (pin7, RB3).  Connect these through resistors to
// Vcc (3.3 V) (2.4k resistors recommended, but around that should be good enough)

// Two bytes will be written to the slave and then read back to the slave.

// Wiring of the chip
// SCL -> SCL2 + pull-up resistor
// SDA -> SDA2 + pull-up resistor
// A2 -> GND
// A1 -> GND 
// A0 -> GND
// RESET -> 3.3V
// ...
// VSS - GND

// VDD - capacitor, VDD
// GP7 -> push button
// ...
// GP0 -> LED


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


int main() {
    
  // some initialization function to set the right speed setting
  char buf[100] = {};                       // buffer for sending messages to the user
  
  
  __builtin_disable_interrupts();
  
      // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    // turn off analog
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    
    i2c_master_setup();                       // init I2C2, which we use as a master
    
  __builtin_enable_interrupts();
  
  
    i2c_master_start();                     // Begin the start sequence
    i2c_master_send((SLAVE_ADDR << 1) | 0); // send the slave address, left shifted by 1,
                                            // which clears bit 0, indicating a write
    i2c_master_send(0x00);                  // IODIR register address is 0x00
    i2c_master_send(0b11110000);            // set pins GP0-3 as outputs
                                            // set pins GP4-7 as inputs
    i2c_master_stop();
    
    i2c_master_start();                     
    i2c_master_send((SLAVE_ADDR << 1) | 0); 
    i2c_master_send(0x09);                  // GPIO register address is 0x00
    i2c_master_send(0b00000000);            // set values to logic-low
    i2c_master_stop();


  while(1) {
      
    i2c_master_start();                     
    i2c_master_send((SLAVE_ADDR << 1) | 0); 
    i2c_master_send(0x09);                  // indicate register of pin to read (GPIO)
    i2c_master_restart();                   // send a RESTART so we can begin reading
    i2c_master_send((SLAVE_ADDR << 1) | 1); // send slave address, left shifted by 1,
                                            // and then a 1 in lsb, indicating read
    char r = i2c_master_recv();             // receive a byte from the bus
    i2c_master_ack(1);                      // send NACK (1):  master needs no more bytes
    i2c_master_stop();
    
    char r_compare = 0b10000000;
    if (r == r_compare) {
        i2c_master_start();                     
        i2c_master_send((SLAVE_ADDR << 1) | 0); 
        i2c_master_send(0x09);                  // GPIO register address is 0x00
        i2c_master_send(0b00000001);            // set values to logic-low
        i2c_master_stop();
    } else {
        i2c_master_start();                     
        i2c_master_send((SLAVE_ADDR << 1) | 0); 
        i2c_master_send(0x09);                  // GPIO register address is 0x00
        i2c_master_send(0b00000000);            // set values to logic-low
        i2c_master_stop();
    }
    
  }
    
  return (0);
}
