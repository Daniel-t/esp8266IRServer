# esp8266IRServer

## Credit
This is a copy of the code orginally found here https://alexbloggt.com/universal-infrarot-websteuerung-ueber-esp8266/
Only minor modifications have been made.

## Usage
Connect an IR led (with resistor) to the appropriate pins and supply power.  WiFi configuration is via WiFI manager, on first poweron an AP

The IP address will print on the serial console once connected to the network.  Arbitary IR codes can be sent in this format
 with a SSID "IRSVR" will be started to allow for configuration of local wireless settings
 
http://IP/ir?code=CODE&bits=BITS&protocol=Protocol

CODE is the code in HEX
BITS is the number of BITS to send
PROTOCOL is the Protocol name as defined in the IRRemoteESP8266 Library

Commands can also be  sent with Original Button Codes (OBC) for Samsung and NEC protocols

http://IP/ir?deviceCode=X&subDeviceCode=Y&obc=Z&protocol=Protocol

subDeviceCode is Optional, if omitted for Samsung protocol, the deviceCode is repeated.
