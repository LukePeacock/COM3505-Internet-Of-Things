#ifndef Exercise_6_H 
#define Exercise_6_H

// the wifi and HTTP server libraries ////////////////////////////////////////
#include <WiFi.h>
#include <WebServer.h>

// MAC address ///////////////////////////////////////////////////////////////
char MAC_ADDRESS[13]; // MAC addresses are 12 chars, plus the NULL terminator
void getMAC(char *);


// debugging infrastructure; setting different DBGs true triggers prints ////
#define dbg(b, s) if(b) Serial.print(s)
#define dln(b, s) if(b) Serial.println(s)
#define startupDBG      true
#define loopDBG         true
#define monitorDBG      true
#define netDBG          true
#define miscDBG         true
#define analogDBG       true
#define otaDBG          true

// globals for a wifi access point and webserver /////////////////////////////
String apSSID;                  // SSID of the AP
WebServer webServer(80);     // a simple web server

// function protos
void setup();
void loop();
void startAP();
void printIPs();
void startWebServer();
void handleNotFound();
void handleRoot();
void handleHello();

#endif
