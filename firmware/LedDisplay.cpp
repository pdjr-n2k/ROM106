/**********************************************************************
 * LedDisplay - handle a parallel buffer based LED display.
 * 2022 (c) Paul Reeve.
 */

#include "LedDisplay.h"

LedDisplay::LedDisplay(unsigned char *defaultBuffer, unsigned long interval, int gpioData, int gpioClock, int gpioLatch) {
    this->defaultBuffer = defaultBuffer;
    this->interval = interval;
    this->gpioData = gpioData;
    this->gpioClock = gpioClock;
    this->gpioLatch = gpioLatch;
    this->preempt = false;
    this->override = false;
}


void LedDisplay::loop() {
    static unsigned long deadline = 0UL;
    unsigned long now = millis();

    if (((now > deadline) || this->preempt) && (!this->override)) {
        digitalWrite(this->gpioLatch, 0);
        shiftOut(this->gpioData, this->gpioClock, LSBFIRST, &this->defaultBuffer);
        digitalWrite(this->gpioLatch, 1);

        this->preempt = false;
        deadline = (now + this->interval);
  }
}

void LedDisplay::preempt(unsigned char state) {
    this->preempt = true;
    digitalWrite(this->gpioLatch, 0);
    shiftOut(this->gpioData, this->gpioClock, LSBFIRST, state);
    digitalWrite(this->gpioLatch, 1);
}

void LedDisplay::override(unsigned char state) {
    this->override = true;

}
    void cancelOverride();

protected:

private:
    unsigned char *defaultBuffer;
    int gpioData;
    int gpioClock;
    int gpioLatch;
    bool preempt;
    bool override;

};

#endif