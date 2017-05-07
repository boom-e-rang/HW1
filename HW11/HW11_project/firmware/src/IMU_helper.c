#include <xc.h> 
#include "IMU_helper.h"
#include "i2c_master_noint.h"
#include <stdio.h>

#define SLAVE_ADDR 0x6b


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

void convert_acc(unsigned char * array, int * acc_all) {
    
    acc_all[0] = (unsigned short) array[0] + (signed short) (array[1] << 8);
    acc_all[1] = (unsigned short) array[2] + (signed short) (array[3] << 8);
    acc_all[2] = (unsigned short) array[4] + (signed short) (array[5] << 8);
    
}
