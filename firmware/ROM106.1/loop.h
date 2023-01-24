/**********************************************************************
 * loop.h
 */
 
processRelayOperationQueueMaybe();

if (PGN127501Scheduler.IsTime()) { PGN127501Scheduler.UpdateNextTime(); transmitPGN127501(); }
