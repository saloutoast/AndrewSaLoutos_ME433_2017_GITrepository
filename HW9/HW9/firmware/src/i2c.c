// I2C functions for ME 433, HW 5
// Implement I2C communication with MCP23008 chip, with no interrupts

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "i2c.h"

void I2C2_master_setup(void) {             // set up I2C2 as a master, at 100 kHz 
    I2C2BRG = 0xE9; // I2C2BRG = [1/(2*Fsck) - PGD]*Pblck - 2, PGD = 104ns, = 233 = 0xE9
    I2C2CONbits.ON = 1; // turn on the I2C2 module
}
    
void I2C2_master_start(void) {             // send a START signal
    I2C2CONbits.SEN = 1; // send the start bit
    while(I2C2CONbits.SEN) {;} // wait for the start bit to be sent
}    
    
void I2C2_master_restart(void) {            // send a RESTART signal
    I2C2CONbits.RSEN = 1; // send a restart
    while(I2C2CONbits.RSEN) {;} // wait for the restart to clear
}    
    
void I2C2_master_send(unsigned char byte) { // send a byte (either an address or data)
    I2C2TRN = byte; // if an address, bit 0 = 0 for write, 1 for read
    while(I2C2STATbits.TRSTAT) {;} // wait for the transmission to finish
    if(I2C2STATbits.ACKSTAT) { // if this is high, slave has not acknowledged
       ; // throw an error ("I2C2 Master: failed to receive ACK\r\n")
    }
}

unsigned char I2C2_master_recv(void) {      // receive a byte of data
    I2C2CONbits.RCEN = 1; // start receiving data
    while(!I2C2STATbits.RBF) {;} // wait to receive the data
    return I2C2RCV; // read and return the data
}    
    
void I2C2_master_ack(int val) {          // send an ACK (0) or NACK (1)
    I2C2CONbits.ACKDT = val; // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1; // send ACKDT
    while(I2C2CONbits.ACKEN) {;} // wait for ACK/NACK to be sent
}    
    
void I2C2_master_stop(void) {               // send a stop
    I2C2CONbits.PEN = 1; // comm is complete and master relinquishes bus
    while(I2C2CONbits.PEN) {;} // wait for the STOP to complete
}   