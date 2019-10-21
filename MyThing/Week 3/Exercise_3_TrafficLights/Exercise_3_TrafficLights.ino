// MyThing.ino
// COM3505 lab exercises sketch template

int pushButton = 14;
int redLED = 32;
int yellowLED = 15;
int greenLED = 12;
int state = 0;
boolean pressed = false;


// initialisation entry point
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(pushButton, INPUT_PULLUP);
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  Serial.begin(115200);
}

// task loop entry point
void loop() {
  int button = digitalRead(pushButton);
  if (button == 0 && !pressed) {
    if (state == 0) {
      state = 1;
      digitalWrite(yellowLED, HIGH);
      digitalWrite(redLED, LOW);
      pressed = true;
     }
    else if (state == 1) {
      state = 2;
      digitalWrite(yellowLED, LOW);
      digitalWrite(greenLED, HIGH);
      pressed = true;
     }
     else if (state == 2)
     {
      state = 0;
      digitalWrite(greenLED, LOW);
      digitalWrite(redLED, HIGH);
      pressed = true;
     };
  }
  if (button == 1) {pressed = false;}
}
