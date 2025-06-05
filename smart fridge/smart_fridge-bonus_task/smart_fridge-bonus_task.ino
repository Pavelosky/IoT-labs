#include <ESP8266WiFi.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#define DHTTYPE DHT11

// Pin definitions
const int temp_hum_pin = D6;        // DHT11 sensor pin
const int buzzer_pin = D5;          // Buzzer pin
const int potentiometer_pin = A0;   // Potentiometer pin

// Temperature thresholds
const int HIGH_TEMP_THRESHOLD = 30;  // Temperature to trigger buzzer
const int LOW_TEMP_THRESHOLD = 29;   // Temperature to reset buzzer state

// Variables
float temperature = 0;
float humidity = 0;
int potValue = 0;
int displayMode = 1;  // 1, 2, or 3 for different LCD messages

// Buzzer control variables
bool buzzerTriggered = false;       // Flag to track if buzzer has been triggered
unsigned long buzzerStartTime = 0;  // Time when buzzer started
const unsigned long BUZZER_DURATION = 3000;  // Buzzer duration in milliseconds (3 seconds)
bool buzzerActive = false;          // Is buzzer currently active

// Initialize components
DHT dht(temp_hum_pin, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Andy's Smart Fridge Water Reminder System");
  Serial.println("=========================================");
  
  // Initialize pins
  pinMode(buzzer_pin, OUTPUT);
  pinMode(potentiometer_pin, INPUT);
  digitalWrite(buzzer_pin, LOW);  // Ensure buzzer is off initially
  
  // Initialize DHT sensor
  dht.begin();
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Display startup message
  lcd.setCursor(0, 0);
  lcd.print("Andy's Smart");
  lcd.setCursor(0, 1);
  lcd.print("Fridge System");
  delay(2000);
  lcd.clear();
  
  Serial.println("System initialized successfully!");
  Serial.println("Monitoring temperature...");
}

void loop() {
  // Read temperature and humidity from DHT11
  readTemperatureHumidity();
  
  // Handle buzzer logic for temperature alerts
  handleTemperatureAlert();
  
  // Read potentiometer and determine display mode
  readPotentiometerMode();
  
  // Update LCD display based on current mode
  updateLCDDisplay();
  
  // Print debug information to serial monitor
  printDebugInfo();
  
  // Small delay to prevent overwhelming the system
  delay(500);
}

void readTemperatureHumidity() {
  // Read values from DHT11 sensor
  float tempReading = dht.readTemperature();
  float humReading = dht.readHumidity();
  
  // Check if readings are valid
  if (isnan(tempReading) || isnan(humReading)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  temperature = tempReading;
  humidity = humReading;
}

void handleTemperatureAlert() {
  unsigned long currentTime = millis();
  
  // Check if temperature exceeds threshold and buzzer hasn't been triggered yet
  if (temperature >= HIGH_TEMP_THRESHOLD && !buzzerTriggered) {
    // Start the buzzer
    tone(buzzer_pin, 1000);  // 1000 Hz tone
    buzzerTriggered = true;
    buzzerActive = true;
    buzzerStartTime = currentTime;
    Serial.println("🚨 ALERT: Temperature exceeded 30°C! Buzzer activated!");
    Serial.println("💧 Reminder: Time to fill up the fridge with water bottles!");
  }
  
  // Turn off buzzer after specified duration
  if (buzzerActive && (currentTime - buzzerStartTime >= BUZZER_DURATION)) {
    noTone(buzzer_pin);
    buzzerActive = false;
    Serial.println("🔇 Buzzer turned off after 3 seconds");
  }
  
  // Reset buzzer trigger when temperature drops below low threshold
  if (temperature < LOW_TEMP_THRESHOLD && buzzerTriggered) {
    buzzerTriggered = false;
    noTone(buzzer_pin);  // Ensure buzzer is off
    buzzerActive = false;
    Serial.println("✅ Temperature dropped below 29°C - Buzzer reset");
  }
}

void readPotentiometerMode() {
  // Read potentiometer value (0-1024)
  potValue = analogRead(potentiometer_pin);
  
  // Map potentiometer value to display modes (1, 2, 3)
  // 0-341: Mode 1, 342-682: Mode 2, 683-1024: Mode 3
  if (potValue <= 341) {
    displayMode = 1;
  } else if (potValue <= 682) {
    displayMode = 2;
  } else {
    displayMode = 3;
  }
}

void updateLCDDisplay() {
  lcd.clear();
  
  switch (displayMode) {
    case 1:
      // Welcome message
      lcd.setCursor(0, 0);
      lcd.print("Smart Fridge");
      lcd.setCursor(0, 1);
      lcd.print("Andy's System");
      break;
      
    case 2:
      // Temperature reading
      lcd.setCursor(0, 0);
      lcd.print("Temperature:");
      lcd.setCursor(0, 1);
      lcd.print(temperature, 1);  // Display with 1 decimal place
      lcd.print(" C");
      break;
      
    case 3:
      // Water reminder message (only when temperature > 30°C)
      lcd.setCursor(0, 0);
      if (temperature >= HIGH_TEMP_THRESHOLD) {
        lcd.print("Time to fill up");
        lcd.setCursor(0, 1);
        lcd.print("the fridge!");
      } else {
        lcd.print("Temperature OK");
        lcd.setCursor(0, 1);
        lcd.print("Stay hydrated!");
      }
      break;
  }
}

void printDebugInfo() {
  Serial.println("--- System Status ---");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  Serial.print("Potentiometer: ");
  Serial.print(potValue);
  Serial.print(" (Mode: ");
  Serial.print(displayMode);
  Serial.println(")");
  Serial.print("Buzzer Status: ");
  if (buzzerActive) {
    Serial.println("ACTIVE");
  } else if (buzzerTriggered) {
    Serial.println("TRIGGERED (Cooling down)");
  } else {
    Serial.println("Ready");
  }
  
  // Display current LCD mode
  Serial.print("LCD Display: ");
  switch (displayMode) {
    case 1:
      Serial.println("Welcome Message");
      break;
    case 2:
      Serial.println("Temperature Reading");
      break;
    case 3:
      Serial.println("Water Reminder");
      break;
  }
  
  Serial.println("====================");
  Serial.println();
}