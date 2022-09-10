#include "Arduino.h"
#include "Scheduler.h"

Scheduler::Scheduler(unsigned long millis) {
    this->loopInterval = millis;
}

bool Scheduler::schedule(unsigned long when, void (*func)(), unsigned int opcode) {
    bool retval = false;

    for (unsigned int i = 0; i < 10; i++) {
        if (this->scheduledEvents[i].when == 0UL) {
            this->scheduledEvents[i].when = (millis() + when);
            this->scheduledEvents[i].func = func;
            this->scheduledEvents[i].opcode = opcode;
            retval = true;
            break;
        }
    }
    return(retval);
}

void Scheduler::loop() {
    static unsigned long next = 0UL;
    unsigned long now = millis();

    if (now > next) {
        for (unsigned int i = 0; i < 10; i++) {
            if (this->scheduledEvents[i].when >= now) {
                this->scheduledEvents[i].func(this->scheduledEvents[i].opcode);
                this->scheduledEvents[i].when = 0UL;
            }
        }
        next = (now + this->loopInterval);
    } 
}