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

#include "app.h"
#include <stdio.h>
#include "i2c.h"
#include "ILI9163C.h"
#include "IMU.h"

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

unsigned char IMU_data[14]; // arrays to store IMU information before and after shifting
signed short combined_data[7]; // [temp, x_g, y_g, z_g, x_xl, y_xl, z_xl]
    
unsigned char msg[100];

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

    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
    TRISAbits.TRISA4 = 0; // pin RA4 is an output pin (LED)
    LATAbits.LATA4 = 1; // default output on RA4 is high (LED is on)
    TRISBbits.TRISB4 = 1; // pin RB4 is an input pin (push button)
    
    SPI1_init(); // initialize SPI peripheral
    
    LCD_init(); // initialize the LCD screen
    
    IMU_init(); // initialize the IMU sensor
    
    LCD_clearScreen(0xFFFF);
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
            if(_CP0_GET_COUNT() > 4800000) { // 24 MHz / 4800000 = 5 Hz
                _CP0_SET_COUNT(0);
                IMU_read_multiple( 0x20, IMU_data, 14);
                int i;
                for (i=0; i<7; i+=1) {
                    combined_data[i] = ((IMU_data[(2*i)+1] << 8) | IMU_data[(2*i)]);
                }
                if (combined_data[4] > 0) {
                    LCD_drawBar_x(0,64,WHITE,128,5);
                    LCD_drawBar_x(64,64,CYAN,combined_data[4]/500,5); // draw bar for x component, normalize data to be about 25 pixels at 1 g
                } else {
                    LCD_drawBar_x(0,64,WHITE,128,5);
                    LCD_drawBar_x( (64+(combined_data[4]/500)) , 64, BLUE, (-1)*(combined_data[4]/500), 5);
                }
                if (combined_data[5] > 0) {
                    LCD_drawBar_y(62,0,WHITE,128,5);
                    LCD_drawBar_y(62,67,MAGENTA,combined_data[5]/500,5); // draw bar for y component, normalize data to be about 25 pixels at 1 g
                } else {
                    LCD_drawBar_y(62,0,WHITE,128,5);
                    LCD_drawBar_y(62, (67+(combined_data[5]/500)) , RED, (-1)*(combined_data[5]/500), 5);
                }
                sprintf(msg, "x: %d      ", combined_data[4]);
                LCD_dispString(msg, 10, 10, 0, 0xFFFF);
                sprintf(msg, "y: %d      ", combined_data[5]);
                LCD_dispString(msg, 75, 10, 0, 0xFFFF);

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
