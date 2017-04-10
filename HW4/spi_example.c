
#include <stdio.h>
#include <math.h>

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

unsigned char spi_io(unsigned char o);
void initSPI1(void);
void setVoltage(char channel, char voltage);
void makeWaveform(void);


int main(void) {
  
  initSPI1(); 
  RPB15Rbits.RPB15R = 0b0101;

  sprintf(buf, "Status 0x%x\r\n",status);

  sprintf(buf,"Writing \"%s\" to ram at address 0x%x\r\n", data, addr1);
  
                                                    // write the data to the ram
  ram_write(addr1, data, strlen(data) + 1);         // +1, to send the '\0' character
  ram_read(addr1, read, strlen(data) + 1);          // read the data back
  sprintf(buf,"Read \"%s\" from ram at address 0x%x\r\n", read, addr1);

  while(1) {
    ;
  }
  return 0;
}


unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
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
  SPI1CONbits.ON = 1;       // turn on SPI2

                            // send a ram set status command.
  CS = 0;                   // enable the ram
  spi_io(0x01);             // ram write status
  spi_io(0x41);             // sequential mode (mode = 0b01), hold disabled (hold = 0)
  CS = 1;                   // finish the command
}


void setVoltage(char channel, char voltage) {
    CS = 0;
    
    
    CS = 1;   
}
// write len bytes to the ram, starting at the address addr
void ram_write(unsigned short addr, const char data[], int len) {
  int i = 0;
                        // enable the ram by lowering the chip select line
  spi_io(0x2);                   // sequential write operation
  spi_io((addr & 0xFF00) >> 8 ); // most significant byte of address
  spi_io(addr & 0x00FF);         // the least significant address byte
  for(i = 0; i < len; ++i) {
    spi_io(data[i]);
  }
                       // raise the chip select line, ending communication
}

// read len bytes from ram, starting at the address addr
void ram_read(unsigned short addr, char data[], int len) {
  int i = 0;
  CS = 0;
  spi_io(0x3);                   // ram read operation
  spi_io((addr & 0xFF00) >> 8);  // most significant address byte
  spi_io(addr & 0x00FF);         // least significant address byte
  for(i = 0; i < len; ++i) {
    data[i] = spi_io(0);         // read in the data
  }
  CS = 1;
}

void makeWaveform(void) {
  int i = 0;
  for (i = 0; i < NUMSAMPS; ++i) {
    Waveform[i] = sin(i/NUMSAMPS*2*PI);
  }
}
