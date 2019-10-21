// MyThing.ino
// COM3505 lab exercises sketch template

int pushButton = 14;
int externalLED = 32;
int state = 0;
int previousMills;
unsigned long interval = 100;


// initialisation entry point
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(pushButton, INPUT_PULLUP);
  pinMode(externalLED, OUTPUT);
  Serial.begin(115200);
}

// task loop entry point
void loop() {
  unsigned long currentMills = millis();
  int button = digitalRead(pushButton);
  if (currentMills - previousMills >= interval && button == 0) {
    previousMills = currentMills;

    if (state == 0) { state = 1;}
    else {state = 0;};
    
  }
  digitalWrite(externalLED, state);
}
