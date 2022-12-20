/**********************************************************************
 * module-loop.inc
 */
 
// Once the start-up settle period is over we can enter production by
  // executing our only substantive function.
  if (SWITCHBANK_INSTANCE < 253) transmitSwitchbankStatusMaybe();

  // Process relay operation queue.
  processRelayOperationQueueMaybe();