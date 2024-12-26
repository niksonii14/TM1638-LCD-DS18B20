#include <OneWire.h>
#include <DallasTemperature.h>
#include <TM1638plus.h>
#include <LiquidCrystal.h>

// Define GPIO pins for the DS18B20 sensor
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int numberOfDevices;
DeviceAddress tempDeviceAddress;

// Define GPIO pins for TM1638
#define STB 27
#define CLK 26
#define DIO 25
TM1638plus module(STB, CLK, DIO);

// Define GPIO pins for the LCD
LiquidCrystal lcd(5, 23, 18, 13, 22, 15); // Changed D6 to GPIO 22

uint8_t lastButtons = 0;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize DS18B20
  sensors.begin();
  numberOfDevices = sensors.getDeviceCount();
  Serial.print("Found ");
  Serial.print(numberOfDevices);
  Serial.println(" temperature sensor(s).");

  // Initialize TM1638
  module.displayBegin();
  module.reset();

  // Initialize LCD
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("Temp & TM1638");
  delay(2000); // Allow user to read initial message
  lcd.clear();
}

void loop() {
  // Update temperature readings
  sensors.requestTemperatures();

  // Display temperatures on LCD
  lcd.setCursor(0, 0);
  lcd.print("TEMPERATURE");
  for (int i = 0; i < numberOfDevices; i++) {
    if (sensors.getAddress(tempDeviceAddress, i)) {
      float tempC = sensors.getTempC(tempDeviceAddress);
      lcd.setCursor(0, 1 + i);
      lcd.print("Sensor ");
      lcd.print(i + 1);
      lcd.print(": ");
      lcd.print(tempC, 2);  // Print with 2 decimal places
      lcd.print(" C");
    }
  }

  // Display temperatures on TM1638
  char displayData[9] = "        "; // 8 characters for TM1638
  for (int i = 0; i < numberOfDevices && i < 2; i++) { // Max 2 sensors
    if (sensors.getAddress(tempDeviceAddress, i)) {
      float tempC = sensors.getTempC(tempDeviceAddress);

      // Convert temperature to integer with 2 decimal places
      int tempInt = (int)(tempC * 100); // Scale temperature by 100 for two decimal places
      int tempWhole = tempInt / 100;  // Whole part
      int tempDecimal = tempInt %100;  // Decimal part

      
      char tempStr[6];  
      snprintf(tempStr, sizeof(tempStr), "%02d.%02d", tempWhole, tempDecimal); // Format as "xx.xx"

      
      for (int j = 0; j < 4; j++) {
        displayData[i * 4 + j] = tempStr[j]; // Place 5 characters for each sensor
      }
    }
  }

  // Send formatted data to TM1638
  module.displayText(displayData);

  // Poll buttons from TM1638
  uint8_t buttons = module.readButtons();
  if (buttons != 0 && buttons != lastButtons) {
    // Find button number pressed
    int buttonNumber = -1;
    for (int i = 0; i < 8; i++) {
      if (buttons & (1 << i)) {
        buttonNumber = i + 1; // Buttons are 1-indexed
        break;
      }
    }

    // Blink the corresponding LED on TM1638
    if (buttonNumber != -1) {
      module.setLED(buttonNumber - 1, 1); // Turn on LED
      delay(500);
      module.setLED(buttonNumber - 1, 0); // Turn off LED

      // Update LCD with button information
      lcd.setCursor(0, 3);
      lcd.print("Button Pressed:");
      lcd.print(buttonNumber);

      // Print to Serial Monitor
      Serial.print("Button Pressed: ");
      Serial.println(buttonNumber);
    }

    // Update last button state
    lastButtons = buttons;
  }

  // Reset lastButtons if no buttons are pressed
  if (buttons == 0) {
    lastButtons = 0;
  }

  delay(10); // Delay for LCD update rate
}
