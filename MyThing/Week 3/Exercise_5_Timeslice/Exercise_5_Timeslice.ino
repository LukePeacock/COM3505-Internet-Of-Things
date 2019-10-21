// MyThing.ino
// COM3505 lab exercises sketch template
int redLED = 32;
int yellowLED = 15;
int greenLED = 12;
int state = 0;
boolean pressed = false;


// initialisation entry point
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  Serial.begin(115200);
}

// task loop entry point
int timep = 0;
void loop() {
  timep = millis();
  if (timep % 10000 == 0) {
    if (digitalRead(yellowLED) == 0) {digitalWrite(yellowLED, HIGH);}
    else {digitalWrite(yellowLED, LOW);}
  }
  if (timep % 1000 == 0) {
    if (digitalRead(redLED) == 0){
      digitalWrite(redLED, HIGH);
      digitalWrite(greenLED, LOW);
    }
    else {
      digitalWrite(redLED, LOW);
      digitalWrite(greenLED, HIGH);
    }
  }
}
