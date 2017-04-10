// SPI functions for ME 433

#include "SPI.h"

#define SS1 LATBbits.LATB15

void initSPI1() { // function to initialize SPI1 communication
    TRISBbits.TRISB15 = 0;
    SS1 = 1;
    
    // PIC pins: SS1 (RPB15)), SDO1 (RPB13), SDI1 (RPB8), SCK1 (RPB14)
    SDI1Rbits.SDI1R = 0b0100; // SDI1
    RPB13Rbits.RPB13R = 0b0011; // SDO1
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


void setVoltage(char channel, unsigned char voltage) { // function to set DAC voltage 
    
    unsigned char byte1 = 0;
    unsigned char byte2 = 0;
    
    if (channel == 0) { // write to VoutA
        byte1 = (voltage  >> 4);
        byte1 |= (0b0111 << 4);
        byte2 = (voltage << 4);
    }
    else if (channel == 1) { // write to VoutB
        byte1 = (voltage  >> 4);
        byte1 |= (0b1111 << 4);
        byte2 = (voltage << 4);
    }
    SS1 = 0; // write 2 bytes to DAC
    SPI1_io(byte1);
    SPI1_io(byte2);
    SS1 = 1;
}
