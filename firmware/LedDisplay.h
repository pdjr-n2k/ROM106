/**********************************************************************
 * LedDisplay - handle a parallel buffer based LED display.
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

#ifndef LEDDISPLAY_H
#define LEDDISPLAY_H

#include <Arduino.h>

class LedDisplay {

public:
    LedDisplay(unsigned char *defaultBuffer, int gpioData, int gpioClock, int gpioLatch);
    void loop();
    void setImmediate();
    void override(unsigned char state);
    void cancelOverride();

protected:

private:
    struct Callback { void (*func)(); unsigned long interval; unsigned long when; bool repeat; };
    Callback callbacks[10] = { {NULL,0UL,0UL,false},{NULL,0UL,0UL,false},{NULL,0UL,0UL,false},{NULL,0UL,0UL,false},{NULL,0UL,0UL,false},{NULL,0UL,0UL,false},{NULL,0UL,0UL,false},{NULL,0UL,0UL,false},{NULL,0UL,0UL,false},{NULL,0UL,0UL,false} };
    int size = 0;
    unsigned long loopInterval;

};

#endif