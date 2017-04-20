/* 
 * File:   IMU.h
 * Author: Andrew
 *
 * Created on April 19, 2017, 9:17 PM
 */

#ifndef IMU_H
#define	IMU_H

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

void IMU_init(void); // set up IMU sensor

void IMU_read_multiple(unsigned char reg, unsigned char * data, int length); // read multiple consecutive registers

#endif	/* IMU_H */

