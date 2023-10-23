const int redInpPin = A0;
const int greenInpPin = A1;
const int blueInpPin = A2;

const int redOutPin = 9;
const int greenOutPin = 10;
const int blueOutPin = 11;

const int inpMin = 0;
const int inpMax = 1023;

const int outMin = 0;
const int outMax = 255;


void setup() {
  pinMode(redInpPin, INPUT);
  pinMode(greenInpPin, INPUT);
  pinMode(blueInpPin, INPUT);

  pinMode(redOutPin, OUTPUT);
  pinMode(greenOutPin, OUTPUT);
  pinMode(blueOutPin, OUTPUT);

  Serial.begin(9600);

}

void loop() {
  long redInpRead = analogRead(redInpPin);
  long greenInpRead = analogRead(greenInpPin);
  long blueInpRead = analogRead(blueInpPin);

  long mapedValueRed = map(redInpRead, inpMin, inpMax, outMin, outMax);
  long mapedValueGreen = map(greenInpRead, inpMin, inpMax, outMin, outMax);
  long mapedValueBlue = map(blueInpRead, inpMin, inpMax, outMin, outMax);

  analogWrite(redOutPin, mapedValueRed);
  analogWrite(greenOutPin, mapedValueGreen);
  analogWrite(blueOutPin, mapedValueBlue);
}
