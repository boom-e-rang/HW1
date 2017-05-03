#include <xc.h> 
#include "helper.h"
#include "i2c_master_noint.h"
#include "ILI9163C.h"


#define SLAVE_ADDR 0x6b

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

void write_to_slave(int register_address, int number) {
    
    i2c_master_start();                     // Begin the start sequence
    i2c_master_send((SLAVE_ADDR << 1) | 0); // send the slave address, left shifted by 1,
                                            // which clears bit 0, indicating a write
    i2c_master_send(register_address);      
    i2c_master_send(number); 
    i2c_master_stop();
    
}

int sign(int x) {
    if (x > 0) {
        return (1);
    } else if (x < 0) {
        return (-1);
    } else {
        return (0);
    }
}

void convert(unsigned char * array, int * acc_gyr) {
    
    acc_gyr[3] = (unsigned short) array[0] + (signed short) (array[1] << 8);
    acc_gyr[4] = (unsigned short) array[2] + (signed short) (array[3] << 8);
    acc_gyr[5] = (unsigned short) array[4] + (signed short) (array[5] << 8);
    acc_gyr[0] = (unsigned short) array[6] + (signed short) (array[7] << 8);
    acc_gyr[1] = (unsigned short) array[8] + (signed short) (array[9] << 8);
    acc_gyr[2] = (unsigned short) array[10] + (signed short) (array[11] << 8);
    
}

void convert_justZ(unsigned char * array, int * acc_gyr) {
    
    acc_gyr[0] = (unsigned short) array[0] + (signed short) (array[1] << 8);
    
}