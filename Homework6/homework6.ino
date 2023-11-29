#include <EEPROM.h>

#define TRIG_PIN 9
#define ECHO_PIN 10
#define LDR_PIN A0
#define RED_PIN 3
#define GREEN_PIN 5
#define BLUE_PIN 6

const int maxReadings = 10;
const int ultrasonicAddress = 0;
const int ldrAddress = 20;

const int inputSubMenuAddress = 65;
const int inputUltrasonic = 75;
const int inputLDR = 85;
const int inputRGB = 100;

int interval;
int ultrasonicThreshold;
int ldrThreshold;

int ultrasonicIndex = 0;
int ultrasonicReadings[maxReadings];
int ldrIndex = 0;
int ldrReadings[maxReadings];

int ultrasonicValue;
int ldrValue;

unsigned long previousMillis = 0;


bool automaticMode = false; // Automatic mode status
bool isAlert = false; // Alert status


void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println(EEPROM.read(95));
  delay(1000);
  displayMainMenu();
}

void displayMainMenu() {
  Serial.println(F("\n*** Main Menu ***"));
  Serial.println(F("1. Sensor Settings"));
  Serial.println("2. Reset Logger Data");
  Serial.println("3. System Status");
  Serial.println("4. RGB LED Control");
  Serial.println("Enter the number corresponding to your choice:");
}

void displaySensorSettingsMenu() {
  Serial.println("\t*** Sensor Settings ***");
  Serial.println("\t1. Set Sampling Interval");
  Serial.println("\t2. Set Ultrasonic Alert Threshold");
  Serial.println("\t3. Set LDR Alert Threshold");
  Serial.println("\t4. Back to Main Menu");
  Serial.println("\tEnter the number corresponding to your choice:");
}

  void displayResetLoggerDataMenu() {
  Serial.println("\n*** Reset Logger Data ***");
  Serial.println("Are you sure you want to delete all data?");
  Serial.println("1. Yes");
  Serial.println("2. No");
  Serial.println("Enter the number corresponding to your choice:");
}

void displaySystemStatusMenu() {
  Serial.println("\n*** System Status ***");
  Serial.println("1. Current Sensor Readings");
  Serial.println("2. Current Sensor Settings");
  Serial.println("3. Display Logged Data");
  Serial.println("4. Back to Main Menu");
  Serial.println("Enter the number corresponding to your choice:");
}

void displayRGBLEDControlMenu() {
  Serial.println("\n*** RGB LED Control ***");
  Serial.println("1. Manual Color Control");
  Serial.println("2. Toggle Automatic Mode");
  Serial.println("3. Back to Main Menu");
  Serial.println("Enter the number corresponding to your choice:");
}

bool isSubMenu = false;
bool isSubMenu1 = false;
int choice;

void loop() {
  addData();
  updateLED();

  if (!isSubMenu1) {
    if (!isSubMenu) {
        handleMainMenuChoice();
      } else {
        handleSubMenuChoice();
      }
  } else {
    switch(choice) { 
      case 1:
        handleSubMenuSensorSettings();
        break;
      case 2:
        handleResetLoggerData();
        break;
      case 3:
        handleSystemStatus();
        break;
      case 4:
        handleRGBLEDControl();
        break;
      default:
        Serial.println("Invalid option. Please select again");
        break;
    }
  }
}
void handleMainMenuChoice() {
  if (Serial.available() > 0) {
    choice = Serial.parseInt();
    switch (choice) {
      case 1:
        isSubMenu = true;
        displaySensorSettingsMenu();
        break;
      case 2:
        isSubMenu = true;
        displayResetLoggerDataMenu();
        break;
      case 3:
        isSubMenu = true;
        displaySystemStatusMenu();
        break;
      case 4:
        isSubMenu = true;
        displayRGBLEDControlMenu();
        break;
      default:
        Serial.println("Invalid Option. Please select again.");
        displayMainMenu();
        break;
    }
  }
}


void handleSubMenuChoice() {
  switch(choice) {
    case 1:
      sensorSettings();
      break;
    case 2:
      resetLoggerData();
      break;
    case 3:
      systemStatus();
      break;
    case 4:
      rgbLEDControl();
      break;
    default:
      Serial.println("Invalid option. Please select again");
      displayMainMenu();
      break;
  }
}

int choiceSettings;
int choiceReset;
int choiceStatus;
int choiceRGB;

void sensorSettings() {
  if(Serial.available() > 0) {
    choiceSettings = Serial.parseInt();
    switch (choiceSettings) {
      case 1:
        Serial.println("Enter sampling interval (1-10 seconds):");
        isSubMenu1 = true;
        break;
      case 2:
        Serial.println("Enter threshold value for the ultrasonic sensor:");
        isSubMenu1 = true;
        break;
      case 3:
        Serial.println("Enter threshold value for the LDR sensor:");
        isSubMenu1 = true;
        break;
      case 4:
        displayMainMenu();
        isSubMenu = false;
        break;
      default:
        Serial.println("Invalid option. Please select again");
        displaySensorSettingsMenu();
        break;
    }
  }
}

void handleSubMenuSensorSettings() {
    switch(choiceSettings) {
      case 1:
        setSamplingInterval();
        break;
      case 2:
        setUltrasonicThreshold();
        break;
      case 3:
        setLDRThreshold();
        break;
      case 4:
        returnToMainMenu();
        break;
      default:
        Serial.println("Invalid option. Please select again");
        displaySensorSettingsMenu();
        break;
  }
}

void setSamplingInterval() {
  if (Serial.available() > 0) {
    interval = Serial.parseInt();
    if (interval >= 1 && interval <= 10) {
      EEPROM.put(inputSubMenuAddress, interval);
      Serial.print("Sampling interval is ");
      Serial.print(EEPROM.get(inputSubMenuAddress,interval));
      returnToMainMenu();
    } else {
      Serial.println("Invalid interval. Please enter a value between 1 and 10.");
    }
  }
}

void setUltrasonicThreshold() {
  if (Serial.available() > 0) {
    ultrasonicThreshold = Serial.parseInt();
    if (ultrasonicThreshold >= 1 && ultrasonicThreshold <= 100) {
      EEPROM.put(inputUltrasonic, ultrasonicThreshold);
      Serial.print("Ultrasonic threshold set to ");
      Serial.print(EEPROM.get(inputUltrasonic, ultrasonicThreshold));
      returnToMainMenu();
    } else {
      Serial.println("Invalid interval. Please enter a value between 1 and 100.");
    }
  } 
}

void setLDRThreshold() {
  if (Serial.available() > 0) {
    ldrThreshold = Serial.parseInt();
    if (ldrThreshold >= 1 && ldrThreshold <= 500) {
      EEPROM.put(inputLDR, ldrThreshold);
      Serial.print("LDR threshold set to ");
      Serial.print(EEPROM.get(inputLDR, ldrThreshold));
      returnToMainMenu();
    } else {
      Serial.println("Invalid interval. Please enter a value between 1 and 500.");
    }
  } 
}

void returnToMainMenu() {
  choiceStatus = 0;
  choiceSettings = 0;
  choiceReset = 0;
  choiceRGB = 0;
  displayMainMenu();
  isSubMenu1 = false;
  isSubMenu = false;

}

void resetLoggerData() {
  if (Serial.available() > 0) {
    choiceReset = Serial.parseInt();
    switch (choiceReset) {
      case 1:
        Serial.println("1. Yes");
        isSubMenu1 = true;
        break;
      case 2:
        Serial.println("2. No");
        isSubMenu1 = false;
        returnToMainMenu();
        break;
      default:
        Serial.println("Invalid option. Please select again");
        displayResetLoggerDataMenu();
        break;
    }
  }
}

void handleResetLoggerData() {
  switch(choiceReset) {
    case 1:
      resetData();
      returnToMainMenu();
      break;
    case 2:
      Serial.println("Data reset canceled");
      returnToMainMenu();
      break;
    default:
      Serial.println("Invalid option. Please select again");
      displayResetLoggerDataMenu();
      break;
  }
}

void clearEEPROMData(int startAddress, int dataSize) {
  for (int i = 0; i < dataSize * sizeof(int); ++i) {
    EEPROM.put(startAddress + i, 0);
  }
}
void resetData() {
  clearEEPROMData(ultrasonicAddress, maxReadings);
  clearEEPROMData(ldrAddress, maxReadings);
}

bool loggedDataDisplayed = false;
bool sensorSettingDisplayed = false;

void systemStatus() {
  if (Serial.available() > 0) {
    choiceStatus = Serial.parseInt();
    switch (choiceStatus) {
      case 1:
        Serial.println("1. Current Sensor Readings");
        isSubMenu1 = true;
        break;
      case 2:
        Serial.println("2. Current Sensor Settings");
        isSubMenu1 = true;
        break;
      case 3:
        Serial.println("3. Display Logged Data");
        isSubMenu1 = true;
        break;
      case 4:
        Serial.println("4. Back to Main Menu");
        displayMainMenu();
        isSubMenu = false;
        break;
      default:
        Serial.println("Invalid option. Please select again");
        displaySystemStatusMenu();
        break;
    }
  }
}

void handleSystemStatus() {
  switch(choiceStatus) {
    case 1:
      readChar();
      break;
    case 2:
      sensorSettingDisplayed = false;
      currentSensorSettings();
      break;
    case 3:
      loggedDataDisplayed = false;
      displayLoggedData();
      break;
    case 4:
      returnToMainMenu();
      break;
    default:
      Serial.println("Invalid option. Please select again");
      displaySystemStatusMenu();
      break;
  }
}



void readChar() {
  if (Serial.available() > 0) {
    char userInput = Serial.read();
    if (userInput == 'x' && isSubMenu1) { // Press 'x' to exit
      isSubMenu1 = false;
      EEPROM.update(95, 1);
      returnToMainMenu();
    }
  }
}
long duration = 0;
int distance = 0;

int readUltrasonicSensor() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);

  distance = duration * 0.034 / 2; 

  return distance;
}

int readLDRSensor() {
  return analogRead(LDR_PIN);
}

bool printReady = true;

void addData() {
   unsigned long currentMillis = millis();
   int samplingInterval = EEPROM.get(inputSubMenuAddress, interval);

 if (currentMillis - previousMillis >= samplingInterval * 1000) {
    previousMillis = currentMillis;

   ultrasonicValue = readUltrasonicSensor(); 
   ultrasonicReadings[ultrasonicIndex] = ultrasonicValue;
   EEPROM.put(ultrasonicAddress + ultrasonicIndex * sizeof(int),ultrasonicValue);
   ultrasonicIndex = (ultrasonicIndex + 1) % maxReadings;

   ldrValue = readLDRSensor(); 
   ldrReadings[ldrIndex] = ldrValue;
   EEPROM.put(ldrAddress + ldrIndex * sizeof(int), ldrValue);
   ldrIndex = (ldrIndex + 1) % maxReadings;

    if (ultrasonicValue < ultrasonicThreshold || ldrValue < ldrThreshold) {
        isAlert = true;
    } else {
        isAlert = false;
    }

   if (choiceStatus == 1) {
      readChar();
      currentSensorReadings();
      printReady = false; 
   }
 }
}


void currentSensorReadings() {
  Serial.println('Press x to exit');
  int stopReadingFlag = EEPROM.read(95);
  if (stopReadingFlag == 1) {
    Serial.println("Sensor readings stopped.");
    EEPROM.put(95, 0);
    displayMainMenu();
    isSubMenu1 = false;
    isSubMenu = false;
    return;
  }

  Serial.println("Sensor Readings:");
  Serial.println("Ultrasonic Readings:"); 
  int ultrasonicReading;
  Serial.println(EEPROM.get(ultrasonicAddress, ultrasonicReading));

  Serial.println("LDR Readings:");
  int ldrReading;
  Serial.println(EEPROM.get(ldrAddress, ldrReading));
  printReady = true;

}

void currentSensorSettings() {
  if (sensorSettingDisplayed) {
    return;
  }

  Serial.print("Sampling interval is: ");
  Serial.print(EEPROM.get(inputSubMenuAddress, interval));
  Serial.println();
  Serial.print("Ultrasonic threshold is: ");
  Serial.print(EEPROM.get(inputUltrasonic, ultrasonicThreshold));
  Serial.println();
  Serial.print("LDR threshold is: ");
  Serial.print(EEPROM.get(inputLDR, ldrThreshold));

  sensorSettingDisplayed = true;
  returnToMainMenu();
  }

  void displayLoggedData() {
  if (loggedDataDisplayed) {
    return;
  }

  Serial.println("Last 10 Sensor Readings:");
  
  Serial.println("Ultrasonic Readings:");
  for (int i = 0; i < maxReadings; ++i) {
    int index = i % maxReadings;
    Serial.print(EEPROM.get(ultrasonicAddress + index * sizeof(int), ultrasonicReadings[index]));
    Serial.print(" ");
  }
  Serial.println();

  Serial.println("LDR Readings:");
  for (int i = 0; i < maxReadings; ++i) {
    int index = i % maxReadings;
    Serial.print(EEPROM.get(ldrAddress + index * sizeof(int), ldrReadings[index]));
    Serial.print(" ");
  }
  Serial.println();
  loggedDataDisplayed = true;
  returnToMainMenu();
}

void rgbLEDControl() {
  if (Serial.available() > 0) {
    choiceRGB = Serial.parseInt();
    switch(choiceRGB) {
      case 1:
        Serial.println("1. Manual Color Control");
        isSubMenu1 = true;
        break;
      case 2:
        Serial.println("2. Toggle Automatic Mode");
        isSubMenu1 = true;
        break;
      case 3:
        Serial.println("3. Back to Main Menu");
        displayMainMenu();
        isSubMenu = false;
        break;
      default: 
        Serial.println("Invalid option. Please select again");
        displayRGBLEDControlMenu();
        break;
    } 
  }
}

void handleRGBLEDControl() {
  switch(choiceRGB) {
    case 1:
      manualColorControl();
      break;
    case 2:
      toggleAutomaticMode();
      break;
    case 3:
      returnToMainMenu();
      break;
    default:
      Serial.println("Invalid option. Please select again");
      displayRGBLEDControlMenu();
      break;
  }
}

int red;
int green;
int blue;
void manualColorControl() {
 if (Serial.available() > 0) {
   red = Serial.parseInt();
   green = Serial.parseInt();
   blue = Serial.parseInt();

   if (red >= 0 && red <= 255 && green >= 0 && green <= 255 && blue >= 0 && blue <= 255) {
     EEPROM.put(inputRGB, red);
     EEPROM.put(inputRGB + 1, green);
     EEPROM.put(inputRGB + 2, blue);
     Serial.print("RGB color set to R:");
     Serial.print(EEPROM.read(inputRGB));
     Serial.print(" G:");
     Serial.print(EEPROM.read(inputRGB + 1));
     Serial.print(" B:");
     Serial.println(EEPROM.read(inputRGB + 2));
     returnToMainMenu();
   } else {
     Serial.println("Invalid RGB values. Please enter values between 0 and 255.");
   }
 }
}

void toggleAutomaticMode() {
 automaticMode = !automaticMode;
 returnToMainMenu();
}

void updateLED() {
 if (automaticMode) {
   if (isAlert) {
     setLEDColor(255, 0, 0); // Red color for alert
   } else {
     setLEDColor(0, 255, 0); // Green color for no alert
   }
 } else {
   int lastRed = EEPROM.read(inputRGB); 
   int lastGreen = EEPROM.read(inputRGB + 1); 
   int lastBlue = EEPROM.read(inputRGB + 2); 

   setLEDColor(lastRed, lastGreen, lastBlue);
 }
}

void setLEDColor(int redValue, int greenValue, int blueValue) {
 analogWrite(RED_PIN, redValue);
 analogWrite(GREEN_PIN, greenValue);
 analogWrite(BLUE_PIN, blueValue);
}
