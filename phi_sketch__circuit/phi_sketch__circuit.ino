// Define the number of LEDs
const int led_number = 6;

// Create an array to store the pin numbers for the LEDs
int pinLeds[led_number] = {D2, D4};
int blink_speed = 200;

// Define the pin number for the button
const int pinButton = D1;

// Variable to store the button state
bool flashEnabled = true;

void setup() {
  // Setup function runs once when the program starts

  // Loop through the array to configure each LED pin as an output
  for (int pinLed = 0; pinLed < led_number; pinLed++) {
    pinMode(pinLeds[pinLed], OUTPUT); // Set the pin as output
    digitalWrite(pinLeds[pinLed], LOW); // Initialize the LED to be off
  }

  // Configure the button pin as an input
  pinMode(pinButton, INPUT);
}

void loop() {
  // Check for button press and toggle flashEnabled
  toggleFlashState();

  // If flashing is enabled, flash the LEDs
  if (flashEnabled) {
    for (int pinLed = 0; pinLed < led_number; pinLed++) {
      digitalWrite(pinLeds[pinLed], HIGH); // Turn on the LED
      delay(blink_speed); // default 200ms
      digitalWrite(pinLeds[pinLed], LOW); // Turn off the LED
      // Check for button press and toggle flashEnabled
        toggleFlashState();
    }
  }
}

void toggleFlashState() {
  // Read the button state
  int buttonState = digitalRead(pinButton);

  // If the button is pressed, toggle the flashEnabled state
  if (buttonState == HIGH) {
    if (blink_speed == 100){
      blink_speed = 400;
    }
    else if (blink_speed == 400){
      blink_speed = 200;
    }
    else {
      blink_speed = 100;
    }
  }
}
