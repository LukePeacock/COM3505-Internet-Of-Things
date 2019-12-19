// main.cpp / sketch.ino
///
// a library or two... ///////////////////////////////////////////////////////
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_spi_flash.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>

#include "private.h" // stuff not for checking in
#include "unphone.h"

#include <ESPAsyncWebServer.h>
#include "joinme-2019.h"
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

// libraries for projects; comment out as required
//#include <Adafruit_VS1053.h>      // the audio chip
#include <RCSwitch.h>             // 433 MHz remote switching
//#include <DHTesp.h>               // temperature / humidity sensor
//#include <GP2Y1010_DustSensor.h>  // the Sharp dust sensor
//#include <Adafruit_NeoMatrix.h>   // neopixel matrix
//#include <Adafruit_NeoPixel.h>    // neopixels generally
//#include <Adafruit_MotorShield.h> // the hbridge motor driver
//#include <Adafruit_TSL2591.h>     // light sensor
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();


// OTA, MAC address, messaging, loop slicing//////////////////////////////////
int firmwareVersion = 1; // keep up-to-date! (used to check for updates)
char *getMAC(char *);    // read the address into buffer
char MAC_ADDRESS[13];    // MAC addresses are 12 chars, plus the NULL
void flash();            // the RGB LED
void loraMessage();      // TTN
void lcdMessage(char *); // message on screen
int loopIter = 0;        // loop slices

// globals for a wifi access point and webserver ////////////////////////////
String apSSID = String("IoT-Sockets-"); // SSID of the AP
String apPassword = _DEFAULT_AP_KEY;     // passkey for the AP
AsyncWebServer* webServer;               // async web server
String ip2str(IPAddress);                // helper for printing IP addresses

// web server utils /////////////////////////////////////////////////////////
// the replacement_t type definition allows specification of a subset of the
// "boilerplate" strings, so we can e.g. replace only the title, or etc.
typedef struct { int position; const char *replacement; } replacement_t;
void getHtml(String& html, const char *[], int, replacement_t [], int);
#define ALEN(a) ((int) (sizeof(a) / sizeof(a[0]))) // only in definition scope!
#define GET_HTML(strout, boiler, repls) \
  getHtml(strout, boiler, ALEN(boiler), repls, ALEN(repls));

//Function Protos
// we're not in arduino land any more, so need to declare function protos ///
byte hextoi(char c);
void initWebServer();
void hndlRoot(AsyncWebServerRequest *);
void hndlNotFound(AsyncWebServerRequest *);
void hndlWifichz(AsyncWebServerRequest *);
void hndlWifiStatus(AsyncWebServerRequest *);
void hndlWifi(AsyncWebServerRequest *);
void hndlWifiConfig(AsyncWebServerRequest *);

void hndlSocketChange(AsyncWebServerRequest *);
void socketSend(String, String, String, String&);
void socketForm(String&, String, String);
void apListForm(String&);
void printIPs();
void showLog(AsyncWebServerRequest *);
void log(String);
void convertDateTime(String &);

String dataLog = "<div class='datalog'>";
// SETUP: initialisation entry point /////////////////////////////////////////
void setup() {
  UNPHONE_DBG = true;
  unPhone::begin();

  // power management
  unPhone::printWakeupReason(); // what woke us up?
  unPhone::checkPowerSwitch();  // if power switch is off, shutdown

  // flash the internal RGB LED
  flash();

//  // LoRaWAN example
//  if(false) loraMessage();

//  // buzz a bit
//  for(int i = 0; i < 3; i++) {
//    unPhone::vibe(true);  delay(150); unPhone::vibe(false); delay(150);
//  }

  // Init the switch
  // Transmitter is connected to esp32 Pin #12
  mySwitch.enableTransmit(12);
  // We need to set pulse length as it's different from default
  mySwitch.setPulseLength(175);
    
  // Init Access point and networking stuff
    getMAC(MAC_ADDRESS);
  apSSID.concat(MAC_ADDRESS);
  Serial.printf("\nSetup...\nESP32 MAC = %s\n", MAC_ADDRESS);
  Serial.printf("WiFi Manager...\n");
  joinmeManageWiFi(apSSID.c_str(), apPassword.c_str()); // get network connection
  Serial.printf("WiFi Manager Done!\n\n");
  
    // Init webserver
    webServer = new AsyncWebServer(80);
    initWebServer();
    Serial.printf("Firmware Version: %d\n", firmwareVersion);
    
    // Check for and perform update
    vTaskDelay(2000/ portTICK_PERIOD_MS);
     joinmeOTAUpdate(
       firmwareVersion, _GITLAB_PROJ_ID,
       // "", // for publ repo "" works, else need valid PAT: _GITLAB_TOKEN,
       _GITLAB_TOKEN,
       "MyProjectThing%2Ffirmware%2F"
     );
    
    
 
  // say hi, store MAC, blink etc.
  Serial.printf("Hello from MyProjectThing...\n");
  getMAC(MAC_ADDRESS);          // store the MAC address
  Serial.printf("\nsetup...\nESP32 MAC = %s\n", MAC_ADDRESS);
  Serial.printf("battery voltage = %3.3f\n", unPhone::batteryVoltage());
    log("<p>setup...ESP32 MAC =" + String(MAC_ADDRESS) + "</p>");
    log("<p>Battery Voltage = " + String(unPhone::batteryVoltage()) + "</p>");
    
  lcdMessage("hello from " + WiFi.localIP().c_str()); // say hello on screen
 
  flash(); // flash the internal RGB LED
}

void loop() {
  D("\nentering main loop\n")
  while(1) {
    micros(); // update overflow

    // TODO do your stuff here...
    
    unPhone::checkPowerSwitch(); // if power switch is off shutdown
    
    // allow the protocol CPU IDLE task to run periodically
    if(loopIter % 2500 == 0) {
        if(loopIter % 25000 == 0)
        {
            D("completed loop %d, yielding 1000th time since last\n", loopIter);
            if (loopIter & 250000 == 0)
            {
                D("Completed Loop %d, yielding 10000th time time last\n", loopIter);
                Serial.printf("battery voltage = %3.3f\n", unPhone::batteryVoltage());
                log("Battery Voltage = " + String(unPhone::batteryVoltage()) + "</p>");
            }
        }
      delay(100); // 100 appears min to allow IDLE task to fire
    }
    loopIter++;

    // register button presses
    if(unPhone::button1())
    {
        mySwitch.send(5527299, 24);      // lookup the code to match your socket
        Serial.println("Socket 3 On!");
        lcdMessage("Socket 3 On!");
    }
    if(unPhone::button2())
    {
        mySwitch.send(5527308, 24);      // these codes are for type 1408 3
        Serial.println("Socket 3 Off");
        lcdMessage("Socket 3 Off!");
    }
    if(unPhone::button3()) Serial.println("button3");
  }
}

void log(String s){
    dataLog += s;
}

// misc utilities ////////////////////////////////////////////////////////////
// get the ESP's MAC address
char *getMAC(char *buf) { // the MAC is 6 bytes; needs careful conversion...
  uint64_t mac = ESP.getEfuseMac(); // ...to string (high 2, low 4):
  char rev[13];
  sprintf(rev, "%04X%08X", (uint16_t) (mac >> 32), (uint32_t) mac);

  // the byte order in the ESP has to be reversed relative to normal Arduino
  for(int i=0, j=11; i<=10; i+=2, j-=2) {
    buf[i] = rev[j - 1];
    buf[i + 1] = rev[j];
  }
  buf[12] = '\0';
  return buf;
}

// message on LCD
void lcdMessage(char *s) {
  unPhone::tftp->setCursor(0, 465);
  unPhone::tftp->setTextSize(2);
  unPhone::tftp->setTextColor(HX8357_CYAN, HX8357_BLACK);
  unPhone::tftp->print("                          ");
  unPhone::tftp->setCursor(0, 465);
  unPhone::tftp->print(s);
}

// cycle the LED
void flash() {
  unPhone::rgb(0, 0, 0); delay(300); unPhone::rgb(0, 0, 1); delay(300);
  unPhone::rgb(0, 1, 1); delay(300); unPhone::rgb(1, 0, 1); delay(300);
  unPhone::rgb(1, 1, 0); delay(300); unPhone::rgb(1, 1, 1); delay(300);
}

// web server utils /////////////////////////////////////////////////////////
void getHtml( // turn array of strings & set of replacements into a String
  String& html, const char *boiler[], int boilerLen,
  replacement_t repls[], int replsLen
) {
  for(int i = 0, j = 0; i < boilerLen; i++) {
    if(j < replsLen && repls[j].position == i)
      html.concat(repls[j++].replacement);
    else
      html.concat(boiler[i]);
  }
}
const char *templatePage[] = {    // we'll use Ex07 templating to build pages
  "<html><head><title>",                                                //  0
  "default title",                                                      //  1
  "</title>\n",                                                         //  2
  "<meta charset='utf-8'>",                                             //  3
  "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n"
  "<style>body{background:#FFF; color: #000; font-family: sans-serif;", //  4
  "font-size: 150%;} p{margin: 0pt;} .datalog{background:#b2b2b2; ",
  "font-size: 60%, margin: 2vw; padding: 2vw;}</style>\n",                                        //  5
  "</head><body>\n",                                                    //  6
  "<h2>Welcome to IoT Sockets!</h2>\n",                                       //  7
  "<!-- page payload goes here... -->\n",                               //  8
  "<!-- ...and/or here... -->\n",                                       //  9
  "\n<p><a href='/'>Home</a>&nbsp;&nbsp;&nbsp;</p>\n",                  // 10
  "</body></html>\n\n",                                                 // 11
};

void initWebServer() { // changed naming conventions to avoid clash with Ex06
  // register callbacks to handle different paths
  webServer->on("/", hndlRoot);              // slash
  webServer->onNotFound(hndlNotFound);       // 404s...
  webServer->on("/generate_204", hndlRoot);  // Android captive portal support
  webServer->on("/L0", hndlRoot);            // erm, is this...
  webServer->on("/L2", hndlRoot);            // ...IoS captive portal...
  webServer->on("/ALL", hndlRoot);           // ...stuff?
  webServer->on("/wifi", hndlWifi);     // Handle Wifi page
  webServer->on("/wificonfig", hndlWifiConfig);          // page for choosing an AP
  webServer->on("/wifichz", hndlWifichz);    // landing page for AP form submit
  webServer->on("/wifi_status", hndlWifiStatus);      // status check, e.g. IP address
  //webServer->on("/socket/change_status", hndlSocketChange); // Turn socket on or off;
  webServer->on("/socket_send", hndlSocketChange);
    webServer->on("/log", showLog);
  webServer->begin();
  dln(startupDBG, "HTTP server started");
}

void showLog(AsyncWebServerRequest *request){
    log("<p>Up Time: ");
    String time = "";
    convertDateTime(time);
    log(time);
    log(" (dd:hh:mm:ss) - WARNING: Resets after 50 Days!</p>");
    String s = dataLog.c_str();
    s += "</div>";
    replacement_t repls[] = { // the elements to replace in the boilerplate
      {  1, "Data Log: "},
      {  8, "" },
      {  9,  s.c_str()},
    };
    String htmlPage = ""; // a String to hold the resultant page
    GET_HTML(htmlPage, templatePage, repls);
    request->send(200, "text/html", htmlPage);
}

void convertDateTime(String &f){
    unsigned long time = millis();
    int millisec  = time % 100;    // Get  milliseconds of time
    int tseconds = time / 1000;    // Get time in seconds
     int seconds = tseconds % 60;    // Get seconds by modulo on tseconds with 60
    int tminutes = tseconds / 60;   // Get total minutes by dividing seconds by 60
    int minutes = tminutes % 60;    // Get minutes of hour by modulo tminutes and 60
    int thours = tminutes / 60;     // Get hour by dividing minutes by 60
    int hours = thours % 24;
    int days = (thours / 24) % 50;
    f += days;
    f += ":" + hours;
    f += ":" + minutes;
    f += ":" + seconds;
    f += ":" + millisec;
}

// webserver handler callbacks
void hndlNotFound(AsyncWebServerRequest *request) {
  dbg(netDBG, "URI Not Found: ");
  dln(netDBG, request->url());
  request->send(200, "text/plain", "URI Not Found");
}
void hndlRoot(AsyncWebServerRequest *request) {
  dln(netDBG, "serving page notionally at /");
  String s = "";
   
  // Socket 1408 3
  s += "<p style=\"display:inline\">Socket 1408 3: </p>";
  String f = "";
  socketForm(f, "1408", "3");
  s += f.c_str();
  s += "<p></p>";

  // Socket 1401 2
  s += "<p style=\"display:inline\">Socket 1401 2: </p>";
  f = "";
  socketForm(f, "1401", "2");
  s += f.c_str();
  s += "<p></p>";
    
    
  // All Sockets
  s += "<p><p style=\"display:inline\">All Sockets: </p>";
  s += "<form method='POST' action='socket_send' style=\"display:inline\"> ";
  s += "<input type='hidden' name='all' value='true'>";
  s += "<input type='hidden' name='status' value='true'>";
  s += "<input type='submit' value='On'></form>";
  s += "<form method='POST' action='socket_send' style=\"display:inline\"> ";
  s += "<input type='hidden' name='all' value='true'>";
  s += "<input type='hidden' name='status' value='false'>";
  s += "<input type='submit' value='Off'></form>";
  s += "</p>";
    
  replacement_t repls[] = { // the elements to replace in the boilerplate
    {  1, apSSID.c_str() },
    {  8, "" },
    {  9,  s.c_str()},
    { 10, "<p><a href='/wifi'>Wifi Settings</a>&nbsp;&nbsp;&nbsp;<a href='/log'>Log</a></p>" },
  };
  String htmlPage = ""; // a String to hold the resultant page
  GET_HTML(htmlPage, templatePage, repls);
  request->send(200, "text/html", htmlPage);
}

void socketForm(String& f, String socketNumOne, String socketNumTwo){
    dln(netDBG, "Creating Form for: " + socketNumOne + ":" + socketNumTwo);
    f += "<form method='POST' action='socket_send' style=\"display:inline\"> ";
    f += "<input type='hidden' name='socketNumOne' value='";
    f += socketNumOne;
    f += "'>";
    f += "<input type='hidden' name='socketNumTwo' value='";
    f += socketNumTwo;
    f += "'>";
    f += "<input type='hidden' name='status' value='true'>";
    f += "<input type='submit' value='On'></form>";
    f += "<form method='POST' action='socket_send' style=\"display:inline\"> ";
    f += "<input type='hidden' name='socketNumOne' value='";
    f += socketNumOne;
    f += "'>";
    f += "<input type='hidden' name='socketNumTwo' value='";
    f += socketNumTwo;
    f += "'>";
    f += "<input type='hidden' name='status' value='false'>";
    f += "<input type='submit' value='Off'></form>";
}

void hndlSocketChange(AsyncWebServerRequest *request) {
    dln(netDBG, "serving page at /socket_send");
    
    String title = "<h2>Changing Socket Status...</h2>";
    String message = "";
    String socketNumOne = "";
    String socketNumTwo = "";
    String status = "";
    bool all = false;
    for(uint8_t i = 0; i < request->args(); i++ ) {
        String test = "";
        test += request->argName(i).c_str();
        test + " :: ";
        test += request->arg(i).c_str();
        dln("netDBG", test.c_str());
      if(request->argName(i) == "socketNumOne")
        socketNumOne = request->arg(i);
      else if(request->argName(i) == "socketNumTwo")
        socketNumTwo = request->arg(i);
      else if(request->argName(i) == "status")
        status = request->arg(i);
      else if(request->argName(i) == "all")
        all = true;
    }
    if (all)
    {
        socketSend("1408", "3", status, message);
        message += "; ";
        socketSend("1401", "2", status, message);
    }
    else if (socketNumOne == "" || socketNumTwo == "")
        message = "<h2>Ooops, no Socket...?</h2>\n<p>Looks like a bug :-(</p>";
    else
        socketSend(socketNumOne, socketNumTwo, status, message);
    
    
    replacement_t repls[] = { // the elements to replace in the template
      { 1, apSSID.c_str() },
      { 7, title.c_str() },
      { 8, "" },
      { 9, message.c_str()},
    };
    String htmlPage = "";     // a String to hold the resultant page
    GET_HTML(htmlPage, templatePage, repls);

    request->send(200, "text/html", htmlPage);
}

void socketSend(String socketNumOne, String socketNumTwo, String status, String& message ){
    String s = "";
    // Send Signal
    // If socket is 1408 3, check status and send
    if (socketNumOne == "1408" && socketNumTwo == "3")
    {
        if (status == "true")
        {
            mySwitch.send(5527299, 24);      // lookup the code to match your socket
            s = "Socket 1408 3 On!";
            message += s.c_str();
        }
        else
        {
            mySwitch.send(5527308, 24);      // these codes are for type 1408 3
            s ="Socket 1408 3 Off";
            message += s.c_str();
        }
    }
    // else if socket is 1401 2, check status and send
    else if (socketNumOne == "1401" && socketNumTwo == "2")
    {
        if (status == "true")
        {
            mySwitch.send(1398211, 24);      // lookup the code to match your socket
            s ="Socket 1401 2 On!";
            message += s.c_str();
        }
        else
        {
            mySwitch.send(1398220, 24);      // these codes are for type 1408 3
            s = "Socket 1401 2 Off";
            message += s.c_str();
        }
    }
    // Else error, socket not found
    else {
        s = "Socket Not Found";
        message = "<h2>Ooops, " + s + "...?</h2>\n<p>Looks like a bug :-(</p>";
    }
    Serial.println(s.c_str());
    lcdMessage((char*)s.c_str());
}
void hndlWifi(AsyncWebServerRequest *request) {
    dln(netDBG, "serving page at /wifi");
    String s = "";
    s +="<p>Choose a <a href='/wificonfig'>wifi access point</a>.</p>";
    s +="<p>Check <a href='/wifi_status'>wifi status</a>.</p>";
    replacement_t repls[] = { // the elements to replace in the boilerplate
      {  1, apSSID.c_str() },
      {  8, "" },
      {  9, s.c_str()},
    };
    String htmlPage = ""; // a String to hold the resultant page
    GET_HTML(htmlPage, templatePage, repls);
    request->send(200, "text/html", htmlPage);
}
void hndlWifiConfig(AsyncWebServerRequest *request) {
  dln(netDBG, "serving page at /wificonfig");

  String form = ""; // a form for choosing an access point and entering key
  apListForm(form);
  replacement_t repls[] = { // the elements to replace in the boilerplate
    { 1, apSSID.c_str() },
    { 7, "<h2>Network configuration</h2>\n" },
    { 8, "" },
    { 9, form.c_str() },
  };
  String htmlPage = ""; // a String to hold the resultant page
  GET_HTML(htmlPage, templatePage, repls);

  request->send(200, "text/html", htmlPage);
}
void hndlWifichz(AsyncWebServerRequest *request) {
  dln(netDBG, "serving page at /wifichz");

  String title = "<h2>Joining wifi network...</h2>";
  String message = "<p>Check <a href='/wifi_status'>wifi status</a>.</p>";

  String ssid = "";
  String key = "";
  for(uint8_t i = 0; i < request->args(); i++ ) {
      String test = "";
      test += request->argName(i).c_str();
      test += " :: ";
      test +=request->arg(i).c_str();
      dln(netDBG,  test.c_str());
    if(request->argName(i) == "ssid")
      ssid = request->arg(i);
    else if(request->argName(i) == "key")
      key = request->arg(i);
  }

  if(ssid == "") {
    message = "<h2>Ooops, no SSID...?</h2>\n<p>Looks like a bug :-(</p>";
  } else {
    char ssidchars[ssid.length()+1];
    char keychars[key.length()+1];
    ssid.toCharArray(ssidchars, ssid.length()+1);
    key.toCharArray(keychars, key.length()+1);
    WiFi.begin(ssidchars, keychars);
  }

  replacement_t repls[] = { // the elements to replace in the template
    { 1, apSSID.c_str() },
    { 7, title.c_str() },
    { 8, "" },
    { 9, message.c_str() },
  };
  String htmlPage = "";     // a String to hold the resultant page
  GET_HTML(htmlPage, templatePage, repls);

  request->send(200, "text/html", htmlPage);
  //request->redirect("/wifi");

}
void hndlWifiStatus(AsyncWebServerRequest *request) { // UI to check connectivity
  dln(netDBG, "serving page at /wifi_status");

  String s = "";
  s += "<ul>\n";
  s += "\n<li>SSID: ";
  s += WiFi.SSID();
  s += "</li>";
  s += "\n<li>Status: ";
  switch(WiFi.status()) {
    case WL_IDLE_STATUS:
      s += "WL_IDLE_STATUS</li>"; break;
    case WL_NO_SSID_AVAIL:
      s += "WL_NO_SSID_AVAIL</li>"; break;
    case WL_SCAN_COMPLETED:
      s += "WL_SCAN_COMPLETED</li>"; break;
    case WL_CONNECTED:
      s += "WL_CONNECTED</li>"; break;
    case WL_CONNECT_FAILED:
      s += "WL_CONNECT_FAILED</li>"; break;
    case WL_CONNECTION_LOST:
      s += "WL_CONNECTION_LOST</li>"; break;
    case WL_DISCONNECTED:
      s += "WL_DISCONNECTED</li>"; break;
    default:
      s += "unknown</li>";
  }

  s += "\n<li>Local IP: ";     s += ip2str(WiFi.localIP());
  s += "</li>\n";
  s += "\n<li>Soft AP IP: ";   s += ip2str(WiFi.softAPIP());
  s += "</li>\n";
  s += "\n<li>AP SSID name: "; s += apSSID;
  s += "</li>\n";

  s += "</ul></p>";

  replacement_t repls[] = { // the elements to replace in the boilerplate
    { 1, apSSID.c_str() },
    { 7, "<h2>Status</h2>\n" },
    { 8, "" },
    { 9, s.c_str() },
  };
  String htmlPage = ""; // a String to hold the resultant page
  GET_HTML(htmlPage, templatePage, repls);

  request->send(200, "text/html", htmlPage);
}
void apListForm(String& f) { // utility to create a form for choosing AP
  const char *checked = " checked";
  int n = WiFi.scanNetworks();
  dbg(netDBG, "scan done: ");

  if(n == 0) {
    dln(netDBG, "no networks found");
    f += "No wifi access points found :-( ";
    f += "<a href='/wifi'>Back</a><br/><a href='/wificonfig'>Try again?</a></p>\n";
  } else {
    dbg(netDBG, n); dln(netDBG, " networks found");
    f += "<p>Wifi access points available:</p>\n"
         "<p><form method='POST' action='wifichz'> ";
    for(int i = 0; i < n; ++i) {
      f.concat("<input type='radio' name='ssid' value=\"");
      f.concat(WiFi.SSID(i));
      f.concat("\"");
      f.concat(checked);
      f.concat(">");
      f.concat(WiFi.SSID(i));
      f.concat(" (");
      f.concat(WiFi.RSSI(i));
      f.concat(" dBm)");
      f.concat("<br/>\n");
      checked = "";
    }
    f += "<br/>Pass key: <input type='textarea' name='key'><br/><br/> ";
    f += "<input type='submit' value='Submit'></form></p>";
  }
}
String ip2str(IPAddress address) { // utility for printing IP addresses
  return
    String(address[0]) + "." + String(address[1]) + "." +
    String(address[2]) + "." + String(address[3]);
}
