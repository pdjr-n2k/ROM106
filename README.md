# ROM106 - NMEA 2000 relay output module

**ROM106** is a specialisation of
[NOP100](https://github.com/preeve9534/NOP100)
and implements a six channel NMEA 2000 relay output module which
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

**ROM106** accepts PGN 127502 Switch Bank Control messages addressed
to its module's instance number, updating its relay outputs to the
requested states.
Operation of individual relays is queued so that any request for state
change of multiple relays cannot result in a possibly problematic
concurrent relay switching load.

The module reports module status by regularly broadcasting a PGN 127501
Binary Status Report message.
Additionally, a report is broadcast immediately following the receipt and
handling of a PGN 127502 Switch Bank Control message.
The regular transmission interval can be configured by the user.

## Module configuration

**ROM106** understands the following configuration parameters.

| Address | Name                         | Default value | Description |
| :---:   | :---                         | :---:         | :--- |
| 0x01    | MODULE INSTANCE NUMBER       | 0xFF          | Used as a unique identifier for this switchbank. Must be set by the user to a value between 0 and 252 before the module will transmit status reports. |
| 0x02    | PGN 127501 TRANSMIT INTERVAL | 0x04          | Sets the basic transmit interval for broadcast of module status reports.|

