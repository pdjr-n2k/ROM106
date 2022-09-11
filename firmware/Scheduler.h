/**********************************************************************
 * Scheduler - callback scheduler.
 * 2022 (c) Paul Reeve.
 * 
 * Example:
 * 
 * #define LOOP_INTERVAL 20UL
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
    struct scheduledEvent { void (*func)(), unsigned long interval, unsigned long deadline, bool repeat = false, schduledEvent *next };
    scheduledEvent *scheduledEvents = NULL;
    unsigned long loopInterval;

}

#endif