# ROM104 - NMEA 2000 relay output module

This project implements an NMEA 2000 relay output module with
support for four relay output channels.
A complementary project,
[SIM108](https://github.com/preeve9534/SIM108/),
implements an eight-channel NMEA 2000 switch input module.

The project consists of a microcontroller-based hardware design
and associated firmware.
The stock firmware realises an NMEA 2000 switchbank interface
that transmits
[PGN 127501 Binary Status Report]() messages
and responds to
[PGN 127502 Switch Bank Control]() messages.

The module is powered from the NMEA bus and has an LEN of 1.0.

## Design

### Hardware

Each output channel drives a bistable SPDT relay: this has the
dual benefit of preserving relay states in the event of bus
failure and of minimising the power consumed by relay operation.

Each relay presents zero-volt CO, NC and NO connections through
a PCB mounted terminal block rated for switching 5A at 220VAC or
30VDC.

A bank of four LEDs indicates the state of each output channel.

The module's CAN/NMEA bus connection is designed to support an
NMEA 2000 compatible M12 5-pin male circular connector, but
other connector types can be substituted.
A DIL switch allows a 120 Ohm resistor to be connected across
the host data bus permitting the module to be installed as
either a drop node or a bus termination node.

The module's switchbank instance number is configured using an
8-position DIL switch.

### Firmware

Relay operations are queued so that concurrent relay switching
loads cannot occur.

__ROM104__'s stock firmware transmits(T) and receives(R) the
following NMEA 2000 message types.

| PGN                           | Mode   | Description |
| :---                          | :----: | :---------- |
| 127501 (Binary Status Report) | T      | Issued every four seconds or immediately on the state change of any output channel. |
| 127502 (Switch Bank Control)  | R      | Used to set relay channel state. |  

## Implementation

| REF   | Subsystem       | Component               | Part |
| :---  | :---            | :---                    | :--- |
| --    | ENC             | [Plastic flanged enclosure](https://docs.rs-online.com/1460/0900766b814af994.pdf) | [919-0357](https://uk.rs-online.com/web/p/general-purpose-enclosures/9190357) |
| --    | PCB             | [PCB](./ROM104.brd.pdf) | |
| U5    | Microcontroller | [PJRC Teensy 3.2 MCU](https://www.pjrc.com/store/teensy32.html) |
| C3    | Microcontroller | [100nF ceramic capacitor](https://docs.rs-online.com/554d/0900766b817069f5.pdf)| [538-1427](https://uk.rs-online.com/web/p/mlccs-multilayer-ceramic-capacitors/5381427) |
| SW1   | Microcontroller | [8-way SPST DIP switch](https://docs.rs-online.com/c98b/0900766b810b550f.pdf) | [756-1347](https://uk.rs-online.com/web/p/dip-sip-switches/7561347/) |
| U4    | Power supply    | [TracoPower TMR-2411 DC-DC converter](https://docs.rs-online.com/1b79/0900766b8172f5cb.pdf) | [433-8258](https://uk.rs-online.com/web/p/dc-dc-converters/4338258) |
| F1    | Power supply    | [1A resettable fuse](https://docs.rs-online.com/ec39/0900766b80bc9043.pdf) | [657-1772](https://uk.rs-online.com/web/p/resettable-fuses/6571772) |
| C2    | Power supply    | [1000uF aluminium capacitor](https://docs.rs-online.com/0d4a/0900766b815816c4.pdf) | [711-1149](https://uk.rs-online.com/web/p/aluminium-capacitors/7111148) |
| U3    | CAN interface   | [MCP2551-I/P CAN transceiver](https://docs.rs-online.com/f763/0900766b8140ba57.pdf) | [876-7259](https://uk.rs-online.com/web/p/can-interface-ics/8767259) | 
| C1    | CAN interface   | [100nF ceramic capacitor](https://docs.rs-online.com/554d/0900766b817069f5.pdf)| [538-1427](https://uk.rs-online.com/web/p/mlccs-multilayer-ceramic-capacitors/5381427) |
| R5    | CAN interface   | [4K7 0.25W resistor](https://docs.rs-online.com/d566/A700000008919924.pdf) | [707-7260](https://uk.rs-online.com/web/p/through-hole-resistors/7077726) |
| SW2   | CAN interface   | [2-way SPST DIP switch](https://docs.rs-online.com/a014/0900766b81670159.pdf) | [177-4261](https://uk.rs-online.com/web/p/dip-sip-switches/1774261) |
| R6    | CAN interface   | [120R 0.5W resistor](https://docs.rs-online.com/1e48/0900766b8157ae0f.pdf) | [707-8154](https://uk.rs-online.com/web/p/through-hole-resistors/7078154) |
| J2    | CAN interface   | [Terminal block 1x5 2.54"](https://docs.rs-online.com/85fb/0900766b816edda7.pdf) | [220-4298](https://uk.rs-online.com/web/p/pcb-terminal-blocks/2204298) |
| J2*   | CAN interface   | [M12 5-pin male connector ](https://docs.rs-online.com/6e45/A700000007926144.pdf) | [877-1154](https://uk.rs-online.com/web/p/industrial-circular-connectors/8771154) |
| U1,U2 | Relay output    | [L293D quadruple half-H driver](https://docs.rs-online.com/90a7/0900766b8135fae0.pdf) | [714-0622](https://uk.rs-online.com/web/p/motor-driver-ics/7140622) |
| K1,K2,K3,K4 | Relay output | [TE Connectivity 5A latching relay](https://docs.rs-online.com/39e5/0900766b81397a52.pdf) | [616-8584](https://uk.rs-online.com/web/p/power-relays/6168584) |
| J1 | Relay output | [Wurth 401B terminal block](https://docs.rs-online.com/238a/0900766b8173e753.pdf) | [191-7518](https://uk.rs-online.com/web/p/pcb-terminal-blocks/1917518) |
| D1,D2,D3,D4 | User interface | [2mm rectangular LED](https://docs.rs-online.com/3547/0900766b81384f75.pdf) | [229-2447](https://uk.rs-online.com/web/p/leds/2292447) |
| R1,R2,R3,R4 | User interface | [470R 0.25W resistor](https://docs.rs-online.com/d566/A700000008919924.pdf) | [707-7726](https://uk.rs-online.com/web/p/through-hole-resistors/7077726/) |

## Assembly

Components must be placed and soldered with care taken to ensure
correct orientation and polarity.

The host NMEA bus can be wired directly to J2 or (and preferably)
J2 can be omitted, the ENCLOSURE drilled to accommodate J3 and
J3's flying leads soldered directly to the pads intended for J1.

D9 through D17 can be soldered with long leads and holes drilled in
ENCLOSURE to expose the LED or (and preferably), they can each be
mounted with CLIP to ENCLOSURE and trailing leads used to connect
back to the PCB mounting location.
The latter approach means exact positioning of the holes which
expose the PCB mounted LEDs is not required.

## Configuration

1. It will almost always be simpler to configure the module on the bench
   and then install it in its normal operating location.

2. Begin configuration by exposing the module PCB.

3. Configure bus termination.
   Set SW2[T] to ON(1) if the module will be connected as a terminating node
   at the end of its host NMEA bus backbone; or
   set SW2[T] to OFF(0) if the module will be connected to its host NMEA bus
   via a T-connector and drop cable.

4. Configure bus ground.
   Set SW2[G] to ON(1) to connect the NMEA bus shield to the module GND.
   Set SW2[G] to OFF(0) to isolate the NMEA bus shield from the module GND.
   Usually it is appropriate to set SW2[G] to OFF(0).

5. Configure switchbank instance address.
   Set SW1[1..8] to a binary representation of your chosen, unique, instance
   address in the range 0 through 252.
   Setting an address outside this range will disable the module.
   SW1[1] sets address bit 0; SW1[8] sets address bit 7.
   
## Installation

1. Each relay output channel supports CO (COmmon), NO (Normally Open) and NC
   (Normally Closed) connections. a reference switch input ground to J2[9].
   CO is connected to NO and disconnected from NC when the associated switchbank
   channel is ON and is connected to NC and disconnected from NO when the
   channel is OFF.

2. Connect up to a maximum of eight switch inputs to J2[1..8].
   Each input must supply an ON voltage in the range 5VDC through 50VDC
   relative to J2[9].
   Each input must be able to source around 10mA necessary to drive the
   opto-isolation input circuitry.
