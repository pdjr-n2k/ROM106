# ROM106 - NMEA 2000 relay output module

This project is a specialisation of
[NOP100](https://github.com/preeve9534/NOP100).

**ROM106** is a six channel NMEA 2000 relay output module which
presents on the host NMEA bus as a switchbank device with Class
Code 30 (Electrical Distribution) and Function Code 140 (Load
Controller).

The module transmits PGN 127501 Binary Status Report messages and
responds to PGN 127502 Switch Bank Control messages addressed to
the module's instance number.

The module is powered from the host NMEA bus and has an LEN of 0.5.

## Relay outputs

**ROM106**'s relay output sub-system consists of six bistable relays
with zero-volt common (CO), normally-open (NO) and normally-closed (NC)
terminals availble for the connection of external circuits.

Relay connections are rated for a maximum of 5A at up to 220VAC
or 30VDC.

## Relay operation and status reporting

**ROM106**'s relay states are updated on the receipt of a PGN 127502
Switch Bank Control message addressed to the module's instance number.

Operation of individual relays is queued so that any request for state
change of multiple relays cannot result in a possibly problematic
concurrent relay switching load.

**ROM106** reports module status by regularly broadcasting a PGN 127501
Binary Status Report message.
Additionally, a report is always transmitted immediately following the
receipt and handling of a PGN 127502 message.
The default transmission interval can be configured by the user.

## Module configuration

**ROM106** understands the following configuration parameters.

| Address | Name                         | Default value | Description |
| :---:   | :---                         | :---:         | :--- |
| 0x01    | MODULE INSTANCE NUMBER       | 0xFF          | This unique switchbank identifier |
| 0x02    | PGN 127501 TRANSMIT INTERVAL | 0x04          | |



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
| &#9898;&#9898;&#9898;&#9898;&#9898;&#9898;&#9898;&#9898; | Led check (displayed immediately power is applied. |
| &#9681;&#9681;&#9681;&#9681;&#9681;&#9681;&#9681;&#9681; | Configured instance number (displayed after LED check).
| &#9898;&#9711;&#9681;&#9681;&#9681;&#9681;&#9681;&#9681; | Module active not transmitting (relay channels on).
| &#9898;&#9898;&#9681;&#9681;&#9681;&#9681;&#9681;&#9681; | Module active and transmitting (relay channels on).
| &#9711;&#9711;&#9898;&#9898;&#9898;&#9898;&#9898;&#9898; | Queue overflow. |

## See also

* [SIM108](https://github.com/preeve9534/SIM108/) - NMEA 2000 switch input module.
