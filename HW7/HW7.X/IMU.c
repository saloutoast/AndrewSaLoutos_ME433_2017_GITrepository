#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "IMU.h"
#include "i2c.h"

// function to initialize the IMU chip
// I2C slave address is 0b1101011
// WHO_AM_I register address: 0x0F
#define SLAVE_ADDR 0b11010110 // default address has last bit set to write

void IMU_init(void) { // initialize IMU registers
    
    ANSELBbits.ANSB2 = 0; // turn off analog functionality so pins will work with I2C2
    ANSELBbits.ANSB3 = 0;
    
    I2C2_master_setup(); // intiailize communications
    
    I2C2_master_start();
    I2C2_master_send(SLAVE_ADDR); 
    I2C2_master_send(0x10); // write to CTRL1_XL register (sample rate of 1.66 kHz, 2g sensitivity, 100 Hz filter)
    I2C2_master_send(0b10000010);
    I2C2_master_stop();
    
    I2C2_master_start();
    I2C2_master_send(SLAVE_ADDR); 
    I2C2_master_send(0x11); // write to CTRL2_G register (sample rate of 1.66 kHz, 1000 dps)
    I2C2_master_send(0b10001000);
    I2C2_master_stop();
    
    I2C2_master_start();
    I2C2_master_send(SLAVE_ADDR);
    I2C2_master_send(0x12); // write to CTRL3_C register (set IF_INC bit)
    I2C2_master_send(0b00000100);
    I2C2_master_stop();   
}

void IMU_read_multiple(unsigned char reg, unsigned char * data, int length) { // read multiple consecutive registers
    
    int ii;
    
    I2C2_master_start();
    I2C2_master_send(SLAVE_ADDR); // write to send register
    I2C2_master_send(reg); // send first register address to read from
    I2C2_master_restart(); // restart to switch to read mode
    I2C2_master_send(SLAVE_ADDR | 1); // address with read bit
    for (ii=0; ii<length; ii+=1) { // read until array is filled
        data[ii] = I2C2_master_recv();
        if (ii<(length-1)) {
            I2C2_master_ack(0);
        } else {
            I2C2_master_ack(1);
        }
    }
    I2C2_master_stop();
}