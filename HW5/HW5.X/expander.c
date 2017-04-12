// I/O expander functions for ME 433, HW 5
// Read from and write to pins on the MCP23008 I/O expander chip

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "expander.h"

#define SLAVE_ADDR 0b01000000 // default address has last bit set to write

void initExpander(void) { // initializes the I/O expander
    
    ANSELBbits.ANSB2 = 0; // turn off analog functionality so pins will work with I2C2
    ANSELBbits.ANSB3 = 0;
    
    I2C2_master_setup();
    
    I2C2_master_start(); // send initialization register values
    I2C2_master_send(SLAVE_ADDR);
    I2C2_master_send(0x0); // start with 0th register, IODIR, and increment from there
    I2C2_master_send(0b11110000); // IODIR register (1 is input and 0 is output)
    I2C2_master_send(0); // IPOL, not using
    I2C2_master_send(0); // GPINTEN (interrupts), not using
    I2C2_master_send(0); // DEFVAL (interrupts), not using
    I2C2_master_send(0); // INTCON (interrupts), not using
    I2C2_master_send(0); // IOCON (configuration bits), don't need to change any from defaults
    I2C2_master_send(0b10000000); // GPPU (pull-up resistors), set pull-up resistor for pushbutton input
    I2C2_master_send(0); // INTF (interrupts), not using
    I2C2_master_send(0); // INTCAP (interrupts), not using
    I2C2_master_send(0b00000001); // PORT (GPIO), set LED output pin to high
//  I2C2_master_send(0); // OLAT (output latch), can just use PORT register instead of this register
    I2C2_master_stop();    
}

void setExpander(char pin, char level) { // sets values of pins on I/O expander
    ; 
}

char getExpander(void) { // returns values of pins on I/O expander
    ;
}