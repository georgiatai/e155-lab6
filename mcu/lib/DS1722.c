// DS1722.c
// Georgia Tai
// ytai@g.hmc.edu
// Nov. 1, 2025
// Configuration and communication functions for DS1722 temperature sensor

#include "DS1722.h"

// initialize DS1722
void initTempSensor() {

    // set chip enable (CE) pin high
    digitalWrite(DS1722_CE_PIN, PIO_HIGH);

    spiSendReceive(0x80);
    spiSendReceive(0xE0);

    // set chip enable (CE) pin low
    digitalWrite(DS1722_CE_PIN, PIO_LOW);

}

void setResolution(char resolution) {
    
    // set chip enable (CE) pin high
    digitalWrite(DS1722_CE_PIN, PIO_HIGH);
    spiSendReceive(0x80);

    // send write resolution command based on desired resolution
    // config register = {3'b111, 1SHOT, R2, R1, R0, SD}
    if (resolution == 8) {
        spiSendReceive(0xE0);      // {R2,R1,R0} = 000
        //delay_millis(TIM15, 100);
    }
    else if (resolution == 9) {
        spiSendReceive(0xE2);      // {R2,R1,R0} = 001
        //delay_millis(TIM15, 100);
    }
    else if (resolution == 10) {
        spiSendReceive(0xE4);      // {R2,R1,R0} = 010
        //delay_millis(TIM15, 100);
    }
    else if (resolution == 11) {
        spiSendReceive(0xE6);      // {R2,R1,R0} = 011
        //delay_millis(TIM15, 100);
    }
    else if (resolution == 12) {
        spiSendReceive(0xE8);      // {R2,R1,R0} = 1xx
        //delay_millis(TIM15, 100);
    }

    // set chip enable (CE) pin low
    digitalWrite(DS1722_CE_PIN, PIO_LOW);
}

// get temp
double readTemp(){

    // obtain MSB data
    digitalWrite(DS1722_CE_PIN, PIO_HIGH);
    spiSendReceive(0x02);                   // MSB address = 02h
    uint8_t msb = spiSendReceive(0x00);
    digitalWrite(DS1722_CE_PIN, PIO_LOW);

    // obtain LSB data
    digitalWrite(DS1722_CE_PIN, PIO_HIGH);
    spiSendReceive(0x01);                   // LSB address = 01h
    uint8_t lsb = spiSendReceive(0x00);
    digitalWrite(DS1722_CE_PIN, PIO_LOW);

    // calculate temperature
    uint16_t rawdata = (msb << 8) | lsb;  // raw data of 12-bit 2's complement
    int16_t  rawtemp = rawdata >> 4;
    if (rawtemp & 0x0800)                // check sign of raw temperature data
        rawtemp = rawtemp - 0x1000;      // sign extend
    double temp = rawtemp * 0.0625f;

    return temp;
}
