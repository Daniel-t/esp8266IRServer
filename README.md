# esp8266IRServer

## Credit
This is a copy of the code orginally found here https://alexbloggt.com/universal-infrarot-websteuerung-ueber-esp8266/
Only minor modifications have been made.

## Usage
Connect an IR led (with resistor) to the appropriate pins and supply power.

The IP address will print on the serial console once connected to the network.  Arbitary IR codes can be sent in this format

http://IP/ir?code=CODE&bits=BITS&protocol=Protocol

CODE is the code in HEX
BITS is the number of BITS to send
PROTOCOL is the Protocol name as defined in the IRRemoteESP8266 Library
