/**********************************************************************
 * LedDisplay - handle a parallel buffer based LED display.
 * 2022 (c) Paul Reeve.
 */

#ifndef LEDDISPLAY_H
#define LEDDISPLAY_H

#include <Arduino.h>

class LedDisplay {

public:
    LedDisplay(unsigned char (*getStatus)(), unsigned long interval, int gpioData, int gpioClock, int gpioLatch);
    void loop();
    void preempt();
    void override(unsigned char state);
    void cancelOverride();

protected:

private:
    unsigned char (*getStatus)();
    unsigned long interval;
    int gpioData;
    int gpioClock;
    int gpioLatch;
    bool PREEMPT_FLAG;
    bool OVERRIDE_FLAG;

};

#endif