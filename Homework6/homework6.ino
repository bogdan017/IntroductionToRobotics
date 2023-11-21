#include <EEPROM.h>
#define TRIG_PIN 9
#define ECHO_PIN 10
#define LDR_PIN A0
#define RED_PIN 3
#define GREEN_PIN 4
#define BLUE_PIN 5

enum SensorSettingsMenu {
  NO_SUBMENU_SELECTED,
  SAMPLING_INTERVAL,
  ULTRASONIC_THRESHOLD,
  LDR_THRESHOLD
};

enum ResetLoggerDataMenu {
  NO_SUBMENU_SELECTED,
  RESET_CONFIRMATION,
  CANCEL_RESET
};

enum SystemStatusMenu {
  NO_SUBMENU_SELECTED,
  CURRENT_READINGS,
  DISPLAY_LOGGED_DATA
};

enum RGBLEDControlMenu {
  NO_SUBMENU_SELECTED,
  MANUAL_COLOR_CONTROL,
  TOGGLE_AUTOMATIC_MODE
};

SensorSettingsMenu currentSensorMenu = NO_SUBMENU_SELECTED;
ResetLoggerDataMenu currentResetLoggerMenu = NO_SUBMENU_SELECTED;
SystemStatusMenu currentSystemStatusMenu = NO_SUBMENU_SELECTED;
RGBLEDControlMenu current RGBLEDControlMenu = NO_SUBMENU_SELECTED;

const int maxReadings = 10;
const int ultrasonicAddress = 0;
const int ldrAddress = 10;


bool isInSubmenu = false;
int menuChoice;
int samplingInterval;
int ultrasonicThreshold;
int ldrThreshold;

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  Serial.begin(9600);
  displayMainMenu();
}

void loop() {
  //addSensorData();
  //setLedColor();

  if (Serial.available() > 0) {
    menuChoice = Serial.parseInt();
    switch (menuChoice) {
      case 1:
        sensorSettingsMenu();
        break;
      case 2:
        resetLoggerData();
        break;
      case 3:
        systemStatusMenu();
        break;
      case 4:
        rgbLEDControlMenu();
        break;
      default:
        Serial.println("Invalid Option. Please select again.");
        break;
    }
  }
}

void displayMainMenu() {
  Serial.println("\n*** Main Menu ***");
  Serial.println("1. Sensor Settings");
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

void sensorSettingsMenu() {
  displaySensorSettingsMenu();

  if (Serial.available() > 0) {
    int subMenuOption = Serial.parseInt();

    switch (subMenuOption) {
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
        Serial.println("Invalid Option. Please select again.");
        displaySensorSettingsMenu();
        break;
    }
  }
}

void setSamplingInterval() {
  int isSamplingInInterval = 1;
  Serial.println("Enter sampling interval (1-10 seconds):");
  if(Serial.available() > 0) {
    samplingInterval = Serial.parseInt();
    if (!(samplingInterval >= 0 && samplingInterval <= 10)) {
      isSamplingInInterval = 0;
  }
    switch(isSamplingInInterval) {
      case 1: 
        Serial.println("Your sampling rate is ");
        break;
      case 0:
        Serial.println("Invalid option. Please select again");
        Serial.println("Enter sampling interval (1-10 seconds):");
        break;
    }
  }
}

void setUltrasonicThreshold() {
  Serial.println("Enter Ultrasonic alert threshold:");
  if (Serial.available() > 0) {
    ultrasonicThreshold = Serial.parseInt();
  }
}

void setLDRThreshold() {
  Serial.println("Enter LDR alert threshold:");
  if (Serial.available() > 0) {
    ldrThreshold = Serial.parseInt();
  }
}

void returnToMainMenu() {
  displayMainMenu();
}

void displayResetLoggerDataMenu() {
  Serial.println("\n*** Reset Logger Data ***");
  Serial.println("Are you sure you want to delete all data?");
  Serial.println("1. Yes");
  Serial.println("2. No");
  Serial.println("Enter the number corresponding to your choice:");
}

void resetLoggerData() {
  displayResetLoggerDataMenu();


}

void displaySystemStatusMenu() {
  Serial.println("\n*** System Status ***");
  Serial.println("1. Current Sensor Readings");
  Serial.println("2. Current Sensor Settings");
  Serial.println("3. Display Logged Data");
  Serial.println("4. Back to Main Menu");
  Serial.println("Enter the number corresponding to your choice:");
}

void systemStatusMenu() {
  displaySystemStatusMenu();

  if (Serial.available() > 0) {
    int systemStatusChoice = Serial.parseInt();

    switch (systemStatusChoice) {
      case 1:
        displaySensorReadings();
        break;
      case 2:
        displaySensorSettings();
        break;
      case 3:
        displayLoggedData();
        break;
      case 4:
        returnToMainMenu();
        break;
      default:
        Serial.println("Invalid Option. Please select again.");
        displaySystemStatusMenu();
        break;
    }
  }
}

int readUltrasonicSensor() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  return pulseIn(ECHO_PIN, HIGH);
}

int readLDRSensor() {
  return analogRead(LDR_PIN);
}

void displaySensorReadings() {
  Serial.println("Press 'x' to stop sensors readings");
  unsigned long previousMillis = 0;

  while (true) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= samplingInterval) {
      previousMillis = currentMillis;

      int ultrasonicValue = readUltrasonicSensor();
      int ldrValue = readLDRSensor();

      Serial.print("Ultrasonic: ");
      Serial.println(ultrasonicValue);
      Serial.print("LDR: ");
      Serial.println(ldrValue);
    }

    if (Serial.available()) {
      char userInput = Serial.read();
      if (userInput == 'x') { // 'x' to stop displaying sensor readings
        Serial.println("Sensor readings stopped.");
        break;
      }
    }
  }
}

void displaySensorSettings() {
  Serial.println("Current sensor settings:");
  Serial.println("Sampling interval is");
  Serial.print(samplingInterval);
  Serial.println("Ultrasonic threshold is, ");
  Serial.print(ultrasonicThreshold);
  Serial.println("LDR threshold is ");
  Serial.print(ldrThreshold);
}

void displayLoggedData() {
  Serial.println("Last 10 readings:");

  for (int i = maxReadings - 1; i >= 0; --i) {
    int address = ultrasonicAddress + i; // Starting address for ultrasonic readings
    int ultrasonicValue;
    EEPROM.get(address,ultrasonicValue);
    Serial.print("Ultrasonic ");
    Serial.print(maxReadings - i);
    Serial.print(": ");
    Serial.println(ultrasonicValue);
  }

  for (int i = maxReadings - 1; i >= 0; --i) {
    int address = ldrAddress + i; // Starting address for LDR readings
    int ldrValue;
    EEPROM.get(address,ldrValue);
    Serial.print("LDR ");
    Serial.print(maxReadings - i);
    Serial.print(": ");
    Serial.println(ldrValue);
  }
}

void displayRGBLEDControlMenu() {
  Serial.println("\n*** RGB LED Control ***");
  Serial.println("1. Manual Color Control");
  Serial.println("2. Toggle Automatic Mode");
  Serial.println("3. Back to Main Menu");
  Serial.println("Enter the number corresponding to your choice:");
}

void rgbLEDControlMenu() {
  displayRGBLEDControlMenu();

  if (Serial.available() > 0) {
    int rgbControlChoice = Serial.parseInt();

    switch (rgbControlChoice) {
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
        Serial.println("Invalid Option. Please select again.");
        displayRGBLEDControlMenu();
        break;
    }
  }
}

void manualColorControl() {
  Serial.println("Enter RGB values (e.g., R,G,B):");
}

void setLEDColor(int red, int green, int blue) {
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}

void toggleAutomaticMode() {
  if (ultrasonicValue > ultrasonicThreshold || ldrValue > ldrThreshold) {
    alert = true;
  } else {
    alert = false;
  }

  automaticMode = !automaticMode;
  Serial.print("Automatic mode toggled. ");
  
  if (automaticMode) {
    Serial.println("Automatic mode ON.");
    if (alert) {
      setLEDColor(255, 0, 0); // Red
    } else {
      setLEDColor(0, 255, 0); // Green
    }
  } else {
    Serial.println("Automatic mode OFF.");
    setLEDColor(lastRedValue, lastGreenValue, lastBlueValue);
  }
}
