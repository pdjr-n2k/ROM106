# ROM104 - NMEA 2000 relay output module

This project implements an NMEA 2000 relay output module with
support for four relay output channels.
A complementary project,
[SIM108](https://github.com/preeve9534/SIM108/),
implements an eight-channel NMEA 2000 switch input module.

The project consists of a microcontroller-based hardware design
and associated firmware.
The stock firmware realizes an NMEA 2000 switchbank interface
that transmits
[PGN 127501 Binary Switch Status]() messages
and responds to
[PGN 127502 Binary Switch Control]() messages.

The module is powered from the NMEA bus and has an LEN of 1.0.

## Hardware design

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

The design exploits the following active components.

| Component | Function |
| :--- | :--- |
| [Teensy 3.2](https://www.pjrc.com/store/teensy32.html) | Microcontroller. |
| [TMR-1-1211]() | 12VDC to 5VDC 1A power supply (DC-DC converter). |
| [MCP2551-I/P](http://ww1.microchip.com/downloads/en/devicedoc/20001667g.pdf) | CAN transceiver. |
| [L2983]() |  Quadruple Half-H Driver for relay coil polarity reversal.|

## Firmware

Relay operations are queued so that concurrent relay switching
loads cannot occur.

__ROM104__'s stock firmware transmits(T) and receives(R) the
following NMEA 2000 message types.

| PGN | Mode   | Description |
| --- | :----: | ----------- |
| 127501 (Binary Switch Status)  | T | Issued every four seconds or immediately on the state change of any output channel. |
| 127502 (Binary Switch Control) | R | Used to set relay channel state.  

## PCB

The
[module PCB](./ROM104.brd.pdf)
is a 75mm x 75mm square. 

### Electronic components

| Component   | Description                                     | Further information
|------------ |------------------------------------------------ |--------------------- |
| C1          | 1000uF aluminium capacitor                      | [711-1148](https://uk.rs-online.com/web/p/aluminium-capacitors/7111148)
| C2,C3       | 100nF ceramic capacitor]                        | [538-1427](https://uk.rs-online.com/web/p/mlccs-multilayer-ceramic-capacitors/5381427)
| D1,D2,D3,D4 | 2V 1.8mm rectangular LED                        | [229-2425](https://uk.rs-online.com/web/p/leds/2292425)
| D5          | 2V 3.0mm circular LED                           | [228-5916](https://uk.rs-online.com/web/p/leds/2285916)
| F1          | ECE BU135 1.35A polymer fuse                    | [ECE](https://www.ece.com.tw/images/cgcustom/file020170930043926.pdf)
| J1,J2       | Phoenix Contact FK-MPT terminal block 1x8 3.5mm | [229-2425](https://uk.rs-online.com/web/p/pcb-terminal-blocks/8020169)
| J3          | Phoenix Contact MPT terminal block 1x5 2.54"    | [220-4298](https://uk.rs-online.com/web/p/pcb-terminal-blocks/2204298)
| R1,R10-R13  | 390R 0.25W resistor                             | [707-7634](https://uk.rs-online.com/web/p/through-hole-resistors/7077634)
| R3-R9       | 2K2 0.25W resistor                              | [707-7690](https://uk.rs-online.com/web/p/through-hole-resistors/7077690)
| R14         | 120R 0.25W resistor                             | [707-7599](https://uk.rs-online.com/web/p/through-hole-resistors/7077599)
| SW1         | 6mm momentary push button                       | Sourced from eBay
| SW2         | 2-way SPST DIP switch                           | [177-4261](https://uk.rs-online.com/web/p/dip-sip-switches/1774261)
| SW3         | 8-way SPST DIP switch                           | [756-1347](https://uk.rs-online.com/web/p/dip-sip-switches/7561347)
| U1          | PJRC Teensy 3.2 MCU                             | [PJRC](https://www.pjrc.com/store/teensy32.html)
| U2          | TracoPower TMR-1-1211 DC-DC converter           | [781-3190](https://uk.rs-online.com/web/p/dc-dc-converters/7813190)
| U3          | MCP2551-I/P CAN transceiver                     | [040-2920](https://uk.rs-online.com/web/p/can-interface-ics/0402920)
| SENSORS     | LM335Z - if you choose to make your own sensors | [159-4685](https://uk.rs-online.com/web/p/temperature-humidity-sensor-ics/1594685)

### Suggested hardware

| Component   | Description                                     | Further information
|------------ |------------------------------------------------ |--------------------- |
| ENCLOSURE   | Plastic, general purpose, flange mount box      | [919-0391](https://uk.rs-online.com/web/p/general-purpose-enclosures/9190391)
| J4          | M12 5-pin male NMEA bus connector               | [877-1154](https://uk.rs-online.com/web/p/industrial-circular-connectors/8771154)
| CLIP        | 3mm LED panel clip                              | Sourced from eBay

### Assembly

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

## Module configuration

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
   
### Connecting relay outputs

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
