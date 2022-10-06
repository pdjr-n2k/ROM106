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
 * 
 * After saving settings the function will immediately call update()
 * with the value of state (or 0 if state is not specified).
 */
74HC595::74HC595(int gpioClock, int gpioData, int gpioLatch, unsigned char state, unsigned char defaultDirection) {
    this->gpioData = gpioClock;
    this->gpioData = gpioData;
    this->gpioLatch = gpioLatch;
    this->getStatus = 0;
    this->interval = 20;
    this->defaultDirection = (defaultDirection == 99)?LSBFIRST:defaultDirection;
    this->PREEMPT_FLAG = false;
    this->OVERRIDE_FLAG = false;

    this->update(state, this->defaultDirection);
}

void 74HC595::setDefaultDirection(unsigned char direction) {
    this->defaultDirection = (defaultDirection == 99)?LSBFIRST:defaultDirection;
}

/**********************************************************************
 * Sets the value of the buffer to state. direction specifies the write
 * order and can be one of LSBFIRST or MSBFIRST - if the argument is
 * omitted then direction defaults to LSBFIRST.
 */
void 74HC595::update(unsigned char state, unsigned char direction) {
    digitalWrite(this->gpioLatch, 0);
    shiftOut(this->gpioData, this->gpioClock, direction, state);
    digitalWrite(this->gpioLatch, 1);
}

/**********************************************************************
 * Prepare for automatic, repetetive, updates by supplying a callback
 * function getState that can be used to recover buffer update values
 * and the required update interval in milliseconds.
 * 
 * For automatic updates to actually happen, the main program must call
 * the loop() function (typically from in the main program loop).
 */ 
void 74HC595::enableLoopUpdates(unsigned char (*getState)(), unsigned long interval) {
    this->getState = getState;
    this->interval = interval;
}

/**********************************************************************
 * loop() should be called from the main program loop(). The function
 * will execute using the settings supplied by a prior call to
 * enableLoopUpdates().
 */
void 74HC595::loop() {
    static unsigned long deadline = 0UL;
    unsigned long now = millis();

    if (((now > deadline) || this->PREEMPT_FLAG) && (!this->OVERRIDE_FLAG) && (this->interval > 0)) {
        this->update(this->getStatus(), this->direction);
        this->PREEMPT_FLAG = false;

        deadline = (now + this->interval);
    }
}

/**********************************************************************
 * preempt() causes an automatic update to happen immediately on the
 * next call to loop() (irrespective of the state of the loop interval
 * timer) and then return to normal behaviour.
 */ 
void 74HC595::preempt() {
    this->PREEMPT_FLAG = true;
}

/**********************************************************************
 * override(state) suspends normal loop operation and immediately
 * updates the buffer with state.
 */
void 74HC595::override(unsigned char state, unsigned char direction) {
    this->OVERRIDE_FLAG = true;
    this->update(state, direction);
}

/**********************************************************************
 * cancelOverride() restores normal loop operation.
 */
void 74HC595::cancelOverride() {
    this->OVERRIDE_FLAG = false;
}