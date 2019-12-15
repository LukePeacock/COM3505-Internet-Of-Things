#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(115200);
  // Transmitter is connected to esp32 Pin #12
  mySwitch.enableTransmit(12);
  // We need to set pulse length as it's different from default
  mySwitch.setPulseLength(175);
}

void loop() {
  mySwitch.send(5527299, 24);      // lookup the code to match your socket
  Serial.println("Socket 1 On");
  delay(10000);  
  mySwitch.send(5527308, 24);      // these codes are for type 1406
  Serial.println("Socket 1 Off");
  delay(10000);
}
