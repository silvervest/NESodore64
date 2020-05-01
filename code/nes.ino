// Pins to connect to NES controller
#define NES_CLK   1 // PD2
#define NES_LATCH 2 // PD1
#define NES_DATA  0 // PD0

// bit positions of the data byte read from controller
#define NES_A      0b10000000
#define NES_B      0b01000000
#define NES_SELECT 0b00100000
#define NES_START  0b00010000
#define NES_UP     0b00001000
#define NES_DOWN   0b00000100
#define NES_LEFT   0b00000010
#define NES_RIGHT  0b00000001

// output pins for the joystick port
#define JOY_PIN_UP     8  // PB0
#define JOY_PIN_DOWN   9  // PB1
#define JOY_PIN_LEFT   10 // PB2
#define JOY_PIN_RIGHT  11 // PB3
#define JOY_PIN_FIRE   12 // PB4

// bit positions for the joystick pins
#define JOY_UP    0b00010000
#define JOY_DOWN  0b00001000
#define JOY_LEFT  0b00000100
#define JOY_RIGHT 0b00000010
#define JOY_FIRE  0b00000001

// mode set masks
#define MODE_NORMAL     0b11011111 // SELECT - dpad is normal, B is jump, A is fire
#define MODE_RAPIDFIRE  0b10011111 // SELECT + B - dpad is normal, B is jump, hold A for rapid fire
#define MODE_WADDLE     0b01011111 // SELECT + A - dpad is normal, B is waddle, A is fire - hold right and B to waddle faster

// hold our current controller state
byte controllerState = 255; // initial state of 255 means no buttons pressed
byte lastControllerState = 255; // hold the previous state of the controller also
byte joystickState = 0; // initial state of 0 means no buttons pressed
byte currentMode = MODE_NORMAL; // might as well use the mask to store this too!
bool modeSelect = false;

// some mode specific variables for tracking
byte waddleState = 0; // 0 = left, 1 = right - always start with a left stroke
int waddleCount = 0;
unsigned long lastWaddleTime = 0;
byte fireState = LOW;
unsigned long lastFireTime = 0;

// modes have three speeds, in low medium high speed order
int waddleSpeeds[3] = {20, 50, 100}; // in milliseconds!
int fireSpeeds[3] = {20, 50, 100}; // in milliseconds!

void setup() {
  // set up our clock and latch pins for output, ensuring they begin low
  digitalWrite(NES_CLK, LOW);
  pinMode(NES_CLK, OUTPUT);
  digitalWrite(NES_LATCH, LOW);
  pinMode(NES_LATCH, OUTPUT);

  // our data pin for input
  pinMode(NES_DATA, INPUT);

  // and the joystick ports for output
  digitalWrite(JOY_UP, LOW);
  pinMode(JOY_PIN_UP, OUTPUT);
  digitalWrite(JOY_DOWN, LOW);
  pinMode(JOY_PIN_DOWN, OUTPUT);
  digitalWrite(JOY_LEFT, LOW);
  pinMode(JOY_PIN_LEFT, OUTPUT);
  digitalWrite(JOY_RIGHT, LOW);
  pinMode(JOY_PIN_RIGHT, OUTPUT);
  digitalWrite(JOY_FIRE, LOW);
  pinMode(JOY_PIN_FIRE, OUTPUT);

  // make sure you disable serial communication, as we are re-using the TX and RX pins
  // Serial.begin(115200);
}

void loop() {
  // for timing at the end of this loop
  unsigned long start = micros();

  // get the controller state!
  readControllerState();
  if (lastControllerState != controllerState)
    Serial.println(controllerState, BIN);

  // while SELECT is held, we will move into mode select
  // as soon as SELECT is let go of, the mode will be set
  if (checkButtonState(NES_SELECT)) {
    // make sure we're not outputting anything on the joystick
    joystickState = 0;
    setJoystickPins();
    
    // loop until SELECT is let go of
    Serial.println("entering select mode");
    // starting off, put us into mode select and set the mode to NORMAL
    modeSelect = true;
    currentMode = MODE_NORMAL;
    while (true) {

      // wait a full cycle and read the state again
      delayMicroseconds(16666);
      readControllerState();

      // only set the mode if it's known and we're in mode select
      if (modeSelect == true && controllerState == MODE_RAPIDFIRE || controllerState == MODE_WADDLE) {
        currentMode = controllerState;
        if (lastControllerState != controllerState) {
          Serial.print("setting mode ");
          Serial.println(currentMode, BIN);
        }

        // now that we've picked a mode, lock it in until SELECT is let go of
        modeSelect = false;
      }

      // SELECT was let go of, so return to our regular programming
      if (checkButtonState(NES_SELECT) == false) {
        Serial.println("exiting select mode");
        return; // just exit this loop back to 
      }
    }
  } else { // normal mode, just handle the buttons!
    setJoystickPins();
  }

  // try to loop at 60Hz, every 16666 microsec
  // we can no doubt go much faster than this, but the NES polls normally at 60Hz so let's stick to that
  unsigned long diff = 16666 - (micros() - start);
  if (diff < 16666 && diff > 0)
    delayMicroseconds(diff);
}

byte readControllerState() {
  // pulse the latch to ask the controller to lock in current button state
  digitalWrite(NES_LATCH, HIGH);
  digitalWrite(NES_LATCH, LOW);

  lastControllerState = controllerState;

  // read a byte of data from the controller
  // first read is the first button, ready to roll
  controllerState = digitalRead(NES_DATA);

  // then loop over the remaining 7 bits, shifting the last bit left each time
  for (int i = 1; i <= 7; i ++) {
    digitalWrite(NES_CLK, HIGH);
    controllerState = controllerState << 1;
    controllerState = controllerState + digitalRead(NES_DATA);
    digitalWrite(NES_CLK, LOW);
  }

  // controller unplugged!
  if (controllerState == 0) {
    controllerState = 255;
  }

  // delay a bit for debounce
  delayMicroseconds(5000);
  
  return controllerState;
}

// compare the current controller 
bool checkButtonState(byte button) {
  return (bool)!(button & controllerState);
}

void setJoystickPins() {
  // B button on the NES pad emulates an UP press on the joystick in normal mode
  if (checkButtonState(NES_UP) || (currentMode == MODE_NORMAL && checkButtonState(NES_B))) {
    digitalWrite(JOY_PIN_UP, HIGH);
  } else {
    digitalWrite(JOY_PIN_UP, LOW);
  }

  if (checkButtonState(NES_DOWN)) {
    digitalWrite(JOY_PIN_DOWN, HIGH);
  } else {
    digitalWrite(JOY_PIN_DOWN, LOW);
  }

  if (checkButtonState(NES_LEFT)) {
    digitalWrite(JOY_PIN_LEFT, HIGH);
  } else {
    digitalWrite(JOY_PIN_LEFT, LOW);
  }

  if (checkButtonState(NES_RIGHT)) {
    digitalWrite(JOY_PIN_RIGHT, HIGH);
  } else {
    digitalWrite(JOY_PIN_RIGHT, LOW);
  }

  // B button on the NES pad is also fire in rapid fire mode
  if (checkButtonState(NES_A) || (currentMode == MODE_RAPIDFIRE && checkButtonState(NES_B))) {
    if (currentMode == MODE_RAPIDFIRE) {
      // pick a rapid fire duration based on buttons held
      // A alone is 100ms
      // B alone is 50ms
      // A+B is 20ms
      int delayDuration = fireSpeeds[2];
      if (checkButtonState(NES_A) && checkButtonState(NES_B)) {
        delayDuration = fireSpeeds[0];
      } else if (checkButtonState(NES_B)) {
        delayDuration = fireSpeeds[1];
      }

      // determine whether we should toggle fire based on delay duration
      unsigned long currentTime = millis();
      if ((currentTime - lastFireTime) > delayDuration) {
        lastFireTime = currentTime;

        fireState = !fireState;
      }

      // do it
      digitalWrite(JOY_PIN_FIRE, fireState);
    } else {
      // normal mode, just hold fire high
      digitalWrite(JOY_PIN_FIRE, HIGH);
    }
  } else {
    digitalWrite(JOY_PIN_FIRE, LOW);
    fireState = LOW;
  }

  // in waddle mode, B emulates the back-and-forth left/right directions needed in sports games
  if (currentMode == MODE_WADDLE && checkButtonState(NES_B)) {
    // holding right while holding B will increase the waddle speed, and holding left will waggle slower for some reason
    int delayDuration = waddleSpeeds[1];
    if (checkButtonState(NES_RIGHT)) {
      delayDuration = waddleSpeeds[0];
    } else if (checkButtonState(NES_LEFT)) {
      delayDuration = waddleSpeeds[2];
    }

    // determine whether we should toggle the waddle based on delay duration and our position in the waddle
    unsigned long currentTime = millis();
    if ((currentTime - lastWaddleTime) > delayDuration) {
      lastWaddleTime = currentTime;

      waddleState = !waddleState;
    }

    digitalWrite(JOY_PIN_LEFT, waddleState);
    digitalWrite(JOY_PIN_RIGHT, !waddleState);
  } else {
    waddleState = 0;
  }

}
