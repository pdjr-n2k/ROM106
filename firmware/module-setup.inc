/**********************************************************************
 * module-setup.inc
 */

// Initialise all core GPIO pins.
int rcopins[] = GPIO_RELAY_CONTROL_OUTPUT_PINS;
for (unsigned int i = 0 ; i < ELEMENTCOUNT(rcopins); i++) pinMode(rcopins[i], OUTPUT);
   

/**********************************************************************
 * Clear switchbank status buffer.
 */
N2kResetBinaryStatus(SWITCHBANK_STATUS);
