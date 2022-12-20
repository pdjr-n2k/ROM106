/**********************************************************************
 * module-declarations.inc
 *
 * Contains declarations of types, variables, etc. that are required
 * for implementation of our application. Note that application
 * functions must be declared at the end of module-definitions.inc.
 */

/**********************************************************************
 * SWITCH_INPUTS - array of debounced GPIO inputs which connect the
 * module's external switch inputs.
 */
Button SWITCH_INPUTS[] = { Button(GPIO_SWITCH_INPUT1), Button(GPIO_SWITCH_INPUT2), Button(GPIO_SWITCH_INPUT3), Button(GPIO_SWITCH_INPUT4), Button(GPIO_SWITCH_INPUT5), Button(GPIO_SWITCH_INPUT6), Button(GPIO_SWITCH_INPUT7), Button(GPIO_SWITCH_INPUT8) };

/**********************************************************************
 * SWITCHBANK_STATUS - working storage for holding the most recently
 * read state of the Teensy switch inputs in the format used by the
 * NMEA2000 library.
 */
tN2kBinaryStatus SWITCHBANK_STATUS;

/**********************************************************************
 * processSwitchInputsMaybe() should be called directly from loop().
 * The function checks switch states every SWITCH_PROCESS_INTERVAL and
 * updates SWITCHBANK_STATUS with any changes. If a change is made,
 * then a call is made to transmit the update over NMEA.
 */
void processSwitchInputsMaybe() {
  static unsigned long deadline = 0UL;
  unsigned long now = millis();
  bool updated = false;
  tN2kOnOff switchStatus = N2kOnOff_Unavailable;

  if (now > deadline) {
    for (unsigned int i = 0; i < ELEMENTCOUNT(SWITCH_INPUTS); i++) {
      if (SWITCH_INPUTS[i].toggled()) {
        switchStatus = (SWITCH_INPUTS[i].read() == Button::PRESSED)?N2kOnOff_On:N2kOnOff_Off;
        if (switchStatus != N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, (i + 1))) {
          N2kSetStatusBinaryOnStatus(SWITCHBANK_STATUS, switchStatus, (i + 1));
          updated = true;
        }
      }
    }
    if (updated) transmitSwitchbankStatusMaybe(true);
    deadline = (now + SWITCH_PROCESS_INTERVAL);
  }
}
  
/**********************************************************************
 * transmitSwitchbankStatusMaybe() should be called directly from
 * loop(). The function proceeds to transmit a switchbank binary status
 * message if PGN127501_TRANSMIT_INTERVAL has elapsed or <force> is true.
 */
void transmitSwitchbankStatusMaybe(bool force) {
  static unsigned long deadline = 0UL;
  unsigned long now = millis();

  if ((now > deadline) || force) {
    transmitPGN127501();
    digitalWrite(GPIO_POWER_LED, 1);
    TRANSMIT_LED_STATE = 1;
    deadline = (now + PGN127501_TRANSMIT_INTERVAL);
  }
}

/**********************************************************************
 * Assemble and transmit a PGN 127501 Binary Status Update message
 * reflecting the current switchbank state.
 */
void transmitPGN127501() {
  static tN2kMsg N2kMsg;

  if (MODULE_INSTANCE < 253) {
    SetN2kPGN127501(N2kMsg, MODULE_INSTANCE, SWITCHBANK_STATUS);
    NMEA2000.SendMsg(N2kMsg);
  }
}  

int updateModuleInstance(int value) {
  if (!(value & 0x0100)) {
    EEPROM.write(INSTANCE_ADDRESS_EEPROM_ADDRESS, value);
    MODULE_INSTANCE = EEPROM.read(INSTANCE_ADDRESS_EEPROM_ADDRESS);
  }
  STATUS_LEDS.writeByte(MODULE_INSTANCE); delay(1000);
  return(0);
}

/**********************************************************************
 * OVERRIDE OF FUNCTIONS IN NOP100
 */

/**********************************************************************
 * getStatusLedsStatus - overrides the default function in the generic
 * firmware, returning a value that can be used to update the status
 * LEDs with the input channel states.
 */
#define GET_STATUS_LEDS_STATUS
uint8_t getStatusLedsStatus() {
  unsigned char retval = 0;
  for (int i = 0; i < 8; i++) {
    retval = (retval | (((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, (i + 1)) == N2kOnOff_On)?1:0) << i));
  }
  return(retval);
}

