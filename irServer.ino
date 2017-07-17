#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <FS.h>  

WiFiManager wifiManager;


MDNSResponder mdns;
ESP8266WebServer server(80);
 
int SERIAL_SPEED = 115200;

int RECV_PIN = 5; //an IR detector/demodulator is connected to GPIO  //D1 
int SEND_PIN = 4; // D2 paired with a 1.6 Ohm resistor

char * CONFIG_PATH = "config.txt";
char * CONFIG_BACKUP_PATH = "config.bak";


IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN);

decode_results  results1;        // Somewhere to store the results
decode_results  results;        // Somewhere to store the results

void handleRoot() {
 server.send(200, "text/html", "Please specify command! Form: /ir?code=xxx&bits=xx&protocol=x");
}
 
void handleIr(){
  String codestring=server.arg("code");
  String protocol=server.arg("protocol");
  String bitsstring=server.arg("bits");

  String deviceCode=server.arg("deviceCode");
  String subDeviceCode=server.arg("subDeviceCode");
  String obc=server.arg("obc");
  String pronto=server.arg("pronto");

//  String webOutput = "Protocol: "+protocol+"; Code: "+codestring+"; Bits: "+bitsstring + " - ("+deviceCode + subDeviceCode + obc +")";
  String webOutput = "";
  
  Serial.println(webOutput);

  unsigned long code =0;
  int rc5_control_bit=0;
  
  if ((codestring != "")&&(bitsstring != "")){
    //unsigned long code = codestring.toInt();
    char tarray[15]; 
    codestring.toCharArray(tarray, sizeof(tarray));
    code = strtoul(tarray,NULL,16);  
    // unsigned long code = atol(codestring);
  }
  int bits = bitsstring.toInt();

  
  if ((obc != "")&&(deviceCode != "")){
    //convert OBC & deviceCode to hex CodeString


   int iDeviceCode=StrToUL(deviceCode);
   int iDeviceCodeLSB=flipBits(iDeviceCode); 
   int iDeviceCodeLSB_INV=iDeviceCodeLSB ^ 0xFF; 
   dump(iDeviceCode, iDeviceCodeLSB, iDeviceCodeLSB_INV);
   
   int iSubDeviceCode;
   int iSubDeviceCodeLSB;
   int iSubDeviceCodeLSB_INV;
    if ((subDeviceCode == "")){
      iSubDeviceCode=iDeviceCode;
      iSubDeviceCodeLSB=iDeviceCodeLSB; 
      iSubDeviceCodeLSB_INV=iDeviceCodeLSB_INV; 
    } else {
       iSubDeviceCode=StrToUL(subDeviceCode);
       iSubDeviceCodeLSB=flipBits(iSubDeviceCode); 
       iSubDeviceCodeLSB_INV=iSubDeviceCodeLSB ^ 0xFF; 
    }

   int iOBC=StrToUL(obc);
   int iOBCLSB=flipBits(iOBC); 
   int iOBCLSB_INV=iOBCLSB ^ 0xFF; 
   dump(iOBC,iOBCLSB,iOBCLSB_INV);
   
   Serial.println(iDeviceCodeLSB,HEX);
   Serial.println(iOBCLSB,HEX);
   Serial.println("----");
   if (protocol=="Samsung") {
    code=combineBytes(iDeviceCodeLSB,iSubDeviceCodeLSB,iOBCLSB,iOBCLSB_INV);
   } else if (protocol=="NEC" || protocol=="NECx2") {
    code=combineBytes(iDeviceCodeLSB,iDeviceCodeLSB_INV,iOBCLSB,iOBCLSB_INV);
   } else if (protocol=="RC6") {
    /*NOT TESTED*/
    code=combineBytes(0,0,iDeviceCode,iOBC);
    bits=20;
   }  else if (protocol=="RC5") {
    /*NOT TESTED*/
    /*control=1,device=5,command=6*/
    rc5_control_bit=abs(rc5_control_bit-1);
    code=rc5_control_bit;
    code+=code<<5 + iDeviceCodeLSB;
    code+=code<<6 + iOBCLSB;   
    bits=12;
   } else if (protocol=="JVC") {
    /*NOT TESTED*/
    code=combineBytes(0,0,iDeviceCodeLSB,iOBCLSB);
    bits=16;
   }  else if (protocol=="Sony") {
    /*NOT TESTED & highly suspect, need to seem some example codes*/
    code=iOBCLSB;
    if (subDeviceCode != "") {
      bits=20;
      code=code<<5+iDeviceCodeLSB;
      code=code<<8+iSubDeviceCodeLSB;

    }else if (iDeviceCode > 2^5){
      bits=15;
      code=code<<8+iDeviceCodeLSB;
    } else {
      bits=12;
      code=code<<5+iDeviceCodeLSB;
    }
   } else  {
    code=0;
    server.send(404, "text/html", "Protocol not implemented for OBC!");
   }
   
   Serial.println("---");
   Serial.println(code,HEX);
   Serial.println("---");
   
  }
  
  if (code!=0){
    Serial.println(code,HEX);
    
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
      server.send(404, "text/html", "Protocol not implemented!");
    }
  }  else if (pronto != "") {
    //pronto code
    //blocks of 4 digits in hex
    //preamble is 0000 FREQ LEN1 LEN2
    //followed by ON/OFF durations in FREQ cycles
    //Freq needs a multiplier
    //blocks separated by %20
    //we are ignoring LEN1 & LEN2 for this use case as not allowing for repeats
    //just pumping all 
    int spacing=5;
    int len= pronto.length();
    int out_len =((len-4)/spacing)-3;
    unsigned int prontoCode[out_len];
    unsigned long timeperiod;
    unsigned long multiplier = .241246 ;

    int pos=0;
    unsigned long hz;
    if (pronto.substring(pos,4) != "0000"){
      server.send(404,"text/html","unknown pronto format!");
      //unknown pronto format
    } else {
      pos+=spacing;
      
      hz=strtol(pronto.substring(pos,pos+4).c_str(),NULL,16);
      hz=(hz * .241246);
      hz=1000000/hz;
      //XXX TIMING IS OUT
      timeperiod=1000000/hz;
      pos+=spacing;//hz
      pos+=spacing;//LEN1
      pos+=spacing;//LEN2
      delay(0);
      for (int i = 0; i < out_len; i++){
          prontoCode[i]=(strtol(pronto.substring(pos,pos+4).c_str(),NULL,16) * timeperiod) + 0.5;
          pos+=spacing;
      }
      //sendRaw
      yield();
     
      irsend.sendRaw(prontoCode,out_len,hz/1000);
      server.send(200,"text/html","pronto code!");
    }
    
  } else {
    server.send(404, "text/html", "Missing code or bits!");
  }
}

unsigned long StrToUL (String str){
    char tarray[15]; 
    str.toCharArray(tarray, sizeof(tarray));
    unsigned long code = strtoul(tarray,NULL,10);  
    return (code);
}

unsigned long combineBytes(int a1,int a2, int a3, int a4){
   unsigned long code=0;
   code=code+a1;
   code=(code<<8)+a2;
   code=(code<<8)+a3;
   code=(code<<8)+a4;
   return(code);
}

int flipBits(unsigned char b){
  return ( (b * 0x0202020202ULL & 0x010884422010ULL) % 1023);
}

void dump(int a1,int a2,int a3){
  Serial.print(a1,HEX);
  Serial.print("-");
  Serial.print(a2,HEX);
  Serial.print("-");
  Serial.println(a3,HEX);
}
void handleNotFound(){
  server.send(404, "text/plain", "404");
}

void handleReset(){
  wifiManager.resetSettings();
}

void handleLoadConfig(){
  File f;
  if (!SPIFFS.exists(CONFIG_PATH)){
    //doesn't exist create blank config
    f = SPIFFS.open(CONFIG_PATH,"w");
    f.println("{\"pages\":[{\"name\":\"New Page\",\"buttons\":[]}]}");
    Serial.println("CREATED");
    f.close();
  }
  f = SPIFFS.open(CONFIG_PATH,"r");
  String s=f.readStringUntil('\n');
  Serial.println(s);
  f.close();
  String callback=server.arg("callback");
  server.send(200,"text/plain",callback+"("+s+")"); 
}

void handleLoadBackupConfig(){
  File f;
  if (!SPIFFS.exists(CONFIG_BACKUP_PATH)){
    f = SPIFFS.open(CONFIG_BACKUP_PATH,"w");
    f.println("{\"pages\":[{\"name\":\"New Page\",\"buttons\":[]}]}");
    Serial.println("CREATED");
    f.close();
  }
  f = SPIFFS.open(CONFIG_BACKUP_PATH,"r");
  String s=f.readStringUntil('\n');
  Serial.println(s);
  f.close();
  String callback=server.arg("callback");
  server.send(200,"text/plain",callback+"("+s+")"); 
 
}

void handleSaveConfig(){
  Serial.println("Saving");
  File f;
  File f2;
  if (SPIFFS.exists(CONFIG_PATH)){
    f = SPIFFS.open(CONFIG_PATH,"r");
    f2 = SPIFFS.open(CONFIG_BACKUP_PATH,"w");
    String s=f.readStringUntil('\n');
    f2.println(s);
    Serial.println("BACKED UP");
    f.close();
    f2.close();
    Serial.println("Config backuped");
  }
  f = SPIFFS.open(CONFIG_PATH,"w");
  String newConfig=server.arg("config");
  f.println(newConfig);
  Serial.println(newConfig);
  f.close();
  String callback=server.arg("callback");
  server.send(200,"text/plain",callback+"('SAVED')"); 
 
}

void handleDeleteConfig(){
  File f;
  String callback=server.arg("callback");
  if (!SPIFFS.exists(CONFIG_PATH)){
    Serial.println("FILE NOT FOUND");
  } else {
    SPIFFS.remove(CONFIG_PATH);
    Serial.println("FILE DELETED");
  }
  server.send(200,"text/plain",callback+"(\"File Deleted\")");
 };


void learnHandler(){
  Serial.println("In Learning Handling");
  String callback=server.arg("callback");
   
  String proto="";
  {  // Grab an IR code
   // dumpInfo(&results);           // Output the results
    switch (results.decode_type) {
      default:
      case UNKNOWN:      proto=("UNKNOWN");       break ;
      case NEC:          proto=("NEC");           break ;
      case SONY:         proto=("Sony");          break ;
      case RC5:          proto=("RC5");           break ;
      case RC6:          proto=("RC6");           break ;
      case DISH:         proto=("DISH");          break ;
      case SHARP:        proto=("SHARP");         break ;
      case JVC:          proto=("JVC");           break ;
      case SANYO:        proto=("Sanyo");         break ;
      case MITSUBISHI:   proto=("MITSUBISHI");    break ;
      case SAMSUNG:      proto=("Samsung");       break ;
      case LG:           proto=("LG");            break ;
      case WHYNTER:      proto=("Whynter");       break ;
     // case AIWA_RC_T501: Serial.print("AIWA_RC_T501");  break ;
      case PANASONIC:    proto=("PANASONIC");     break ;
    }
    //results->value
    //Serial.print(results->value, HEX);
    
    Serial.println("Here");           // Blank line between entries
    irrecv.resume();              // Prepare for the next value
    String output=callback+"({protocol:\""+proto+"\", value:\""+String(results.value,HEX)+"\", bits:\""+String(results.bits)+"\"})";
    Serial.println(output);
    server.send(200, "text/html", output);
  }
}

void setup(void){
  irsend.begin();
  Serial.begin(SERIAL_SPEED);
  SPIFFS.begin();
  
  
  wifiManager.autoConnect("IRSVR");
  
  if (mdns.begin("irsvr", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  
  server.on("/", handleRoot);
  server.on("/ir", handleIr); 
  server.on("/reset",handleReset);
  server.on("/learn",learnHandler);
  server.on("/loadConfig",handleLoadConfig);
  server.on("/loadBackupConfig",handleLoadBackupConfig);
  
  server.on("/saveConfig",handleSaveConfig);
  server.on("/deleteConfig",handleDeleteConfig);

  server.onNotFound(handleNotFound);
    
  server.begin();
  Serial.println("HTTP server started");
  irrecv.enableIRIn();

}
 
void loop(void){
  server.handleClient();
  if (irrecv.decode(&results1)){
    Serial.println("Signal received");
     irrecv.decode(&results);
     irrecv.resume();
  }
  
}
