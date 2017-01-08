#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
 
// WLAN data 
const char* ssid = "...";
const char* password = "...";
 
// static IP
IPAddress ip(192,168,0,201);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
 
MDNSResponder mdns;
ESP8266WebServer server(80);
 
// set your pin here  NOTE: Mapping to Arduino PIN
IRsend irsend(2);
 
void handleRoot() {
 server.send(200, "text/html", "Please specify command! Form: /ir?code=xxx&bits=xx&protocol=x");
}
 
void handleIr(){
  String codestring=server.arg("code");
  String protocol=server.arg("protocol");
  String bitsstring=server.arg("bits");
 
  String webOutput = "Protocol: "+protocol+"; Code: "+codestring+"; Bits: "+bitsstring;
  Serial.println(webOutput);
    
  if ((codestring != "")&&(bitsstring != "")){
    //unsigned long code = codestring.toInt();
    char tarray[15]; 
    codestring.toCharArray(tarray, sizeof(tarray));
    unsigned long code = strtoul(tarray,NULL,16);  
    // unsigned long code = atol(codestring);
    int bits = bitsstring.toInt();
    Serial.println(code);
    
    if (protocol == "NEC"){
      server.send(200, "text/html", webOutput);
      irsend.sendNEC(code, bits);
    }
    else if (protocol == "Sony"){
      server.send(200, "text/html", webOutput);
      irsend.sendSony(code, bits);
    }
    else if (protocol == "Whynter"){
      server.send(200, "text/html", webOutput);
      irsend.sendWhynter(code, bits);
    }
    else if (protocol == "LG"){
      server.send(200, "text/html", webOutput);
      irsend.sendLG(code, bits);
    }
    else if (protocol == "RC5"){
      server.send(200, "text/html", webOutput);
      irsend.sendRC5(code, bits);
    }
    else if (protocol == "RC6"){
      server.send(200, "text/html", webOutput);
      irsend.sendRC6(code, bits);
    }
    else if (protocol == "DISH"){
      server.send(200, "text/html", webOutput);
      irsend.sendDISH(code, bits);
    }
    else if (protocol == "SharpRaw"){
      server.send(200, "text/html", webOutput);
      irsend.sendSharpRaw(code, bits);
    }
    else if (protocol == "Samsung"){
      server.send(200, "text/html", webOutput);
      irsend.sendSAMSUNG(code, bits);
    }
    else {
      server.send(200, "text/html", "Protocol not implemented!");
    }
  }
  else {
    server.send(200, "text/html", "Missing code or bits!");
  }
}
 
void handleNotFound(){
  server.send(404, "text/plain", "404");
}
 
void setup(void){
  irsend.begin();
  Serial.begin(9600);
  WiFi.begin(ssid, password);
 // WiFi.config(ip, gateway, subnet);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("=");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (mdns.begin("irsvr", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  server.on("/ir", handleIr); 
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();
}
