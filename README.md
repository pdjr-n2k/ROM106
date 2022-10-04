# ROM106 - NMEA 2000 relay output module

This project implements an NMEA 2000 relay output module with
support for up to six relay output channels.
You may want to check out
[SIM108](https://github.com/preeve9534/SIM108/),
a complementary project that implements an eight-channel NMEA
2000 switch input module.

__ROM106__ consists of a microcontroller-based hardware design
and an associated firmware.

The stock firmware realises an NMEA 2000 switch bank interface
that transmits
[PGN 127501 Binary Status Report]() messages
and responds to
[PGN 127502 Switch Bank Control]() messages.

The module is powered from the NMEA bus and has an LEN of 1.0.

## Design overview

__ROM106__ uses a Teensy 3.2 microcontroller supported by
power supply, CAN interface, configuration, display and
relay output sub-systems.

The power supply sub-system consists of a solid-state DC-DC
converter which adapts the voltage of the NMEA host NMEA bus to
the 5VDC required by the module's electronics.
The power supply is rated at 2W and its bus connection is fused
and reverse polarity protected.

The CAN interface sub-system manages all NMEA data bus I/O.
The data bus connection can be switched by the installer to
include a 120 Ohm bus termination resistor allowing the module
to be installed as either a bus termination node or a drop node.

The configuration sub-system consists of an 8-position DIL switch
and push-button which allow installer configuration of the module's
NMEA instance number.

The display sub-system provides a collection of LEDs which are used
to give configuration feedback and indicate the module operating
status.

The relay output sub-system consists of three H-bridge driver
ICs, each of which supports two output channels by providing a
polarity reversing drive for two bistable SPDT relays.
The use of bistable relays has the dual benefit of persisting relay
states in the event of bus failure and of minimising the bus power
consumed by relay operation.
Each relay presents zero-volt CO, NC and NO connections through
a pluggable terminal block and the relay and connections are rated
for 5A at up to 220VAC/30VDC.

__ROM104__'s stock firmware receives switchbank status instructions
over NMEA and queues any requested relay state change operations so
that concurrent relay switching loads cannot occur.

The firmware transmits(T) and receives(R) the following NMEA 2000
message types.

| PGN                           | Mode   | Description |
| :---                          | :----: | :---------- |
| 127501 (Binary Status Report) | T      | Issued every four seconds or immediately on the state change of any output channel. |
| 127502 (Switch Bank Control)  | R      | Used to set one or more relay channel states. |  

## Implementation

### Parts list

| REF      | Subsystem       | Component               | RS Part#|
| :---     | :---            | :---                    | :--- |
| --       | ENC             | [Plastic flanged enclosure](https://docs.rs-online.com/1460/0900766b814af994.pdf) | [919-0357](https://uk.rs-online.com/web/p/general-purpose-enclosures/9190357) |
| --       | PCB             | [PCB](./ROM104.brd.pdf) | |
| U7       | Microcontroller | [PJRC Teensy 3.2 MCU](https://www.pjrc.com/store/teensy32.html) |
| C8       | Microcontroller | [100nF elctrolytic capacitor](https://docs.rs-online.com/6ccf/0900766b8143e698.pdf)| [862-4146](https://uk.rs-online.com/web/p/aluminium-capacitors/8624146) |
| SW1      | Configuration   | [8-way SPST DIP switch](https://docs.rs-online.com/c98b/0900766b810b550f.pdf) | [756-1347](https://uk.rs-online.com/web/p/dip-sip-switches/7561347/) |
| SW2      | Configuration   | [2-way SPST DIP switch](https://docs.rs-online.com/a014/0900766b81670159.pdf) | [177-4261](https://uk.rs-online.com/web/p/dip-sip-switches/1774261) |
| SW3      | Configuration   | [Push button](https://docs.rs-online.com/9eaa/0900766b81403991.pdf) | [010-2327](https://uk.rs-online.com/web/p/keyboard-switches/0102327) |
| ??       | Configuration   | [MAX6816 debouncer](https://docs.rs-online.com/617e/0900766b81729403.pdf) | [189-9248](https://uk.rs-online.com/web/p/bounce-eliminator-ics/1899248) |
| U6       | Display         | [74HC595 shift register](https://uk.rs-online.com/web/p/counter-ics/7091971) | [709-1971](https://uk.rs-online.com/web/p/counter-ics/7091971) |
| C9       | Display         | [100nF elctrolytic capacitor](https://docs.rs-online.com/6ccf/0900766b8143e698.pdf)| [862-4146](https://uk.rs-online.com/web/p/aluminium-capacitors/8624146) |
| D1-D8    | Display         | [2mm rectangular LED](https://docs.rs-online.com/3547/0900766b81384f75.pdf) | [229-2447](https://uk.rs-online.com/web/p/leds/2292447) |
| RN1      | Display         | [470R 8x resistor array](https://docs.rs-online.com/d532/0900766b8069ccfd.pdf) | [522-4273](https://uk.rs-online.com/web/p/resistor-arrays/5224273) |
| U3       | Power supply    | [TracoPower TMR-2411 DC-DC converter](https://docs.rs-online.com/1b79/0900766b8172f5cb.pdf) | [433-8258](https://uk.rs-online.com/web/p/dc-dc-converters/4338258) |
| F1       | Power supply    | [1A resettable fuse](https://docs.rs-online.com/ec39/0900766b80bc9043.pdf) | [657-1772](https://uk.rs-online.com/web/p/resettable-fuses/6571772) |
| U2       | CAN interface   | [MCP2551-I/P CAN transceiver](https://docs.rs-online.com/f763/0900766b8140ba57.pdf) | [876-7259](https://uk.rs-online.com/web/p/can-interface-ics/8767259) | 
| C1       | CAN interface | [100nF elctrolytic capacitor](https://docs.rs-online.com/6ccf/0900766b8143e698.pdf)| [862-4146](https://uk.rs-online.com/web/p/aluminium-capacitors/8624146) |
| R1       | CAN interface   | [4K7 0.25W resistor](https://docs.rs-online.com/d566/A700000008919924.pdf) | [707-7260](https://uk.rs-online.com/web/p/through-hole-resistors/7077726) |
| R2       | CAN interface   | [120R 0.5W resistor](https://docs.rs-online.com/1e48/0900766b8157ae0f.pdf) | [707-8154](https://uk.rs-online.com/web/p/through-hole-resistors/7078154) |
| J3       | CAN interface   | [Terminal block 1x5 2.54"](https://docs.rs-online.com/85fb/0900766b816edda7.pdf) | [220-4298](https://uk.rs-online.com/web/p/pcb-terminal-blocks/2204298) |
| J3*      | CAN interface   | [M12 5-pin male connector ](https://docs.rs-online.com/6e45/A700000007926144.pdf) | [877-1154](https://uk.rs-online.com/web/p/industrial-circular-connectors/8771154) |
| U1,U4,U5 | Relay output | [L293D quadruple half-H driver](https://docs.rs-online.com/90a7/0900766b8135fae0.pdf) | [714-0622](https://uk.rs-online.com/web/p/motor-driver-ics/7140622) |
| K1-K6    | Relay output | [TE Connectivity 5A latching relay](https://docs.rs-online.com/39e5/0900766b81397a52.pdf) | [616-8584](https://uk.rs-online.com/web/p/power-relays/6168584) |
| J1,J2,J4 | Relay output | [Terminal block header](https://docs.rs-online.com/0a3e/0900766b8157d660.pdf) | [897-1272](https://uk.rs-online.com/web/p/pcb-headers/8971272) |
| C2-C7    | Relay output | [100nF elctrolytic capacitor](https://docs.rs-online.com/6ccf/0900766b8143e698.pdf)| [862-4146](https://uk.rs-online.com/web/p/aluminium-capacitors/8624146) |

### Assembly

Components must be placed and soldered with care taken to ensure
correct orientation and polarity.

The host NMEA bus can be wired directly to J3 or (and preferably)
the ENCLOSURE drilled to accommodate J3* and J3*'s flying leads
connected to the J3 header.

D1 through D8 can be soldered with long leads and holes drilled in
ENCLOSURE to expose the LED lenses or (and preferably), each LED can
be mounted normally on the PCB and elaborated with a light pipe
mounted to the enclosure.

## Use

### Configuration

It will almost always be simpler to configure the module on the bench
and then install it in its normal operating location.

1. Begin configuration by exposing the module PCB.

2. Configure bus termination.
   Locate the BUS dip switch on the PCB.
   Set BUS[T] to ON(1) if the module will be connected as a terminating node
   at the end of its host NMEA bus backbone; or
   set BUS[T] to OFF(0) if the module will be connected to its host NMEA bus
   via a T-connector and drop cable.

3. Configure bus ground.
   Locate the BUS dip switch on the PCB.
   Set BUS[G] to ON(1) to connect the NMEA bus shield to the module GND.
   Set BUS[G] to OFF(0) to isolate the NMEA bus shield from the module GND.
   Usually it is appropriate to set BUS[G] to OFF(0).

4. Configure switchbank instance address.
   Locate the INSTANCE switch and PRG buttons on the PCB.
   Set INSTANCE[0..7] to a binary representation of your chosen, unique,
   instance address in the range 0 through 252.
   The module reads the INSTANCE value on boot; if you need to make a
   change whilst the module is operating, then set the DIL switch to the
   required value and press the PRG button to immediately activate the new
   instance number.
   
### Relay connections

1. Each relay output channel supports CO (COmmon), NO (Normally Open) and NC
   (Normally Closed) connections.

2. Connect a terminal plug to each external circuits you wish to operate
   and plug the terminal into the appropriate module relay output header.
   Make sure to fuse the switched load at or below the 5A relay limit.
   If the switched load is inductive, then use of a flyback diode may be
   a prudent way of protecting the relay output.

### Operation

The module will enter operation immediately upon connection to the NMEA bus.

Immediately power is applied to the module:

1. All eight display LEDs will briefly illuminate to confirm that they are
   working.
2. The LED display will indicate the configured instance number in binary.

After a few seconds the display LED's will be used in the following way.

| LED   | State | Meaning |
| :---: | :---: | :--- |
| 1     | ON    | The module has started (and by implication is receiving power from the NMEA bus). |
| 2     | FLASH | The module is transmitting a status report. |
| 3     | ON    | Output channel 1 is ON |
| 4     | ON    | Output channel 2 is ON |
| 5     | ON    | Output channel 3 is ON |
| 6     | ON    | Output channel 4 is ON |
| 7     | ON    | Output channel 5 is ON |
| 8     | ON    | Output channel 6 is ON |

If the firmware detects an error the LEDs wil indicate an error
code.

| Code     | Meaning |
| :---     | :---    |
| &#9899;&#9899;&#9898;&#9898;&#9898;&#9898;&#9898;&#9898; | Relay operating queue overflow. |
