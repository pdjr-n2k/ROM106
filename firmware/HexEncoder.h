/**********************************************************************
 * HexEncoder - operate a twin rotary hex encoder.
 * 2022 (c) Paul Reeve.
 * 
 * Example:
 * 
 * #define LOOP_INTERVAL 20UL
 * 
 * Scheduler myScheduler(LOOP_INTERVAL);
 * 
 * void setup() {
 *   myScheduler.schedule(2000UL, myCallbackFunction);
 * }
 * 
 * void loop() {
 *   mySchedular.loop();
 * }
 * 
 * void myCallbackFunction() {
 *   Serial.println("Hello world");
 * }
 * 
 */

#ifndef HEXENCODER_H
#define HEXENCODER_H

#include <Arduino.h>

class HexEncoder {

public:
    HexEncoder(unsigned int gpio1, unsigned int gpio2, unsigned int gpio4, unsigned int gpio8, unsigned int gpioHi, unsigned int gpioLo);
    unsigned char getAddress(;
protected:

private:
    unsigned int gpio1;

};

#endif