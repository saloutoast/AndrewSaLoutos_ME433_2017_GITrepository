/* SPI library header file
 * File:   spi.h
 * Author: Andrew
 *
 * Created on April 9, 2017, 6:57 PM
 */

#ifndef SPI_H

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

void initSPI1(); // function to initialize SPI1 communication
char SPI1_io(char write); // function to write through SPI1
void setVoltage(char channel, char voltage); // function to set DAC voltage 

#define	SPI_H

