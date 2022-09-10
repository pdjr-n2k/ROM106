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
    Scheduler(unsigned long loopInterval = 20UL);
    bool schedule(void (*func)(), unsigned long interval, bool repeat = false);
    void loop();

protected:

private:
    struct schduledEvent { void (*func)(), unsigned long interval, unsigned long next, bool repeat = false };
    scheduledEvent[] scheduledEvents = { {0,0UL,0UL,false},{0,0UL,0UL,false},{0,0UL,0UL,false},{0,0UL,0UL,false},{0,0UL,0UL,false},{0,0UL,0UL,false},{0,0UL,0UL,false},{0,0UL,0UL,false},{0,0UL,0UL,false},{0,0UL,0UL,false} };
    unsigned long loopInterval;

}

#endif