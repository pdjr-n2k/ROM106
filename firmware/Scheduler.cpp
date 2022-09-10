#include "Arduino.h"
#include "Scheduler.h"

/**********************************************************************
 * Create a new Scheduler with the process interval specified by
 * <loopInterval>.  The specified interval is the frequency at which
 * the scheduler will check to see if a callback should be executed, so
 * its best if this is frequent.
 */
 
Scheduler::Scheduler(unsigned long loopInterval) {
    this->loopInterval = loopInterval;
}

/**********************************************************************
 * This function must be called from the main loop(). It will execute
 * any scheduled callback function and then delete it from the
 * collection of scheduled callback functions (unless the callback was
 * scheduled with a repeat flag in which cast the callback will be
 * re-scheduled).
 */

void Scheduler::loop() {
    static unsigned long next = 0UL;
    unsigned long now = millis();

    if (now > next) {
        for (unsigned int i = 0; i < 10; i++) {
            if (this->scheduledEvents[i].next >= now) {
                this->scheduledEvents[i].func(this->scheduledEvents[i].opcode);
                this->scheduledEvents[i].next = (this->scheduledEvents[i].repeat)?(now + this->scheduledEvents[i].interval):0UL;
            }
        }
        next = (now + this->loopInterval);
    } 
}

bool Scheduler::schedule(void (*func)(), unsigned long interval, bool repeat, unsigned int opcode) {
    bool retval = false;

    for (unsigned int i = 0; i < 10; i++) {
        if (this->scheduledEvents[i].interval == 0UL) {
            this->scheduledEvents[i].func = func;
            this->scheduledEvents[i].interval = interval;
            this->scheduledEvents[i].next = (millis() + interval);
            this->scheduledEvents[i].repeat = repeat;
            this->scheduledEvents[i].opcode = opcode;
            retval = true;
            break;
        }
    }
    return(retval);
}

