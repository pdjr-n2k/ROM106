#include "Arduino.h"
#include "Scheduler.h"

/**********************************************************************
 * Create a new Scheduler with the process interval specified by
 * <loopInterval>.  The specified interval is the frequency at which
 * the scheduler will check to see if a callback should be executed, so
 * its best if this is frequent.
 */
 
Scheduler::Scheduler(unsigned long loopInterval) {
    this->scheduledEvents = NULL;
    this->loopInterval = loopInterval;
}

/**********************************************************************
 * This function must be called from the main loop(). It will execute
 * any scheduled callback functions and then delete it from the
 * collection of scheduled callback functions (unless the callback was
 * scheduled with a repeat flag in which cast the callback will be
 * re-scheduled).
 */

void Scheduler::loop() {
    static unsigned long deadline = 0UL;
    unsigned long now = millis();
    struct scheduledEvent *ptr = this->scheduledEvents;

    if ((now > deadline) && (ptr)) {
        while (ptr) {
            if (ptr->deadline >= now) {
                ptr->func();
                if (!ptr->repeat) {
                    if (this->scheduledEvents == ptr) {
                        this->scheduledEvents = ptr->next;
                    } else {
                        
                    }
                } 
                
                this->scheduledEvents[i].next = (this->scheduledEvents[i].repeat)?(now + this->scheduledEvents[i].interval):0UL;
            }
            ptr = ptr->next;
        }
        deadline = (now + this->loopInterval);
    } 
}

/**********************************************************************
 * Schedule <func> for callback in <interval> milliseconds. If <repeat>
 * is omitted or false, then the <func> will be called once, otherwise
 * it will be called repeatedly every <interval> milliseconds.
 */

bool Scheduler::schedule(void (*func)(), unsigned long interval, bool repeat) {
    bool retval = false;

    for (unsigned int i = 0; i < 10; i++) {
        if (this->scheduledEvents[i].next == 0UL) {
            this->scheduledEvents[i].func = func;
            this->scheduledEvents[i].interval = interval;
            this->scheduledEvents[i].next = (millis() + interval);
            this->scheduledEvents[i].repeat = repeat;
            retval = true;
            break;
        }
    }
    return(retval);
}

