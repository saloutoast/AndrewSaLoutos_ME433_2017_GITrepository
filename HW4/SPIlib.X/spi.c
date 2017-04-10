// SPI functions for ME 433

#include "spi.h"

#define SS1 LATBbits.LATB15

void initSPI1() { // function to initialize SPI1 communication
    TRISBbits.TRISB15 = 0;
    SS1 = 1;
    
    // PIC pins: SS1 (RPB15)), SDO1 (RPB13), SDI1 (RPB8), SCK1 (RPB14)
    SDI1Rbits.SDI1R = 0b0100; // SDI1
    RPB13Rbits.RPB13 = 0b0011; // SDO1
    // Don't need to initialize SCK1?
    // Don't initialize SS1, it will be manually controlled as a digital output
    
    // Setup SPI1
    SPI1CON = 0;  // turn off the spi module and reset it
    SPI1BUF;  // clear the rx buffer by reading from it
    SPI1BRG = 0x1; // baud rate as fast as possible (20 MHz) [SPI1BRG = (80000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1;  // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on SPI 1
    
}


char SPI1_io(char write) {// function to write through SPI1
    SPI1BUF = write; // send a byte
    while(!SPI1STATbits.SPIRBF) { // receive response
        ;
    }
    return SPI1BUF;
}


void setVoltage(char channel, char voltage) { // function to set DAC voltage 
}