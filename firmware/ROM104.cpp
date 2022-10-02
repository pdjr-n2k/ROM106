/**********************************************************************
 * ROM104.cpp - firmware for the ROM104 relay output module.
 * Copyright (c) 2021-22 Paul Reeve <preeve@pdjr.eu>
 *
 * Target platform: Teensy 3.2
 * 
 * ROM104 is a 4-channel relay module with integrated CAN connectivity
 * built around a Teensy 3.2 microcontroller.
 * 
 * This firmware implements an NMEA 2000 interface for ROM104.
 * 
 * In NMEA 2000 networks, relay output modules (and switch input
 * modules) are identified by an 8-bit instance address which is set by
 * the network engineer when the module is installed. ROM104 includes a
 * DIL switch which is used to configure the module's instance address
 * and this is read by firmware when the module boots.
 * 
 * Once started the firmware issues a PGN127501 Binary Status Report
 * every four seconds or immediately upon a relay state change. This
 * broadcast reports the current status of the module's relays.
 * 
 * The firmware listens on the NMEA 2000 bus for PGN127502 Binary
 * Status Update messages addressed to its configured instance number.
 * These messages operate the ROM104 relay outputs whilst attempting to
 * minimise the module's overall power consumption. The relays used
 * in the ROM104 module are single-coil, bistable, latching devices.
 * 
 * The use of latching relays reduces power consumption because the
 * relay coil only needs to be energised for a short time in order to
 * effect a state change. The firmware further manages power use by
 * queueing state change requests so that only one relay coil will be
 * energised at a time.
 * 
 * Use of a single coil relay demands polarity changes across the coil
 * to effect set and reset operations. ROM104 includes twin H-bridge
 * drivers that the firmware manipulates to operate the relay outputs.
 * 
 * Local feedback on relay states is presented by modulating the ROM104
 * indicator LEDs.
 */

#include <Arduino.h>
#include <ArduinoQueue.h>
#include <EEPROM.h>
#include <NMEA2000_CAN.h>
#include <N2kTypes.h>
#include <N2kMessages.h>
#include <DilSwitch.h>
#include <arraymacros.h>
#include <Scheduler.h>
#include <Debouncer.h>

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
#define GPIO_MPX_CLOCK 0                   GPIO_CH0_LED 0
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
#define SCHEDULER_TICK 20UL                   // The frequency of scheduler management
#define PGN127501_TRANSMIT_INTERVAL 4000UL  // Normal transmission rate
#define RELAY_OPERATION_QUEUE_SIZE 10       // Max number of entries
#define RELAY_OPERATION_QUEUE_INTERVAL 20UL // Frequency of relay queue processing

/**********************************************************************
 * Declarations of local functions.
 */
void messageHandler(const tN2kMsg&);
void handlePGN127502(const tN2kMsg n2kMsg);
void transmitSwitchbankStatusMaybe(bool force = false);
void transmitPGN127501();
void updateLeds(bool transmit);
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
 * SCHEDULER callback scheduler.
 */
Scheduler SCHEDULER (SCHEDULER_TICK);

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

  // Process deferred callbacks.
  SCHEDULER.loop();
}

/**********************************************************************
 * MAIN PROGRAM - isr()
 */
void isr() {
  DIL_SWITCH.sample();
  SWITCHBANK_INSTANCE = DIL_SWITCH.value();
}

/**********************************************************************
 * processRelayOperationQueueMaybe() should be called directly from
 * loop. The function is triggered each RELAY_OPERATION_QUEUE_INTERVAL
 * to terminate any previously triggered relay operating signal and to
 * process any queued request for relay operation.  It is important that
 * the function's operating interval is at least as long as the minimum
 * operating signal hold period of the physical relays installed on the
 * host PCB.
 * 
 * The function operates an H-bridge interface, setting output polarity
 * and then issuing an enable signal on the selected relay. 
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
        case 1: case -1: digitalWrite(GPIO_RELAY1_ENABLE, 1); break;
        case 2: case -2: digitalWrite(GPIO_RELAY2_ENABLE, 1); break;
        case 3: case -3: digitalWrite(GPIO_RELAY3_ENABLE, 1); break;
        case 4: case -4: digitalWrite(GPIO_RELAY4_ENABLE, 1); break;
        case 5: case -5: digitalWrite(GPIO_RELAY5_ENABLE, 1); break;
        case 6: case -6: digitalWrite(GPIO_RELAY6_ENABLE, 1); break;
        default: break;
      }
      operating = true;
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
    #ifdef DEBUG_SERIAL
    Serial.print("Transmitting status:");
    (N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, 1) == N2kOnOff_On)
    Serial.print(" "); Serial.print((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, 1) == N2kOnOff_On)); 
    Serial.print(" "); Serial.print((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, 2) == N2kOnOff_On)); 
    Serial.print(" "); Serial.print((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, 3) == N2kOnOff_On)); 
    Serial.print(" "); Serial.print((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, 4) == N2kOnOff_On)); 
    Serial.print(" "); Serial.print((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, 5) == N2kOnOff_On)); 
    Serial.print(" "); Serial.print((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, 6) == N2kOnOff_On)); 
    #endif

    transmitPGN127501();
    
    updateLeds(true);
    SCHEDULER.schedule([](){ updateLeds(false); }, 50);

    deadline = (now + PGN127501_TRANSMIT_INTERVAL);
  }
}

/**********************************************************************
 * Update the relay state indicator LEDs so that they reflect the value
 * of <status>.
 */ 
void updateLeds(bool transmit) {
  unsignded char out = 0;
  for (int c = 1; c <= 6; c++) out = out + (((N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, c) == N2kOnOff_On)?1:0) * (2 ^ (c - 1)));
  out += (64 + (transmit:128:0));
  digitalWrite(GPIO_MPX_LATCH, 0);
  shiftOut(GPIO_MPX_DATA, GPIO_MPX_CLOCK, LSBFIRST, out);
  digitalWrite(GPIO_MPX_LATCH, 1);
}

/**********************************************************************
 * Assemble and transmit a PGN 127501 Binary Status Update message over
 * the host NMEA bus. <instance> specifies the switchbank instance
 * number and <status> the switchbank channel states. 
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
 * results in an opcode representing an appropriate set or reset
 * operation being added to the RELAY_OPERATION_QUEUE for subsequent
 * operation of the nonconformant relay(s).
 */
void handlePGN127502(const tN2kMsg n2kMsg) {
  unsigned char instance;
  tN2kBinaryStatus bankStatus;
  tN2kOnOff channelStatus;
  bool changed = false;
  
  if (ParseN2kPGN127501(n2kMsg, instance, bankStatus)) {
    if (instance == SWITCHBANK_INSTANCE) {
      for (unsigned int c = 1; c <= 6; c++) {
        channelStatus = N2kGetStatusOnBinaryStatus(bankStatus, c);
        if ((channelStatus == N2kOnOff_On) || (channelStatus == N2kOnOff_Off)) {        
          if (channelStatus != N2kGetStatusOnBinaryStatus(SWITCHBANK_STATUS, c)) {
            RELAY_OPERATION_QUEUE.enqueue((int) (c * ((channelStatus == N2kOnOff_On)?1:-1)));
            changed = true;
          }
        }
      }
      if (changed) {
        transmitSwitchbankStatusMaybe(true);
        updateLeds(true);
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
