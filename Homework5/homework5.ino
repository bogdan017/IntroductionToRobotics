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

int displayMode = -1;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 200;
byte lastStartPauseState;
byte lastLapModeState;

const unsigned int millisecondsPerSecond = 100;

int currentIndex = displayCount;
int lapDisplayIndex = 0;
int lapMemory[displayCount];

const int lapMemorySize = 4;

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

    handleReset();

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
            if (displayMode == 3 || displayMode == 1 || displayMode == -1) { // idle or pause
                displayMode = 0; // check the display state and if it is idle, pause or the code has not been yet uploaded
                                //make the current display state 0, that is start mode
            } else if (displayMode == 0) { // start
                displayMode = 1; // else if we are already on start mode, we can toggle to pause if the button is pressed again
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
                addLap((currentMillis - startTimer) / millisecondsPerSecond); // if we are on the start mode and couting we can save a lap
            } else if (displayMode == 2) { //and if on lap mode we can cycle through them
                currentIndex = (currentIndex + 1) % lapDisplayIndex;
            }
        }
    }
}


void addLap(int lapNum) {
    if (lapDisplayIndex >= lapMemorySize) {
        for (int i = 1; i < lapMemorySize; ++i) {
            lapMemory[i - 1] = lapMemory[i];
        }
        --lapDisplayIndex; 
    }

    lapMemory[lapDisplayIndex] = lapNum;
    ++lapDisplayIndex;
}//adds a lap time to memory, managing lap history within a set memory size


void startModeLoop() {
    if (!timerRunning) {
        timerRunning = true;
        startTimer = millis();
    }

    if (timerPaused) {
        startTimer += millis() - pauseTimer;
        timerPaused = false;

    }

    writeNumber((millis() - startTimer) / millisecondsPerSecond);
}//controls the timer functionality, starts the timer if not running, resumes from pause, and updates display for a specific time


void lapModeLoop() {
    if (lapDisplayIndex == 0) {
        writeNumber(0); 
    } else {
        if (currentIndex == displayCount) {
            writeNumber(0);
        } else {
            writeNumber(lapMemory[currentIndex]);
        }
    }
}


void pauseModeLoop() {
    if (!timerPaused) {
        pauseTimer = millis();
        timerPaused = true;
    }

    writeNumber((pauseTimer - startTimer) / millisecondsPerSecond);
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
    byte decimalPoint = B00000001; 

    if (hasDecimal) {
        // Add decimal point
        digit |= decimalPoint; 
    } else {
        // make sure there is no decimal point on the current display
        digit &= ~decimalPoint; 
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
        if (currentNumber != 0) {
            lastDigit = currentNumber % 10;
            currentNumber /= 10;
        } else {
            lastDigit = 0;
        }

        if (displayDigit == 2 && !decimalAdded) {
            activateDisplay(displayDigit);
            writeReg(byteEncodings[lastDigit], true);
            writeReg(B00000000, false);
            decimalAdded = true;
        } else if (displayDigit != 2) {
            activateDisplay(displayDigit);
            writeReg(byteEncodings[lastDigit], false);
            writeReg(B00000000, false);
        }
        --displayDigit;
    }
}//displays a numerical value on a 4 digit 7 segment display, supporting decimal points if applicable (in this case just for display number 2)
//else just ignores the decimal points


void handleReset() {
    bool isResetPressed = false;

    if (digitalRead(resetPin) == HIGH) {
        isResetPressed = false;
    }

    if(millis() - lastDebounceTime > debounceDelay && digitalRead(resetPin) == LOW && !isResetPressed) {
        if(displayMode == 1){
            displayMode = 2;
        } else if(displayMode == 2) {
            displayMode = 3;
        }
        lastDebounceTime = millis();
        isResetPressed = true;
    } 
}
//added minor changes and comments
