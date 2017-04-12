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

void setExpander(char pin, char level) { // sets value of a pin on I/O expander, starting with pin 0
    char byte = 0;
    (byte | level) << pin;
    
    I2C2_master_start(); // begin communication
    I2C2_master_send(SLAVE_ADDR); // address defaults to write
    I2C2_master_send(0x09); // send to PORT (GPIO) register
    I2C2_master_send(byte); // send desired level on desired pin
    I2C2_master_stop(); // end communication  
}

char getExpander(void) { // returns values of pins on I/O expander
    char pin_vals;
    
    I2C2_master_start(); // begin communication
    I2C2_master_send(SLAVE_ADDR); // address defaults to write
    I2C2_master_send(0x09); // identify POIR (GPIO) register, to read from later
    I2C2_master_restart(); // restart communication
    I2C2_master_send( SLAVE_ADDR|1 ); // address, with last bit changed to 1 (for reading)
    pin_vals = I2C2_master_recv(); // save the value returned from the GPIO register
    I2C2_master_ack(1); // tell slave to stop communicating
    I2C2_master_stop(); // end communications 
    
    return pin_vals;
}