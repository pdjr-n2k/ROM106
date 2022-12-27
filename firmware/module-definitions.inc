/**********************************************************************
 * module-declarations.inc
 *
 * Contains declarations of types, variables, etc. that are required
 * for implementation of our application. Note that application
 * functions must be declared at the end of module-definitions.inc.
 */


/**********************************************************************
 * SWITCHBANK_STATUS - working storage for holding the most recently
 * read state of the Teensy switch inputs in the format used by the
 * NMEA2000 library.
 */
tN2kBinaryStatus SWITCHBANK_STATUS;

unsigned long PGN127501_TRANSMIT_INTERVAL = PGN127501_DEFAULT_TRANSMIT_INTERVAL;

/**********************************************************************
 * RELAY_OPERATION_QUEUE is a queue of integer opcodes each of which
 * specifies a relay (1 through 4) and an operation: SET if the opcode
 * is positive; reset if the opcode is negative. Relay operations are
 * queued for sequential processing in order to smooth out the uneven
 * and possibly unsupportable power demands that could result from
 * parallel or overlapping operation of multiple relays.
 */
ArduinoQueue<int> RELAY_OPERATION_QUEUE(RELAY_OPERATION_QUEUE_SIZE);

  
/**********************************************************************
 * processRelayOperationQueueMaybe() should be called directly from
 * loop.
 * 
 * The function will execute each RELAY_OPERATION_QUEUE_INTERVAL and
 * it is important that this constant is set to a value equal to or
 * greater than the minimum operating signal hold period of the
 * physical relays installed on the host PCB. For most latching
 * relays this will be around 20ms.
 * 
 * The function begins by switching off all H-bridge outputs, ensuring
 * that any output that may have been switched on in the previous
 * operating cycle is terminated.
 * 
 * The RELAY_OPERATION_QUEUE is checked and any head entry opcode is
 * removed and processed. A retrieved opcode will be a signed integer
 * whose absolute value specifies an output channel and whose sign
 * indicates whether the channel should be turned on (set) or turned
 * off (reset).
 * 
 * The function sets up H-bridge SET and RST GPIOs appropriately and
 * then switches on the selected channel's ENABLE GPIO, thus energising
 * the associated relay coil. A call is made to request transmission of
 * an NMEA message signalling the state change.
 */
void processRelayOperationQueueMaybe() {
  static unsigned long deadline = 0UL;
  unsigned long now = millis();
  static bool operating = true;
  int opcode;

  if (now > deadline) {

    if (operating) {
      digitalWrite(GPIO_RELAY1_ENABLE, 0);
      digitalWrite(GPIO_RELAY2_ENABLE, 0);
      digitalWrite(GPIO_RELAY3_ENABLE, 0);
      digitalWrite(GPIO_RELAY4_ENABLE, 0);
      digitalWrite(GPIO_RELAY5_ENABLE, 0);
      digitalWrite(GPIO_RELAY6_ENABLE, 0);
    }

    if (!RELAY_OPERATION_QUEUE.isEmpty()) {
      opcode = RELAY_OPERATION_QUEUE.dequeue();
      if (opcode > 0) {
        digitalWrite(GPIO_RELAY_SET, 1);
        digitalWrite(GPIO_RELAY_RST, 0);
      } else {
        digitalWrite(GPIO_RELAY_SET, 0);
        digitalWrite(GPIO_RELAY_RST, 1);
      }
      switch (opcode) {
        case 1: case -1:
          digitalWrite(GPIO_RELAY1_ENABLE, 1);
          N2kSetStatusBinaryOnStatus(SWITCHBANK_STATUS, (opcode == 1)?N2kOnOff_On:N2kOnOff_Off, 1);
          break;
        case 2: case -2:
          digitalWrite(GPIO_RELAY2_ENABLE, 1);
          N2kSetStatusBinaryOnStatus(SWITCHBANK_STATUS, (opcode == 1)?N2kOnOff_On:N2kOnOff_Off, 2);
          break;
        case 3: case -3:
          digitalWrite(GPIO_RELAY3_ENABLE, 1);
          N2kSetStatusBinaryOnStatus(SWITCHBANK_STATUS, (opcode == 1)?N2kOnOff_On:N2kOnOff_Off, 3);
          break;
        case 4: case -4:
          digitalWrite(GPIO_RELAY4_ENABLE, 1);
          N2kSetStatusBinaryOnStatus(SWITCHBANK_STATUS, (opcode == 1)?N2kOnOff_On:N2kOnOff_Off, 4);
          break;
        case 5: case -5:
          digitalWrite(GPIO_RELAY5_ENABLE, 1);
          N2kSetStatusBinaryOnStatus(SWITCHBANK_STATUS, (opcode == 1)?N2kOnOff_On:N2kOnOff_Off, 5);
          break;
        case 6: case -6:
          digitalWrite(GPIO_RELAY6_ENABLE, 1);
          N2kSetStatusBinaryOnStatus(SWITCHBANK_STATUS, (opcode == 1)?N2kOnOff_On:N2kOnOff_Off, 6);
          break;
        default: break;
      }
      operating = true;
      transmitSwitchbankStatusMaybe(true);
    }
    
    deadline = (now + RELAY_OPERATION_QUEUE_INTERVAL);
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
    TRANSMIT_LED_STATE = 1;
    deadline = (now + PGN127501_TRANSMIT_INTERVAL);
  }
}

/**********************************************************************
 * Process a received PGN 127502 Switch Bank Control message by
 * decoding the switchbank status message and comparing the requested
 * channel state(s) with the current SWITCHBANK_STATUS. Any mismatch 
 * results in one or more opcodes representing an appropriate set or
 * reset operation on each changed channel being queued for subsequent
 * processing.
 * 
 * If the relay operation queue is full, the LED display is set to
 * LED_QUEUE_FULL_ERROR_PATTERN and locked until the queue again
 * becomes usable.
 */
void handlePGN127502(const tN2kMsg &n2kMsg) {
  unsigned char instance;
  tN2kBinaryStatus bankStatus;
  tN2kOnOff channelStatus;
  
  if (ParseN2kPGN127501(n2kMsg, instance, bankStatus)) {
    if (instance == MODULE_INSTANCE) {
      for (unsigned int c = 1; c <= 6; c++) {
        channelStatus = N2kGetStatusOnBinaryStatus(bankStatus, c);
        if ((channelStatus == N2kOnOff_On) || (channelStatus == N2kOnOff_Off)) {        
          if (channelStatus != N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, c)) {
            if (!RELAY_OPERATION_QUEUE.isFull()) {
              //LED_DISPLAY.cancelOverride();
              RELAY_OPERATION_QUEUE.enqueue((int) (c * ((channelStatus == N2kOnOff_On)?1:-1)));
            } else {
              //LED_DISPLAY.override(LED_QUEUE_FULL_ERROR_PATTERN);
            }
          }
        }
      }
    }
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
  for (int i = 0; i < 6; i++) {
    retval = (retval | (((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, (i + 1)) == N2kOnOff_On)?1:0) << i));
  }
  return(retval);
}

