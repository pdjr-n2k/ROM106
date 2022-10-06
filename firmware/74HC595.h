/**********************************************************************
 * LedDisplay - handle a parallel buffer based LED display.
 * 2022 (c) Paul Reeve.
 */

#ifndef 74HC595_H
#define 74HC595_H

#include <Arduino.h>

class 74HC595 {

public:
    74HC595(int gpioClock, int gpioData, int gpioLatch, unsigned char state = 0, unsigned char defaultDirection = 99);
    void setDefaultDirection(unsigned char direction = 99);
    void update(unsigned char state, unsigned char direction = 99);
    void enableLoopUpdates(unsigned char (*getState)(), unsigned long interval);
    void loop();
    void preempt();
    void override(unsigned char state, unsigned char direction = 99);
    void cancelOverride();

protected:

private:
    unsigned char (*getStatus)();
    unsigned long interval;
    int gpioClock;
    int gpioData;
    int gpioLatch;
    unsigned char defaultDirection;
    bool PREEMPT_FLAG;
    bool OVERRIDE_FLAG;

};

#endif