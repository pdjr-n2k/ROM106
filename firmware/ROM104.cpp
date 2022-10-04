/**********************************************************************
 * ROM106.cpp - firmware for the ROM106 relay output module.
 * Copyright (c) 2021-22 Paul Reeve <preeve@pdjr.eu>
 *
 * Target platform: Teensy 3.2
 * 
 * ROM106 is a 6-channel relay module with integrated CAN connectivity
 * built around a Teensy 3.2 microcontroller.
 * 
 * This firmware implements an NMEA 2000 interface for ROM106.
 * 
 * In NMEA 2000 networks, relay output modules (and switch input
 * modules) are uniquely identified by 8-bit instance address which is
 * set by the network engineer when the module is installed. ROM106
 * includes a DIL switch which is used to configure the module's
 * instance address and this is read by firmware when the module boots.
 * 
 * Once started the firmware issues a PGN127501 Binary Status Report
 * every four seconds or immediately upon a relay state change. This
 * broadcast reports the current status of all the module's relays.
 * 
 * The firmware listens on the NMEA 2000 bus for PGN127502 Binary
 * Status Update messages addressed to its configured instance number.
 * Such messages are the only means of operating the module's output
 * relays.
 * 
 * The relays used in the ROM104 module are single-coil, bistable,
 * devices. The use of latching relays reduces power consumption
 * because the relay coil only needs to be energised for a short time
 * in order to effect a state change.
 * 
 * Use of a single coil relay requires the firmware to manage polarity
 * changes across the coil to effect set and reset operations. The
 * firmware operates H-bridge driver ICSs which perform the actual
 * relay coil operation.
 * 
 * The firmware further manages power use by queueing state change
 * requests received over NMEA so that only one relay coil will be
 * energised at a time.
 *
 * Local feedback on relay states and module operation generally is
 * presented by modulating the ROM104 indicator LEDs.
 */

#include <Arduino.h>
#include <ArduinoQueue.h>
#include <EEPROM.h>
#include <NMEA2000_CAN.h>
#include <N2kTypes.h>
#include <N2kMessages.h>
#include <DilSwitch.h>
#include <arraymacros.h>

/**********************************************************************
 * SERIAL DEBUG
 * 
 * The firmware can be built so that it writes copious trace and debug
 * data to the MCU's serial output by defining DEBUG_SERIAL. When the
 * Teensy reboots it switches its USB port to serial emulation and it
 * can take a few seconds for a connected host computer to recognise
 * the switch: - DEBUG_SERIAL_START_DELAY introduces a delay that can
 * be used to prevent loss of early debug output.
 */
#define DEBUG_SERIAL
#define DEBUG_SERIAL_START_DELAY 4000

/**********************************************************************
 * MCU EEPROM (PERSISTENT) STORAGE 
 * 
 * Module configuration is persisted to Teensy EEPROM storage.
 * 
 * SOURCE_ADDRESS_EEPROM_ADDRESS is the storage address for the
 * module's 1-byte N2K/CAN source address.
 */
#define SOURCE_ADDRESS_EEPROM_ADDRESS 0

/**********************************************************************
 * MCU PIN DEFINITIONS
 * 
 * GPIO pin definitions for the Teensy 3.2 MCU and some collections
 * that can be used as array initialisers
 */
#define GPIO_MPX_CLOCK 0
#define GPIO_MPX_LATCH 1
#define GPIO_MPX_DATA 2
#define GPIO_CAN_TX 3
#define GPIO_CAN_RX 4
#define GPIO_INSTANCE_BIT0 5
#define GPIO_INSTANCE_BIT1 6 
#define GPIO_INSTANCE_BIT2 7
#define GPIO_INSTANCE_BIT3 8
#define GPIO_INSTANCE_BIT4 9
#define GPIO_INSTANCE_BIT5 10
#define GPIO_INSTANCE_BIT6 11
#define GPIO_INSTANCE_BIT7 12
#define GPIO_POWER_LED 13
#define GPIO_PRG 14
#define GPIO_RELAY6_ENABLE 16
#define GPIO_RELAY5_ENABLE 17
#define GPIO_RELAY4_ENABLE 18
#define GPIO_RELAY3_ENABLE 19
#define GPIO_RELAY2_ENABLE 20
#define GPIO_RELAY1_ENABLE 21
#define GPIO_RELAY_SET 22
#define GPIO_RELAY_RST 23
#define GPIO_INSTANCE_PINS { GPIO_INSTANCE_BIT0, GPIO_INSTANCE_BIT1, GPIO_INSTANCE_BIT2, GPIO_INSTANCE_BIT3, GPIO_INSTANCE_BIT4, GPIO_INSTANCE_BIT5, GPIO_INSTANCE_BIT6, GPIO_INSTANCE_BIT7 }
#define GPIO_INPUT_PINS { GPIO_PRG, GPIO_INSTANCE_BIT0, GPIO_INSTANCE_BIT1, GPIO_INSTANCE_BIT2, GPIO_INSTANCE_BIT3, GPIO_INSTANCE_BIT4, GPIO_INSTANCE_BIT5, GPIO_INSTANCE_BIT6, GPIO_INSTANCE_BIT7 }
#define GPIO_OUTPUT_PINS { GPIO_POWER_LED, GPIO_MPX_CLOCK, GPIO_MPX_LATCH, GPIO_MPX_DATA, GPIO_RELAY1_ENABLE, GPIO_RELAY2_ENABLE, GPIO_RELAY3_ENABLE, GPIO_RELAY4_ENABLE, GPIO_RELAY5_ENABLE, GPIO_RELAY6_ENABLE, GPIO_RELAY_SET, GPIO_RELAY_RST }

/**********************************************************************
 * DEVICE INFORMATION
 * 
 * Because of NMEA's closed standard, most of this is fiction. Maybe it
 * can be made better with more research. In particular, even recent
 * releases of the NMEA function and class lists found using Google
 * don't discuss anchor systems, so the proper values for CLASS and
 * FUNCTION in this application are not known.  At the moment they are
 * set to 25 (network device) and 130 (PC gateway).
 * 
 * INDUSTRY_GROUP we can be confident about (4 says maritime). However,
 * MANUFACTURER_CODE is only allocated to subscribed NMEA members and,
 * unsurprisingly, an anonymous code has not been assigned: 2046 is
 * currently unused, so we adopt that.  
 * 
 * MANUFACTURER_CODE and UNIQUE_NUMBER together must make a unique
 * value on any N2K bus and an easy way to achieve this is just to
 * bump the unique number for every software build and this is done
 * automatically by the build system.
 */
#define DEVICE_CLASS 75
#define DEVICE_FUNCTION 130
#define DEVICE_INDUSTRY_GROUP 4
#define DEVICE_MANUFACTURER_CODE 2046
#define DEVICE_UNIQUE_NUMBER 849

/**********************************************************************
 * PRODUCT INFORMATION
 * 
 * This poorly structured set of values is what NMEA expects a product
 * description to be shoe-horned into.
 */
#define PRODUCT_CERTIFICATION_LEVEL 1
#define PRODUCT_CODE 002
#define PRODUCT_FIRMWARE_VERSION "1.1.0 (Jun 2022)"
#define PRODUCT_LEN 1
#define PRODUCT_N2K_VERSION 2101
#define PRODUCT_SERIAL_CODE "002-849" // PRODUCT_CODE + DEVICE_UNIQUE_NUMBER
#define PRODUCT_TYPE "ROM106"
#define PRODUCT_VERSION "1.0 (Mar 2022)"

/**********************************************************************
 * Include the build.h header file which can be used to override any or
 * all of the above  constant definitions.
 */
#include "build.h"

#define DEFAULT_SOURCE_ADDRESS 22           // Seed value for source address claim
#define INSTANCE_UNDEFINED 255              // Flag value
#define PGN127501_TRANSMIT_INTERVAL 4000UL  // Normal transmission rate
#define RELAY_OPERATION_QUEUE_SIZE 10       // Max number of entries
#define RELAY_OPERATION_QUEUE_INTERVAL 20UL // Frequency of relay queue processing
#define LED_UPDATE_INTERVAL 50              // Frquency of LED display update
#define LED_QUEUE_FULL_ERROR_PATTERN 63

/**********************************************************************
 * Declarations of local functions.
 */
void messageHandler(const tN2kMsg&);
void handlePGN127502(const tN2kMsg n2kMsg);
void transmitSwitchbankStatusMaybe(bool force = false);
void transmitPGN127501();
void updateLedDisplayMaybe();
void overrideLedDisplay(unsigned char state = 0);
void cancelLedDisplayOverride();
void processRelayOperationQueueMaybe();
void isr();

/**********************************************************************
 * PGNs of messages transmitted by this program.
 * 
 * PGN 127501 Binary Status Report.
 */
const unsigned long TransmitMessages[] PROGMEM={ 127501L, 0 };

/**********************************************************************
 * PGNs of messages handled by this program.
 * 
 * There are none.
 */
typedef struct { unsigned long PGN; void (*Handler)(const tN2kMsg &N2kMsg); } tNMEA2000Handler;
tNMEA2000Handler NMEA2000Handlers[]={ { 127502L, handlePGN127502 }, { 0L, 0 } };

/**********************************************************************
 * DIL_SWITCH switch decoder for module instance address.
 */
int INSTANCE_PINS[] = GPIO_INSTANCE_PINS;
DilSwitch DIL_SWITCH (INSTANCE_PINS, ELEMENTCOUNT(INSTANCE_PINS));

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
 * SWITCHBANK_INSTANCE - working storage for the switchbank module
 * instance number. The user selected value is recovered from hardware
 * and assigned during module initialisation,
 */
unsigned char SWITCHBANK_INSTANCE = INSTANCE_UNDEFINED;

/**********************************************************************
 * SWITCHBANK_STATUS - working storage for holding the current state
 * of the module.
 */
tN2kBinaryStatus SWITCHBANK_STATUS;

/**********************************************************************
 * FORCE_LED_UPDATE - flag which can be set to force an immediate LED
 * update.
 */
bool FORCE_LED_UPDATE = false;

/**********************************************************************
 * OVERRIDE_LED_UPDATE - flag which can be set to prevent regular LED
 * updates.
 */
bool OVERRIDE_LED_UPDATE = false;

/**********************************************************************
 * MAIN PROGRAM - setup()
 */
void setup() {
  #ifdef DEBUG_SERIAL
  Serial.begin(9600);
  delay(DEBUG_SERIAL_START_DELAY);
  #endif

  // Set the mode of all digital GPIO pins.
  int ipins[] = GPIO_INPUT_PINS;
  int opins[] = GPIO_OUTPUT_PINS;
  for (unsigned int i = 0 ; i < ELEMENTCOUNT(ipins); i++) pinMode(ipins[i], INPUT_PULLUP);
  for (unsigned int i = 0 ; i < ELEMENTCOUNT(opins); i++) pinMode(opins[i], OUTPUT);
  
  // We assume that a new host system has its EEPROM initialised to all
  // 0xFF. We test by reading a byte that in a configured system should
  // never be this value and if it indicates a scratch system then we
  // set EEPROM memory up in the fol0ing way.
  //
  // Address | Value                                    | Size in bytes
  // --------+------------------------------------------+--------------
  // 0x00    | N2K source address                       | 1
  //
  //EEPROM.write(SOURCE_ADDRESS_EEPROM_ADDRESS, 0xff);
  if (EEPROM.read(SOURCE_ADDRESS_EEPROM_ADDRESS) == 0xff) {
    EEPROM.write(SOURCE_ADDRESS_EEPROM_ADDRESS, DEFAULT_SOURCE_ADDRESS);
  }

  // Recover module instance number.
  DIL_SWITCH.sample();
  SWITCHBANK_INSTANCE = DIL_SWITCH.value();

  // Confirm LED operation and briefly display instance number.
  overrideLedDisplay(0xff); delay(100);
  overrideLedDisplay(SWITCHBANK_INSTANCE); delay(1000);
  cancelLedDisplayOverride();
  
  // Initialise module status
  N2kResetBinaryStatus(SWITCHBANK_STATUS);

  // Initialise and start N2K services.
  NMEA2000.SetProductInformation(PRODUCT_SERIAL_CODE, PRODUCT_CODE, PRODUCT_TYPE, PRODUCT_FIRMWARE_VERSION, PRODUCT_VERSION);
  NMEA2000.SetDeviceInformation(DEVICE_UNIQUE_NUMBER, DEVICE_FUNCTION, DEVICE_CLASS, DEVICE_MANUFACTURER_CODE);
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, EEPROM.read(SOURCE_ADDRESS_EEPROM_ADDRESS)); // Configure for sending and receiving.
  NMEA2000.EnableForward(false); // Disable all msg forwarding to USB (=Serial)
  NMEA2000.ExtendTransmitMessages(TransmitMessages); // Tell library which PGNs we transmit
  NMEA2000.SetMsgHandler(messageHandler);
  NMEA2000.Open();

  // Attach interrupt service routine to PRG button
  attachInterrupt(digitalPinToInterrupt(GPIO_PRG), isr, RISING);
}

/**********************************************************************
 * MAIN PROGRAM - loop()
 * 
 * Local functions called from loop() implement interval timers which
 * ensure that they will only perform their substantive tasks at
 * meaningful intervals (typically defined by program constants) rather
 * than every loop cycle.
 */ 
void loop() {
  #ifdef DEBUG_SERIAL
  Serial.println();
  Serial.println("Starting:");
  Serial.print("  N2K Source address is "); Serial.println(NMEA2000.GetN2kSource());
  Serial.print("  Module instance number is "); Serial.println(SWITCHBANK_INSTANCE);
  #endif

  // Before we transmit anything, let's do the NMEA housekeeping and
  // process any received messages. This call may result in acquisition
  // of a new CAN source address, so we check if there has been any
  // change and if so save the new address to EEPROM for future re-use.
  NMEA2000.ParseMessages();
  if (NMEA2000.ReadResetAddressChanged()) EEPROM.update(SOURCE_ADDRESS_EEPROM_ADDRESS, NMEA2000.GetN2kSource());

  // Once the start-up settle period is over we can enter production by
  // executing our only substantive function.
  if (SWITCHBANK_INSTANCE < 253) transmitSwitchbankStatusMaybe();

  // Process relay operation queue.
  processRelayOperationQueueMaybe();

  // Update LED display
  updateLedDisplayMaybe();
}

/**********************************************************************
 * MAIN PROGRAM - isr()
 * 
 * Invoked by a press of the PRG button. Updates SWITCHBANK_INSTANCE
 * from the DIL switch setting.
 */
void isr() {
  DIL_SWITCH.sample();
  SWITCHBANK_INSTANCE = DIL_SWITCH.value();
}

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
      operating = false;
    }

    if (!RELAY_OPERATION_QUEUE.isEmpty()) {
      opcode = RELAY_OPERATION_QUEUE.dequeue();
      if (opcode > 0) {
        digitalWrite(GPIO_RELAY_SET, 1); digitalWrite(GPIO_RELAY_RST, 0);
      } else {
        digitalWrite(GPIO_RELAY_SET, 0); digitalWrite(GPIO_RELAY_RST, 1);
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
 * This function is used to broadcast the switchbank status onto the
 * host NMEA bus.  The NMEA specification requires that such broadcasts
 * happen regularly every few seconds and immediately when there is a
 * switchbank state change. Consequently, the function has two use
 * patterns.
 * 
 * transmitSwitchbankStatusMaybe() is used to implement the regular
 * status broadcast. The function should be called from loop() and it
 * will operate once every PGN127501_TRANSMIT_INTERVAL milliseconds.
 * 
 * transmitSwitchbankStatusMaybe(true) should be called immediately a
 * switchbank channel is updated. The function will operate promptly and
 * flag the underlying state change by setting FORCE_LED_UPDATE so that
 * the LED display processes are advised that they need to change the
 * module's LED display.
 */
void transmitSwitchbankStatusMaybe(bool force) {
  static unsigned long deadline = 0UL;
  unsigned long now = millis();

  if ((now > deadline) || force) {
    #ifdef DEBUG_SERIAL
    Serial.print("Transmitting status:");
    Serial.print(" "); Serial.print((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, 1) == N2kOnOff_On)); 
    Serial.print(" "); Serial.print((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, 2) == N2kOnOff_On)); 
    Serial.print(" "); Serial.print((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, 3) == N2kOnOff_On)); 
    Serial.print(" "); Serial.print((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, 4) == N2kOnOff_On)); 
    Serial.print(" "); Serial.print((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, 5) == N2kOnOff_On)); 
    Serial.print(" "); Serial.print((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, 6) == N2kOnOff_On)); 
    #endif

    transmitPGN127501();
    FORCE_LED_UPDATE = force;

    deadline = (now + PGN127501_TRANSMIT_INTERVAL);
  }
}

/**********************************************************************
 * This function should be called from loop() to perform regular and
 * exceptional updates of the module's LED display. If
 * OVERRIDE_LED_UPDATE is true then no display updates will be
 * performed and the function will return directly to the caller,
 * 
 * LED updates are normally performed every LED_UPDATE_INTERVAL, but if
 * FORCE_LED_UPDATE is true then the update will happen immediately.
 *
 * FORCE_LED_UPDATE should be set true by the NMEA message transmission
 * process each time it transmits and will result in the  "transmit" LED
 * being switched on for a single execution cycle.
 */ 
void updateLedDisplayMaybe() {
  static unsigned long deadline = 0UL;
  unsigned long now = millis();
  unsigned char ledstate = 0;

  if (((now > deadline) || FORCE_LED_UPDATE) && (!OVERRIDE_LED_UPDATE)) {
    for (int c = 6; c >= 1; c--) ledstate = (ledstate << 1) | ((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, c) == N2kOnOff_On)?1:0);
    ledstate |= 64; // Power LED on
    ledstate |= (FORCE_LED_UPDATE)?128:0; // Transmit LED on

    digitalWrite(GPIO_MPX_LATCH, 0);
    shiftOut(GPIO_MPX_DATA, GPIO_MPX_CLOCK, LSBFIRST, ledstate);
    digitalWrite(GPIO_MPX_LATCH, 1);
 
    FORCE_LED_UPDATE = false;
    deadline = (now + LED_UPDATE_INTERVAL);
  }
} 

/**********************************************************************
 * Override (i.e. stop) the normal LED updates performed by
 * updateLedDisplayMaybe() and instead set the LED display to the value
 * specified by 'state'.
 */ 
void overrideLedDisplay(unsigned char state) {
  OVERRIDE_LED_UPDATE = true;
  
  digitalWrite(GPIO_MPX_LATCH, 0);
  shiftOut(GPIO_MPX_DATA, GPIO_MPX_CLOCK, LSBFIRST, state);
  digitalWrite(GPIO_MPX_LATCH, 1);
}

/**********************************************************************
 * Cancel the override (set by a prior call to overrideLedDisplay())
 * and resume normal update behaviour.
 */
void cancelLedDisplayOverride() {
  OVERRIDE_LED_UPDATE = false;
}

/**********************************************************************
 * Assemble and transmit a PGN 127501 Binary Status Update message
 * reflecting the current switchbank state.
 */
void transmitPGN127501() {
  static tN2kMsg N2kMsg;

  SetN2kPGN127501(N2kMsg, SWITCHBANK_INSTANCE, SWITCHBANK_STATUS);
  NMEA2000.SendMsg(N2kMsg);
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
void handlePGN127502(const tN2kMsg n2kMsg) {
  unsigned char instance;
  tN2kBinaryStatus bankStatus;
  tN2kOnOff channelStatus;
  
  if (ParseN2kPGN127501(n2kMsg, instance, bankStatus)) {
    if (instance == SWITCHBANK_INSTANCE) {
      for (unsigned int c = 1; c <= 6; c++) {
        channelStatus = N2kGetStatusOnBinaryStatus(bankStatus, c);
        if ((channelStatus == N2kOnOff_On) || (channelStatus == N2kOnOff_Off)) {        
          if (channelStatus != N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, c)) {
            if (!RELAY_OPERATION_QUEUE.isFull()) {
              cancelLedDisplayOverride();
              RELAY_OPERATION_QUEUE.enqueue((int) (c * ((channelStatus == N2kOnOff_On)?1:-1)));
            } else {
              overrideLedDisplay(LED_QUEUE_FULL_ERROR_PATTERN);
            }
          }
        }
      }
    }
  }
}

/**********************************************************************
 * Helper function called by the NMEA2000 library function
 * parseMessages() (which itself must be called from loop()) in order
 * to pass incoming messages to any user-defined handler. The mapping
 * between message PGN and handler function should be defined in the
 * global vector NMEA2000Handlers.
 */
void messageHandler(const tN2kMsg &N2kMsg) {
  int iHandler;
  for (iHandler=0; NMEA2000Handlers[iHandler].PGN!=0 && !(N2kMsg.PGN==NMEA2000Handlers[iHandler].PGN); iHandler++);
  if (NMEA2000Handlers[iHandler].PGN != 0) {
    NMEA2000Handlers[iHandler].Handler(N2kMsg); 
  }
}
