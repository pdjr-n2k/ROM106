/**********************************************************************
 * LedDisplay - handle a parallel buffer based LED display.
 * 2022 (c) Paul Reeve.
 */

#include "LedDisplay.h"

LedDisplay::LedDisplay(unsigned char (*getStatus)(), unsigned long interval, int gpioData, int gpioClock, int gpioLatch) {
    this->getStatus = getStatus;
    this->interval = interval;
    this->gpioData = gpioData;
    this->gpioClock = gpioClock;
    this->gpioLatch = gpioLatch;
    this->PREEMPT_FLAG = false;
    this->OVERRIDE_FLAG = false;
}


void LedDisplay::loop() {
    static unsigned long deadline = 0UL;
    unsigned long now = millis();

    if (((now > deadline) || this->PREEMPT_FLAG) && (!this->OVERRIDE_FLAG)) {
        digitalWrite(this->gpioLatch, 0);
        shiftOut(this->gpioData, this->gpioClock, LSBFIRST, this->getStatus());
        digitalWrite(this->gpioLatch, 1);

        this->PREEMPT_FLAG = false;
        deadline = (now + this->interval);
  }
}

void LedDisplay::preempt() {
    this->PREEMPT_FLAG = true;
    digitalWrite(this->gpioLatch, 0);
    shiftOut(this->gpioData, this->gpioClock, LSBFIRST, this->getStatus());
    digitalWrite(this->gpioLatch, 1);
}

void LedDisplay::override(unsigned char state) {
    this->OVERRIDE_FLAG = true;
    digitalWrite(this->gpioLatch, 0);
    shiftOut(this->gpioData, this->gpioClock, LSBFIRST, state);
    digitalWrite(this->gpioLatch, 1);
}

void LedDisplay::cancelOverride() {
    this->OVERRIDE_FLAG = false;
}