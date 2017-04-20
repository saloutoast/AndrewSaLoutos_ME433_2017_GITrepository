#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro
#include <stdio.h>
#include "ILI9163C.h"
#include "i2c.h"
#include "IMU.h"

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt (1:1048576 ratio, 1:1 ratio is 00000)
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz (8MHz / 2 = 4 MHz)
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV (4Mhz * 24 = 96 MHz))
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz (96 MHz / 2 = 48 MHz !!))
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB (8MHz / 2 = 4 MHz)
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module


int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0; // pin RA4 is an output pin (LED)
    LATAbits.LATA4 = 1; // default output on RA4 is high (LED is on)
    TRISBbits.TRISB4 = 1; // pin RB4 is an input pin (push button)
    
    SPI1_init(); // initialize SPI peripheral
    
    LCD_init(); // initialize the LCD screen
    
    IMU_init(); // initialize the IMU sensor
    
    __builtin_enable_interrupts();
    
    LCD_clearScreen(0xFFFF);
    
    // WHO_AM_I test
    /*
    LCD_dispChar(0b01101001, 30, 30, BLACK, WHITE);
    
    I2C2_master_start();
    I2C2_master_send(0b11010110);
    I2C2_master_send(0b00001111);
    I2C2_master_restart();
    I2C2_master_send(0b11010111);
    char r = I2C2_master_recv();
    I2C2_master_ack(1);
    I2C2_master_stop();
    
    LCD_dispChar(r, 60, 60, BLACK, WHITE);   
    */
    
    unsigned char IMU_data[14]; // arrays to store IMU information before and after shifting
    signed short combined_data[7]; // [temp, x_g, y_g, z_g, x_xl, y_xl, z_xl]
    
    unsigned char msg[100];
    
    while(1) {
        if(_CP0_GET_COUNT() > 4800000) { // 24 MHz / 4800000 = 5 Hz
            _CP0_SET_COUNT(0);
            IMU_read_multiple( 0x20, IMU_data, 14);
            int i;
            for (i=0; i<7; i+=1) {
                combined_data[i] = ((IMU_data[(2*i)+1] << 8) | IMU_data[(2*i)]);
            }
            if (combined_data[4] > 0) {
                LCD_drawBar_x(0,64,WHITE,128,5);
                LCD_drawBar_x(64,64,CYAN,combined_data[4]/500,5); // draw bar for x component, normalize data to be about 25 pixels at 1 g
            } else {
                LCD_drawBar_x(0,64,WHITE,128,5);
                LCD_drawBar_x( (64+(combined_data[4]/600)) , 64, BLUE, (-1)*(combined_data[4]/500), 5);
            }
            if (combined_data[5] > 0) {
                LCD_drawBar_y(62,0,WHITE,128,5);
                LCD_drawBar_y(62,67,MAGENTA,combined_data[5]/500,5); // draw bar for y component, normalize data to be about 25 pixels at 1 g
            } else {
                LCD_drawBar_y(62,0,WHITE,128,5);
                LCD_drawBar_y(62, (67+(combined_data[5]/500)) , RED, (-1)*(combined_data[5]/500), 5);
            }
            sprintf(msg, "x: %d      ", combined_data[4]);
            LCD_dispString(msg, 10, 10, 0, 0xFFFF);
            sprintf(msg, "y: %d      ", combined_data[5]);
            LCD_dispString(msg, 75, 10, 0, 0xFFFF);
                      
        }
    }
    
}


