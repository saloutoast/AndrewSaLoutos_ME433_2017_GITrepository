// I/O expander functions for ME 433, HW 5
// Read from and write to pins on the MCP23008 I/O expander chip

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "expander.h"

void initExpander(void) { // initializes the I/O expander
    
    ANSELBbits.ANSB2 = 0; // turn off analog functionality so pins will work with I2C2
    ANSELBbits.ANSB3 = 0;
    
    I2C2_master_setup();
    
    // define slave address? initialize registers and pull up resistors
    
}

void setExpander(char pin, char level) { // sets values of pins on I/O expander
    ; 
}

char getExpander(void) { // returns values of pins on I/O expander
    ;
}