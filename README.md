# esp8266IRServer

## Credit
This is a copy of the code originally found here https://alexbloggt.com/universal-infrarot-websteuerung-ueber-esp8266/
ODC & Pronto code features have been added.

## Usage
Connect an IR led (with resistor) to the appropriate pins and supply power.  WiFi configuration is via WiFI manager, on first poweron an AP

The IP address will print on the serial console once connected to the network.  Arbitrary IR codes can be sent in this format
 with a SSID "IRSVR" will be started to allow for configuration of local wireless settings
 
http://IP/ir?code=CODE&bits=BITS&protocol=Protocol

CODE is the code in HEX
BITS is the number of BITS to send
PROTOCOL is the Protocol name as defined in the IRRemoteESP8266 Library

Commands can also be  sent with Original Button Codes (OBC) for Samsung, NEC, NECx2, RC5*, RC6*, JVC* & Sony* protocols. * are untested, please post an issue if you test them and they work/dont work.

http://IP/ir?deviceCode=X&subDeviceCode=Y&obc=Z&protocol=Protocol

subDeviceCode is Optional, if omitted for Samsung or NEC protocol, the deviceCode is repeated.

Pronto codes are supported as well
http://IP/ir?pronto=X

X must be a learnt pronto code (starts with 0000), in HEX, with each set of 4 characters separated by a space (%20 when urlencoded).

There's some other undocumented features as well :-)
