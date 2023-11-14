const int latchPin = 11;
const int clockPin = 10;
const int dataPin = 12;

const int resetPin = 1;
const int lapPin = 2;
const int startPausePin = 3;

int displayDigits[] = {4, 5, 6, 7};
const int displayCount = 4;

const int encodingsNumber = 10;

byte byteEncodings[encodingsNumber] = {
	//A B C D E F G DP
	B11111100,	// 0
	B01100000,	// 1
	B11011010,	// 2
	B11110010,	// 3
	B01100110,	// 4
	B10110110,	// 5
	B10111110,	// 6
	B11100000,	// 7
	B11111110,	// 8
	B11110110,	// 9
};

unsigned long lastIncrement = 0;
unsigned long delayCount = 50;
unsigned long number = 0;

bool timerRunning = false;
bool timerPaused = false;
unsigned long startTimer = 0;
unsigned long pauseTimer = 0;

int displayMode = 0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 200;
byte lastStartPauseState;
byte lastLapModeState;

const unsigned int millisecondsPerSecond = 100;

int currentIndex = displayCount;
int lapDisplayIndex = 0;
int lapMemory[displayCount];

const int lapMemorySize = 5;

void setup() {
    pinMode(resetPin, INPUT_PULLUP);
    pinMode(lapPin, INPUT_PULLUP);
    pinMode(startPausePin, INPUT_PULLUP);

    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    for (int i = 0; i < displayCount; i++) {
        pinMode(displayDigits[i], OUTPUT);
        digitalWrite(displayDigits[i], LOW);
    }

    attachInterrupt(digitalPinToInterrupt(startPausePin), startPauseButtonPressInterrupt, FALLING);
    attachInterrupt(digitalPinToInterrupt(lapPin), lapButtonPressInterrupt, FALLING);
}

void loop() {
    lastStartPauseState = digitalRead(startPausePin);
    lastLapModeState = digitalRead(lapPin);

    checkResetButton();

    switch(displayMode) {
        case 0: // start mode
            startModeLoop();
            break;
        case 1: // pause mode
            pauseModeLoop();
            break;
        case 2: // lap mode
            lapModeLoop();
            break;
        default: // idle mode
            idleModeLoop();
            break;
    }
}

void startPauseButtonPressInterrupt() {
    unsigned long currentMillis = millis();
    byte currentState = digitalRead(startPausePin);

    if (currentMillis - lastDebounceTime > debounceDelay) {
        if (lastStartPauseState != currentState) {
            lastDebounceTime = currentMillis;
            if (displayMode == 0) {
                displayMode = 1;
            } else if (displayMode != 2) {
                displayMode = 0;
            }
        }
    }
}

void lapButtonPressInterrupt() {
    unsigned long currentMillis = millis();
    byte currentState = digitalRead(lapPin);

    if (currentMillis - lastDebounceTime > debounceDelay) {
        if (lastLapModeState != currentState) {
            lastDebounceTime = currentMillis;
            if (displayMode == 0 && !timerPaused) {
                addLap((currentMillis - startTimer) / millisecondsPerSecond);
            } else if (displayMode == 2) {
                currentIndex = (currentIndex + 1) % lapDisplayIndex;
            }
        }
    }
}


void addLap(int lapNum) {
    if (lapDisplayIndex < lapMemorySize) {
        lapMemory[lapDisplayIndex] = lapNum;
        ++lapDisplayIndex;
    }

}

void startModeLoop() {
    if (timerPaused) {
        startTimer += millis() - pauseTimer;
    }

    if (!timerRunning) {
        startTimer = millis();
    }

    writeNumber((millis() - startTimer) / millisecondsPerSecond);
}

void pauseModeLoop() {
    if (!timerPaused) {
        pauseTimer = millis();
    }

    writeNumber((pauseTimer - startTimer) / millisecondsPerSecond);
}

void lapModeLoop() {
    if (currentIndex == displayCount) {
        writeNumber(0);
    } else {
        writeNumber(lapMemory[currentIndex]);
    }
}

void idleModeLoop() {
    timerPaused = false;
    timerRunning = false;
    currentIndex = displayCount;
    resetLapMemory();
    writeNumber(0);
}

void resetLapMemory() {
    for (int i = 0; i < lapMemorySize; ++i) {
        lapMemory[i] = 0;
    }
    lapDisplayIndex = 0;
}

void activateDisplay(int displayNumber) {
	for (int i = 0; i < displayCount; i++) {
		digitalWrite(displayDigits[i], HIGH);
	}
	digitalWrite(displayDigits[displayNumber], LOW);
}

void writeReg(int digit, bool hasDecimal) {
    const byte decimalPoint = B00000001; 

    if (hasDecimal) {
        // Add decimal point
        digit = digit + decimalPoint; // Set the LSB to 1
    } else {
        // Ensure no decimal point
        digit = digit - (digit % 2); // Set the LSB to 0
    }

    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, digit);
    digitalWrite(latchPin, HIGH);
}

void writeNumber(int number) {
    int currentNumber = number;
    int displayDigit = 3;
    int lastDigit = 0;
    bool decimalAdded = false;

    while (displayDigit >= 0) {
        if (currentNumber != 0 || displayDigit == 1) {
            lastDigit = currentNumber % 10;
            currentNumber /= 10;
        } else {
            lastDigit = 0;
        }

        if (displayDigit == 1 && !decimalAdded) {
            activateDisplay(displayDigit);
            writeReg(byteEncodings[lastDigit], true);
            writeReg(B00000000, true);
            decimalAdded = true;
        } else if (displayDigit != 1) {
            activateDisplay(displayDigit);
            writeReg(byteEncodings[lastDigit], false);
            writeReg(B00000000, false);
        }
        --displayDigit;
    }
}

void checkResetButton() {
    static bool resetButtonPressed = false;
    if(!resetButtonPressed && millis() - lastDebounceTime > debounceDelay && digitalRead(resetPin) == LOW) {
        if(displayMode == 2) {
            displayMode = 3;
        }
        else if(displayMode == 1){
            displayMode = 2;
        }
        
        lastDebounceTime = millis();
        resetButtonPressed = true;
    }
    else if (digitalRead(resetPin) == HIGH) {
        resetButtonPressed = false;
    }
}