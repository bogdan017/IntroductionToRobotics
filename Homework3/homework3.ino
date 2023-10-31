const int floor1Button = 2;
const int floor2Button = 3;
const int floor3Button = 4;

const int floor1Led = 8;
const int floor2Led = 9;
const int floor3Led = 10;
const int elevatorStateLed = 11;

const int buzzer = 5;

unsigned long lastFloorMove = 0;
unsigned long lastMoveTime = 0;
int debounceTime = 50;
int currentFloor = 1;
int desiredFloor = 1;
bool isMoving = false;

void setup() {
  pinMode(floor1Button, INPUT_PULLUP);
  pinMode(floor2Button, INPUT_PULLUP);
  pinMode(floor3Button, INPUT_PULLUP);

  pinMode(elevatorStateLed, OUTPUT);
  pinMode(floor1Led, OUTPUT);
  pinMode(floor2Led, OUTPUT);
  pinMode(floor3Led, OUTPUT);
  
  pinMode(buzzer, OUTPUT);

  digitalWrite(floor1Led, HIGH);
  digitalWrite(elevatorStateLed, HIGH);
}

void loop() {
  if (isMoving) {
    blinkStateLed();
    moveElevator();
  } else {
    digitalWrite(elevatorStateLed, HIGH);
  }
  
  if (millis() - lastFloorMove > debounceTime) {
    if (digitalRead(floor1Button) == LOW && desiredFloor != 1) {
      desiredFloor = 1;
      isMoving = true;
      lastFloorMove = millis();
      arrivalSound();
    }
    if (digitalRead(floor2Button) == LOW && desiredFloor != 2) {
      desiredFloor = 2;
      isMoving = true;
      lastFloorMove = millis();
      arrivalSound();      
    }
    if (digitalRead(floor3Button) == LOW && desiredFloor != 3) {
      desiredFloor = 3;
      isMoving = true;
      lastFloorMove = millis();
      arrivalSound();
    }
  }
}

unsigned long lastBlink = 0;
int timeBlinking = 300;
bool elevatorStateLedStatus = 1;

void blinkStateLed() {
  if (millis() - lastBlink > timeBlinking) {
    lastBlink = millis();
    elevatorStateLedStatus = !elevatorStateLedStatus;
    digitalWrite(elevatorStateLed, elevatorStateLedStatus);
  }
}

unsigned long floorMoveDuration = 3000; 
unsigned long moveStartTime = 0;

void moveElevator() {
  if (!isMoving) {
    return;
  } else {
    if (currentFloor < desiredFloor) {
      if (millis() - moveStartTime > floorMoveDuration) {
        currentFloor++;
        moveStartTime = millis();
        movingSound();
      }
    } else if (currentFloor > desiredFloor) {
      if (millis() - moveStartTime > floorMoveDuration) {
        currentFloor--;
        moveStartTime = millis();
        movingSound();
      }
    }
    
    digitalWrite(floor1Led, currentFloor == 1);
    digitalWrite(floor2Led, currentFloor == 2);
    digitalWrite(floor3Led, currentFloor == 3);
    
    if (currentFloor == desiredFloor) {
      isMoving = false;
      digitalWrite(elevatorStateLed, HIGH);
      closingDoorsSound();
    }
  }
}

void arrivalSound() {
  tone(buzzer, 1500, 500);
  //delay(200);
  //noTone(buzzer);
}

void movingSound() {
  tone(buzzer, 400, 3000);
  //delay(2000);
  //noTone(buzzer);
}

void closingDoorsSound() {
  tone(buzzer, 800, 500);
  //delay(200);
  //noTone(buzzer);
}
