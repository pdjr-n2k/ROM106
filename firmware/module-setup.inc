/**********************************************************************
 * module-setup.inc
 */
 
/**********************************************************************
 * We are using Button debouncers on our switch input channels and we
 * need to initialise these using the begin() method.
 */
for (unsigned int i = 0; i < ELEMENTCOUNT(SWITCH_INPUTS); i++) SWITCH_INPUTS[i].begin();

/**********************************************************************
 * Clear switchbank status buffer.
 */
N2kResetBinaryStatus(SWITCHBANK_STATUS);
