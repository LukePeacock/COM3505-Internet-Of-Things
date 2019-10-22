// 7_web_utilities.ino
// COM3505 lab exercises sketch template
#include "Exercise_7_Web_Utilities.h"

// initialisation entry point
void setup() {
  // initialize web server
  dln(startupDBG, "\nSetup...");
  Serial.begin(115200);
  getMAC(MAC_ADDRESS);
  startAP();
  startWebServer();  
}

// task loop entry point
void loop() {
  uint64_t time = millis();
  if(time % 5000000 == 0){
    dln(netDBG, "Calling webServer.handleClient()...");
  }

  if(time++ % 1000 == 0){
     return;
  }
  webServer.handleClient();
}

void startAP(){
  apSSID = String("Thing-");
  apSSID.concat(MAC_ADDRESS);

  if(!WiFi.mode(WIFI_AP_STA))
    dln(startupDBG, "Failed to set WiFi mode");
  if(!WiFi.softAP(apSSID.c_str(), "thisisapassword"))
    dln(startupDBG, "Failed to start soft AP");
  printIPs();
}
void printIPs() {
  if(startupDBG) { // easier than the debug macros for multiple lines etc.
    Serial.print("AP SSID: ");
    Serial.print(apSSID);
    Serial.print("; IP address(es): local=");
    Serial.print(WiFi.localIP());
    Serial.print("; AP=");
    Serial.println(WiFi.softAPIP());
  }
  if(netDBG)
    WiFi.printDiag(Serial);
}

void startWebServer() {
  // register callbacks to handle different paths
  webServer.on("/", handleRoot);
  webServer.on("/hello", handleHello);

  // 404s...
  webServer.onNotFound(handleNotFound);

  webServer.begin();
  dln(startupDBG, "HTTP server started");
}

// HTML page creation utilities //////////////////////////////////////////////
// (we'll see a better version in Ex07!)
String getPageTop() {
  return
    "<html><head><title>COM3506 IoT [ID: " + apSSID + "]</title>\n"
    "<meta charset=\"utf-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "\n<style>body{background:#FFF; color: #000; "
    "font-family: sans-serif; font-size: 150%;}</style>\n"
    "</head><body>\n"
  ;
};
String getPageBody() {
  return "<h2>Welcome to Thing!</h2>\n";
}
String getPageFooter() {
  return "\n<p><a href='/'>Home</a>&nbsp;&nbsp;&nbsp;</p></body></html>\n";
}


// webserver handler callbacks ///////////////////////////////////////////////
void handleNotFound() {
  dbg(netDBG, "URI Not Found: ");
  dln(netDBG, webServer.uri());
  webServer.send(200, "text/plain", "URI Not Found");
}
void handleRoot() {
  dln(netDBG, "serving page notionally at /");
  String toSend = getPageTop();
  toSend += getPageBody();
  toSend += getPageFooter();
  webServer.send(200, "text/html", toSend);
}
void handleHello() {
  dln(netDBG, "serving /hello");
  webServer.send(
    200,
    "text/plain",
    "Hello! Have you considered sending your lecturer a large gift today? :)\n"
  );
}




// why not have getMAC return a String? we could, but see:
// https://hackingmajenkoblog.wordpress.com/2016/02/04/the-evils-of-arduino-strings/
// https://www.youtube.com/watch?v=eVGvPsCrQ_Y
void getMAC(char *buf) { // the MAC is 6 bytes, so needs careful conversion...
  uint64_t mac = ESP.getEfuseMac(); // ...to string (high 2, low 4):
  char rev[13];
  sprintf(rev, "%04X%08X", (uint16_t) (mac >> 32), (uint32_t) mac);

  // the byte order in the ESP has to be reversed relative to normal Arduino
  for(int i=0, j=11; i<=10; i+=2, j-=2) {
    buf[i] = rev[j - 1];
    buf[i + 1] = rev[j];
  }
  buf[12] = '\0';
}
