/**
 * @file defines.h
 * @author Paul Reeve (preeve@pdjr.eu)
 * @brief CPP define directives for ROM106.
 * @copyright Copyright (c) 2023
 */

/**********************************************************************
 * @brief GPIO aliases. 
 */
#define GPIO_RELAY1_ENABLE GPIO_D23
#define GPIO_RELAY2_ENABLE GPIO_D22
#define GPIO_RELAY3_ENABLE GPIO_D21
#define GPIO_RELAY4_ENABLE GPIO_D20
#define GPIO_RELAY5_ENABLE GPIO_D6
#define GPIO_RELAY6_ENABLE GPIO_D5
#define GPIO_RELAY_SET GPIO_D8
#define GPIO_RELAY_RST GPIO_D9
#define GPIO_RELAY_CONTROL_OUTPUT_PINS { GPIO_RELAY1_ENABLE, GPIO_RELAY2_ENABLE, GPIO_RELAY3_ENABLE, GPIO_RELAY4_ENABLE, GPIO_RELAY5_ENABLE, GPIO_RELAY6_ENABLE, GPIO_RELAY_SET, GPIO_RELAY_RST }

/**
 * @brief Device information overrides for ROM106.
 */
#define DEVICE_CLASS 30                 // Electrical Distribution
#define DEVICE_FUNCTION 140             // Load Controller
#define DEVICE_UNIQUE_NUMBER 849        // Bump me?

/**
 * @brief Product information overrided for ROM106.
 */
#define PRODUCT_CODE 002
#define PRODUCT_FIRMWARE_VERSION "1.1.0 (Jun 2022)"
#define PRODUCT_LEN 1
#define PRODUCT_SERIAL_CODE "002-849"   // PRODUCT_CODE + DEVICE_UNIQUE_NUMBER
#define PRODUCT_TYPE "ROM106"           // The product name?
#define PRODUCT_VERSION "1.0 (Mar 2022)"

/**
 * @brief PGN overrides for ROM106.
 * */
void handlePGN127502(const tN2kMsg &n2kMsg);

#define NMEA_TRANSMIT_MESSAGE_PGNS { 127501L, 0 }
#define NMEA_PGN_HANDLERS  { { 127502L, handlePGN127502 }, { 0L, 0 } }

/**********************************************************************
 * Module configuration stuff.
 */
#define MODULE_CONFIGURATION_SIZE 3

#define MODULE_CONFIGURATION_INSTANCE_INDEX 1
#define MODULE_CONFIGURATION_PGN127501_TRANSMIT_PERIOD_INDEX 2
#define MODULE_CONFIGURATION_PGN127501_TRANSMIT_OFFSET_INDEX 3

#define MODULE_CONFIGURATION_INSTANCE_DEFAULT 0xff
#define MODULE_CONFIGURATION_PGN127501_TRANSMIT_PERIOD_DEFAULT 0x02
#define MODULE_CONFIGURATION_PGN127501_TRANSMIT_OFFSET_DEFAULT 0x00

#define MODULE_CONFIGURATION_DEFAULT { \
  MODULE_CONFIGURATION_CAN_SOURCE_DEFAULT, \
  MODULE_CONFIGURATION_INSTANCE_DEFAULT, \
  MODULE_CONFIGURATION_PGN127501_TRANSMIT_PERIOD_DEFAULT, \
  MODULE_CONFIGURATION_PGN127501_TRANSMIT_OFFSET_DEFAULT \
}

/**********************************************************************
 * Number of milliseconds between checks on switch input channel state.
 */
#define RELAY_PROCESS_INTERVAL 100       // Process relay outputs every n ms

/**********************************************************************
 * Maximum number of entries in the relay operation queue.
 */
#define RELAY_OPERATION_QUEUE_SIZE 10       // Max number of entries

/**********************************************************************
 * Frequency of relay queue processing.
 */
#define RELAY_OPERATION_QUEUE_INTERVAL 20UL

#define ON_N2K_OPEN
#define CONFIGURATION_VALIDATOR