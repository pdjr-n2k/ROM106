/**********************************************************************
 * Scheduler - callback scheduler.
 * 2022 (c) Paul Reeve.
 * 
 * Example
 * 
 * #define LOOP_INTERVAL 100UL
 * 
 * Scheduler myScheduler(LOOP_INTERVAL);
 * 
 * void setup() {
 *   myScheduler.schedule(2000UL, myCallbackFunction);
 * }
 * 
 * void loop() {
 *   mySchedular.loop();
 * }
 * 
 * void myCallbackFunction() {
 *   Serial.println("Hello world");
 * }
 * 
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

class Scheduler {

public:
    Scheduler(unsigned long loopInterval);
    bool schedule(void (*func)(), unsigned long interval, bool repeat, unsigned int opcode);
    void loop();

protected:

private:
    struct schduledEvent { void (*func)(), unsigned long interval, unsigned long next, bool repeat = false, unsigned int opcode = 0 };
    scheduledEvent[] scheduledEvents = { {0,0UL,0UL,false,0},{0,0UL,0UL,false,0},{0,0UL,0UL,false,0},{0,0UL,0UL,false,0},{0,0UL,0UL,false,0},{0,0UL,0UL,false,0},{0,0UL,0UL,false,0},{0,0UL,0UL,false,0},{0,0UL,0UL,false,0},{0,0UL,0UL,false,0} };
    unsigned long loopInterval = 100UL;

}

#endif