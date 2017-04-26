/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <xc.h> 
#include "app.h"
#include "i2c_master_noint.h"
#include "ILI9163C.h"


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;
#define SLAVE_ADDR 0x6b

unsigned char array[14];

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/

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


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

  __builtin_disable_interrupts();
  
      // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    // for HW1
    TRISBbits.TRISB4 = 1; // set pushbutton to an input pin 
    TRISAbits.TRISA4 = 0; // set LED to an output pin
    LATAbits.LATA4 = 1; // set LED on
    
    // turn off analog (do I need this?)
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    
    i2c_master_setup();                       // init I2C2, which we use as a master
    SPI1_init();
    LCD_init();
    
  __builtin_enable_interrupts();
    
    // clean screen and initialize variables
    LCD_clearScreen(YELLOW);
    char message[15];
    int i=0;
    for (i=0;i<=13;i++) {
        array[i]=1;
    }

    // WHO AM I using functions
    read_all(array, 0x0F, 1);
    sprintf(message, "WHO AM I = %d", array[0]);
    print_to_LCD(message, BLACK, YELLOW, 20, 12);
    
    write_to_slave(0x10, 0b10000010);         // CTRL1_XL (sample rate 1.66 kHz, 2g sensitivity, and 100 Hz filter)
    write_to_slave(0x11, 0b10001000);         // CTRL2_G (sample rate 1.66 kHz, 1000 dps sensitivity)
    
    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
       
        
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            // HW1
            /* _CP0_SET_COUNT(0);
            while(_CP0_GET_COUNT()<12000) {
               if (PORTBbits.RB4==1){
                   break;
                }
            }
            _CP0_SET_COUNT(0);
            if (PORTAbits.RA4==1) {
                LATAbits.LATA4 = 0;
            } else {
                LATAbits.LATA4 = 1;
            }
            while (PORTBbits.RB4==1) {
                ; // wait while button is pushed
            } */
            
            // set clock reading to 0  
            _CP0_SET_COUNT(0);  

            read_all(array, 0x28, 6);
            char message[15];

            int acc_x = (unsigned short) array[0] + (signed short) (array[1] << 8);
            int acc_y = (unsigned short) array[2] + (signed short) (array[3] << 8);
            int acc_z = (unsigned short) array[4] + (signed short) (array[5] << 8);

            int x_scaled = (float) acc_x / 16383.5 * 50.0;
            int y_scaled = (float) acc_y / 16383.5 * 50.0;

            sprintf(message, "Acc in x = %d   ", x_scaled);
            print_to_LCD(message, BLACK, YELLOW, 20, 22);
            sprintf(message, "Acc in y = %d   ", y_scaled);
            print_to_LCD(message, BLACK, YELLOW, 20, 32);

            int ii=0;
            for (ii=0;ii<=50;ii++) {
                if (ii<=abs(x_scaled)) {
                    LCD_drawPixel(64-sign(x_scaled)*ii, 84, BLACK);
                    LCD_drawPixel(64+sign(x_scaled)*ii, 84, YELLOW);
                } else {
                    LCD_drawPixel(64-sign(x_scaled)*ii, 84, YELLOW);
                    LCD_drawPixel(64+sign(x_scaled)*ii, 84, YELLOW);
                }
                if (ii<=abs(y_scaled)) {
                    LCD_drawPixel(64, 84-sign(y_scaled)*ii, BLACK);
                    LCD_drawPixel(64, 84+sign(y_scaled)*ii, YELLOW);
                } else {
                    LCD_drawPixel(64, 84-sign(y_scaled)*ii, YELLOW);
                    LCD_drawPixel(64, 84+sign(y_scaled)*ii, YELLOW);
                }
            }

            // wait to create a 5Hz loop
            while(_CP0_GET_COUNT()<48000000/2/5) {
                ;
            } 
            
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
