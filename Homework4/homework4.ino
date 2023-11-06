/*
  this code is implementing a "moving" led segment between the leds of a 7 segment display
  the movement is done with a joystick along the x and y axes and when the joystick's button is pressed
  it can toggle the state of the current hovered led 
  if the button is pressed for more than 1 second, the display resets
*/

const int pinSwJoystick = 2;
const int pinX = A0;
const int pinY = A1;

const int pinA = 12;
const int pinB = 10;
const int pinC = 9;
const int pinD = 8;
const int pinE = 7;
const int pinF = 6;
const int pinG = 5;
const int pinDP = 4;

const int segSize = 8;
const int noOfMoves = 4;
int startSegment = 7;

bool commonAnode = false;
byte state = LOW;

int xValue = 0;
int yValue = 0;
int joySwState;

bool joyMoved = false;

int minThreshold = 400;
int maxThreshold = 600;

int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};


//values for the moving directions
const int upMove = 0;
const int downMove = 1;
const int leftMove = 2;
const int rightMove = 3;

//the matrix with the led number of every segment's neighbor
byte neighboursMatrix[segSize][noOfMoves] = {
  //    up  down left right
  {0, 6, 5, 1}, // a
  {0, 6, 5, 1}, // b
  {6, 3, 4, 7}, // c
  {6, 3, 4, 2}, // d
  {6, 3, 4, 2}, // e
  {0, 6, 5, 1}, // f
  {0, 3, 6, 6}, // g
  {7, 7, 2, 7}  // dp
};

//state array of every led 
bool segmentState[segSize] = {state}; 
int currentLed = startSegment;

unsigned long lastBlinkTime = 0;
unsigned long blinkTime = 500;
bool blinkState = false;

volatile bool joyState = LOW;
volatile bool joyPressed = false;
volatile bool longJoyPressed = false;
volatile unsigned long joyPressTime = 0;
volatile unsigned long lastJoyReleaseTime = 0;

const unsigned long debounceDuration = 300;
const unsigned long longPressDuration = 1000;

void setup() {
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
    digitalWrite(segments[i], segmentState[i]);
  }

  pinMode(pinSwJoystick, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinSwJoystick), buttonPressInterrupt, CHANGE);

  if (commonAnode == true) {
    state = !state;
  }
}

void loop() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  //if the joystick was moved get the direction where the new segment will be, check if it is not the current led  
  //and set the state according to the state array of every led
  if (joyMoved == false) {
    if (xValue > maxThreshold) {
      // right
      if (neighboursMatrix[currentLed][rightMove] != currentLed) {
        digitalWrite(segments[currentLed], segmentState[currentLed]);
        currentLed = neighboursMatrix[currentLed][rightMove];
        joyMoved = true;
        blinkState = !segmentState[currentLed];
        lastBlinkTime = millis();
      }
    }
    if (xValue < minThreshold) {
      // left
      if (neighboursMatrix[currentLed][leftMove] != currentLed) {
        digitalWrite(segments[currentLed], segmentState[currentLed]);
        currentLed = neighboursMatrix[currentLed][leftMove];
        joyMoved = true;
        blinkState = !segmentState[currentLed];
        lastBlinkTime = millis();
      }
    }
    if (yValue > maxThreshold) {
      // down
      if (neighboursMatrix[currentLed][downMove] != currentLed) {
        digitalWrite(segments[currentLed], segmentState[currentLed]);
        currentLed = neighboursMatrix[currentLed][downMove];
        joyMoved = true;
        blinkState = !segmentState[currentLed];
        lastBlinkTime = millis();
      }
    }
    if (yValue < minThreshold) {
      // up
      if (neighboursMatrix[currentLed][upMove] != currentLed) {
        digitalWrite(segments[currentLed], segmentState[currentLed]);
        currentLed = neighboursMatrix[currentLed][upMove];
        joyMoved = true;
        blinkState = !segmentState[currentLed];
        lastBlinkTime = millis();
      }
    }
  } else if (xValue > minThreshold && xValue < maxThreshold &&
    yValue > minThreshold && yValue < maxThreshold) {
    joyMoved = false;
  }

  //every led will blink when hovered despite of it's current state
  blinkCurrentPosition();

  //if the joystick's button is pressed, toggle the state of the current led
  //long press resets the display
  if (joyPressed) {
    if (joyState == LOW) {
      unsigned long buttonReleaseTime = millis();
      if (buttonReleaseTime - joyPressTime < longPressDuration) {
        segmentState[currentLed] = !segmentState[currentLed];
        digitalWrite(segments[currentLed], segmentState[currentLed]);
      } else {
        longJoyPressed = true;
      }
      joyPressed = false;
    }
  }

  if (longJoyPressed) {
    resetDisplay();
    longJoyPressed = false;
  }
}

void buttonPressInterrupt() {
  //if joystick was pressed more than 1 sec if the button is released the display resets
  //otherwise toggle state and get the time when the button was pressed
  if (digitalRead(pinSwJoystick) == HIGH) {
    joyState = HIGH;
    unsigned long buttonReleaseTime = millis();
    if (buttonReleaseTime - joyPressTime >= longPressDuration && 
        (buttonReleaseTime - lastJoyReleaseTime) >= debounceDuration) {
      longJoyPressed = true;
    }
    lastJoyReleaseTime = buttonReleaseTime;
  } else {
    joyState = LOW;
    joyPressTime = millis();
    joyPressed = true;
  }
}

void blinkCurrentPosition() {
  if (millis() - lastBlinkTime > blinkTime) {
    blinkState = !blinkState;
    lastBlinkTime = millis();
  }
  digitalWrite(segments[currentLed], blinkState);
}

void resetDisplay() {
  for (int i = 0; i < segSize; i++) {
    segmentState[i] = state;
    digitalWrite(segments[i], segmentState[i]);
  }
  currentLed = startSegment;
}
