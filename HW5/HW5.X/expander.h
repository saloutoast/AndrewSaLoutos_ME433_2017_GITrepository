/* Header file for I/O expander library
 * File:   expander.h
 * Author: Andrew
 *
 * Created on April 11, 2017, 11:51 PM
 */

#ifndef EXPANDER_H
#define	EXPANDER_H

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

#include "i2c.h"

void initExpander(void); // initializes the I/O expander

void setExpander(char pin, char level); // sets values of pins on I/O expander
char getExpander(void); // returns values of pins on I/O expander

#endif	/* EXPANDER_H */

