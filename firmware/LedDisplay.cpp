/**********************************************************************
 * 74HC595.cpp - operate a 74HC595 serial-parallel buffer.
 * 2022 (c) Paul Reeve.
 */

#include "LedDisplay.h"

/**********************************************************************
 * Create a new 74HC595 instance and initialise it for managing a
 * serial-parallel 8-bit buffer.
 * 
 * getStatus should specify a callback function that can be used to
 * recover the status byte that will be used by loop() to update the
 * associated LED display.
 * 
 * interval specifies the interval in milliseconds between successive
 * LED updates bt loop().
 * 
 * gpioClock specifies the MPU digital pin that connects to the buffer
 * clock input.
 * 
 * gpioData specifies the MPU digital pin that connects to the buffer
 * serial data input.
 * 
 * gpioLatch specifies the MPU digital pin that connects to the buffer
 * latch input.
 */
74HC595::74HC595(int gpioClock, int gpioData, int gpioLatch, unsigned char state) {
    this->gpioData = gpioClock;
    this->gpioData = gpioData;
    this->gpioLatch = gpioLatch;
    this->getStatus = 0;
    this->interval = 20;
    this->PREEMPT_FLAG = false;
    this->OVERRIDE_FLAG = false;

    this->update(state);
}

void 74HC595::update(unsigned char state, int direction) {
    digitalWrite(this->gpioLatch, 0);
    shiftOut(this->gpioData, this->gpioClock, direction, state);
    digitalWrite(this->gpioLatch, 1);
}

void 74HC595::updateAutomatically(unsigned char (*getState)(), unsigned long interval) {
    this->getState = getState;
    this->interval = interval;
}

/**********************************************************************
 * loop() should be called from the main program loop(). Normally the
 * function will execute once every interval milliseconds by calling
 * getStatus() and using the returned value to update the buffer's
 * outputs.
 * 
 * This normal behaviour can be pre-empted and an immediate update be
 * triggered by a prior call to preempt(). After the update normal loop
 * behaviour will resume.
 * 
 * Normal behavior can be suspended and resumed by calls to override()
 * and cancelOverride().
 */
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

/**********************************************************************
 * preempt() will cause loop() to perform an update on its next
 * invocation (irrespective of the state of the interval timer) and
 * then return to normal behaviour.
 */ 
void LedDisplay::preempt() {
    this->PREEMPT_FLAG = true;
    digitalWrite(this->gpioLatch, 0);
    shiftOut(this->gpioData, this->gpioClock, LSBFIRST, this->getStatus());
    digitalWrite(this->gpioLatch, 1);
}

/**********************************************************************
 * override(state) suspends normal loop operation and immediately
 * updates the buffer with state.
 */
void LedDisplay::override(unsigned char state) {
    this->OVERRIDE_FLAG = true;
    digitalWrite(this->gpioLatch, 0);
    shiftOut(this->gpioData, this->gpioClock, LSBFIRST, state);
    digitalWrite(this->gpioLatch, 1);
}

/**********************************************************************
 * cancelOverride() restores normal loop operation.
 */
void LedDisplay::cancelOverride() {
    this->OVERRIDE_FLAG = false;
}