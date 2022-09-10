#ifndef SCHEDULER_H
#define SCHEDULER_H

class Scheduler {

public:
    Scheduler(unsigned long processInterval);
    bool schedule(unsigned long when, void (*func)(), unsigned int opcode);
    void loop();

protected:

private:
    struct schduledEvent { unsigned long when, void (*func)(), unsigned int opcode = 0 };
    scheduledEvent[] scheduledEvents = { {0UL,0,0},{0UL,0,0},{0UL,0,0},{0UL,0,0},{0UL,0,0},{0UL,0,0},{0UL,0,0},{0UL,0,0},{0UL,0,0},{0UL,0,0} };
    unsigned long loopInterval = 100UL;

}

#endif