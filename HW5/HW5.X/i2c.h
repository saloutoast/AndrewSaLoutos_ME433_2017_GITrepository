/*  Header file for I2C communication library
 * File:   i2c.h
 * Author: Andrew
 *
 * Created on April 11, 2017, 11:14 PM
 */

#ifndef I2C_H
#define	I2C_H

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

// helps implement use I2C2 as a master without using interrupts

void I2C2_master_setup(void);              // set up I2C2 as a master, at 100 kHz

void I2C2_master_start(void);              // send a START signal
void I2C2_master_restart(void);            // send a RESTART signal
void I2C2_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char I2C2_master_recv(void);      // receive a byte of data
void I2C2_master_ack(int val);             // send an ACK (0) or NACK (1)
void I2C2_master_stop(void);               // send a stop

#endif	/* I2C_H */

