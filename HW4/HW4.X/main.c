#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

#include "SPI.h"
#include <math.h>

// DEVCFG0
#pragma config DEBUG = 10 // no debugging
#pragma config JTAGEN = 0 // no jtag
#pragma config ICESEL = 11 // use PGED1 and PGEC1
#pragma config PWP = 111111111 // no write protect
#pragma config BWP = 1 // no boot write protect
#pragma config CP = 1 // no code protect

// DEVCFG1
#pragma config FNOSC = 011 // use primary oscillator with pll
#pragma config FSOSCEN = 0 // turn off secondary oscillator
#pragma config IESO = 0 // no switching clocks
#pragma config POSCMOD = 10 // high speed crystal mode
#pragma config OSCIOFNC = 1 // free up secondary osc pins
#pragma config FPBDIV = 00 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = 10 // do not enable clock switch
#pragma config WDTPS = 10100 // slowest wdt (1:1048576 ratio, 1:1 ratio is 00000)
#pragma config WINDIS = 1 // no wdt window
#pragma config FWDTEN = 0 // wdt off by default
#pragma config FWDTWINSZ = 11 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = 001 // divide input clock to be in range 4-5MHz (8MHz / 2 = 4 MHz)
#pragma config FPLLMUL = 111 // multiply clock after FPLLIDIV (4Mhz * 24 = 96 MHz))
#pragma config FPLLODIV = 001 // divide clock after FPLLMUL to get 48MHz (96 MHz / 2 = 48 MHz !!))
#pragma config UPLLIDIV = 001 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB (8MHz / 2 = 4 MHz)
#pragma config UPLLEN = 0 // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = 0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 1 // USB BUSON controlled by USB module


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
    
    initSPI1(); // initialize SPI peripheral
    
    __builtin_enable_interrupts();
    
//    char voltage = 0;
    char tri_volt = 0;
//    char sin_volt = 0;
//    char sin_track = 0;
    
    while(1) { // update waves at 1 kHz frequency
        if (_CP0_GET_COUNT() > 24000) { // 24 MHz / 1 kHz = 24000
            _CP0_SET_COUNT(0);
            // Triangle wave at 5 Hz
            if (tri_volt < 254) {
                tri_volt = tri_volt + (255/200); // 1 kHz / 5 Hz = 200
            } else {
                tri_volt = 0;
            }
            setVoltage(0, tri_volt);
            setVoltage(1, 0);
            // Sine wave at 10 Hz
//            if (sin_track < 100) {
//                sin_track = sin_track + 1; // 1 kHz / 10 Hz = 100
//            } else { 
//                sin_track = 0;
//            }
        }
//        setVoltage(0, voltage);
//        if (_CP0_GET_COUNT() > 12000000) { // 1/2 second per switch
//            if (voltage == 0) {
//                voltage = 0b01111111;
//            } else {
//                voltage = 0;
//            }
//            _CP0_SET_COUNT(0);
//        }
    }
}