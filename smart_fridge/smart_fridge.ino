
// Initialise the switch pin
// The variable to store the switch value
const int switch_pin = D1;
int switch_value;

// Initialise the potentiometer pin
// The value coming from the potentiometer
const int potentiometer_pin  = A0;
int poteValue;

const int led_pin = D2;

// Initialise the minimum and maximum temperature of the fridge
int minTemp = -2;
int maxTemp = 6;


// put your setup code here, to run once:
void setup() {
  
  // Set the switch and potentiometer pins to INPUT
  // you want to read the state here
  pinMode(switch_pin, INPUT);
  pinMode(potentiometer_pin, INPUT);
  pinMode(led_pin, OUTPUT);

  // Start the serial to debug the values
  Serial.begin(9600);

}

// put your main code here, to run repeatedly:
void loop() {

  // Only execute if the fridge is ON
  if (fridgeOn()){
      // Print the fridge temperature on the serial monitor
      Serial.println(fridgeTemperature());
  }
  else{
    poteValue = analogRead(potentiometer_pin);
    int brightness_value = map(poteValue, 0, 1023, 0, 255);
    analogWrite(led_pin, brightness_value);
  }
    
}

// Utility function to check whether the fridge is ON
bool fridgeOn(){
  
  // read the switch pin value
  switch_value = digitalRead(switch_pin);
  
  // check the status and return either true or false
  if(switch_value == 0){
    Serial.println(0);
    return false;
  }
  Serial.println(1);
  return true;
  
}

// Utility function to set the fridge temperature
int fridgeTemperature(){
  
  // Read the value of the potentiometer (0--1023)
  poteValue = analogRead(potentiometer_pin); 

  // Map the potentiometer value in a range of minTemp - maxTemp
  poteValue = map(poteValue, 0, 1023, minTemp, maxTemp);

  return poteValue;
}
