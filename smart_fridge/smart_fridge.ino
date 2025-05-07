#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


// Set the PORT for the web server
ESP8266WebServer server(80);

// The WiFi details 
const char* ssid = "TechLabNet";
const char* password =  "BC6V6DE9A8T9"; 

// Initialise the switch pin
// The variable to store the switch value
const int switch_pin = D1;
int switch_value;

// Initialise the potentiometer pin
// The value coming from the potentiometer
const int potentiometer_pin  = A0;
int poteValue;

// Initialise the minimum and maximum temperature of the fridge
int minTemp = -2;
int maxTemp = 6;

// Initialise the critical minimum and maximum temperature of the fridge
int criticalMinTemp = 2;
int criticalMaxTemp = 4;

// Initialise the buzzer pin
const int buzzer_pin = D5;

// Initialise the RGB pins
const int red_led_pin = D2;
const int green_led_pin = D3;
const int blue_led_pin = D4;

// put your setup code here, to run once:
void setup() {

  //Connect to the WiFi network
  WiFi.begin(ssid, password);  
  
  // Set the switch and potentiometer pins to INPUT
  // you want to read the state here
  pinMode(switch_pin, INPUT);
  pinMode(potentiometer_pin, INPUT);

  // Set the buzzer to OUTPUT
  // you want to set the state here
  pinMode(buzzer_pin, OUTPUT);
  // No sound initially
  digitalWrite(buzzer_pin, LOW);

   // LEDs as OUTPUT
  pinMode(red_led_pin, OUTPUT);
  pinMode(green_led_pin, OUTPUT);
  pinMode(blue_led_pin, OUTPUT);
  

  // Start the serial to debug the values
  Serial.begin(9600);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {  
      delay(500);
      Serial.println("Waiting to connect...");
  }

  //Print the board IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  

  server.on("/", get_index); // Get the index page on root route 
  server.on("/setBuzzerStatus", setBuzzerStatus); // Get the setBuzzer page
  
  server.begin(); //Start the server
  Serial.println("Server listening");

}

// put your main code here, to run repeatedly:
void loop() {

  // This will keep the server and serial monitor available 
  Serial.println("Server is running");

  //Handling of incoming client requests
  server.handleClient(); 

  // Only execute if the fridge is ON
  if (fridgeOn()){

    // Print the fridge temperature on the serial monitor
    Serial.println(fridgeTemperature());
    
    // Signal temperature status
    temperatureStatus(fridgeTemperature(), fridgeOn());
    
    // Signal critical temperatures
    // trigBuzzer();
    
  }else{
    // Signal temperature status (OFF)
    temperatureStatus(fridgeTemperature(),fridgeOn());
    // No sound, fridge is off
    noTone(buzzer_pin);
  }
}

// Utility function to check whether the fridge is ON
bool fridgeOn(){
  
  // read the switch pin value
  switch_value = digitalRead(switch_pin);
  //Serial.println(switch_value);
  
  // check the status and return either true or false
  if(switch_value == 0){
    return true;
  }
  return false;
  
}

// Utility function to set the fridge temperature
int fridgeTemperature(){
  
  // Read the value of the potentiometer (0--1023)
  poteValue = analogRead(potentiometer_pin); 

  // Map the potentiometer value in a range of minTemp - maxTemp
  poteValue = map(poteValue, 0, 1023, minTemp, maxTemp);

  return poteValue;
}


// Utility function to trigger the buzzer
void trigBuzzer(){
  
  // Check if your fridge temperature is acceptable
    if(fridgeTemperature() < criticalMinTemp || fridgeTemperature() > criticalMaxTemp){
       // Critical temperature, make a sound
       tone(buzzer_pin, 1000);
    }else{
       // Good temperature, no sound
       noTone(buzzer_pin);
    }
    
}


// Utility function to signal the temperature status
void temperatureStatus(int fridgeTemp, bool fridgeOn){
   
   // Turn off the RGB led (inverse mapping)
   if(!fridgeOn){
     analogWrite(red_led_pin, 255);
     analogWrite(green_led_pin, 255);
     analogWrite(blue_led_pin, 255);
     return;
   }

   // Turn on the red value of the RGB led (inverse mapping)
   if (fridgeTemp >= 0){
     int red_value = map(fridgeTemp,0,maxTemp,255,0); // mapping the red value 0 - maxTemp to 255 - 0
     analogWrite(red_led_pin,red_value );
     analogWrite(green_led_pin, 255);
     analogWrite(blue_led_pin, 255);
   }else{
     int blue_value =map(fridgeTemp,minTemp,0,0,255);  // mapping the blue value minTemp - 0 to 0 - 255
     analogWrite(red_led_pin, 255);
     analogWrite(green_led_pin, 255);
     analogWrite(blue_led_pin,blue_value );
   }
    
}


void get_index() {

  String html ="<!DOCTYPE html> <html> ";
  html += "<head><meta http-equiv=\"refresh\" content=\"2\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>";
  html += "<body> <h1>The Smart Fridge Dashboard</h1>";
  html +="<p> Welcome to the smart fridge dashboard</p>";
  html += "<div> <p> <strong> The temperature preference is: ";
  html += fridgeTemperature();
  html +="</strong> degrees. </p>";
  html += "<p> <strong> Buzzer Component ";
  html +="</strong> </p> </div>";
  
  html += "<a href=\"/setBuzzerStatus?s=0\" target=\"_blank\"\"\"><button>Turn Off </button></a>";
  html += "<a href=\"/setBuzzerStatus?s=1\" target=\"_blank\"\"\"><button>Turn On </button></a>";
  
  html +="</body> </html>";
  
  //Print a welcoming message on the index page
  server.send(200, "text/html", html);
  
}

void setBuzzerStatus(){
  
  int query_string = 0;
  
  // Check the query string 
  if (server.arg("s") != ""){ //Parameter found
    // Parse the value from the query
    query_string = server.arg("s").toInt();
    // Check the value and update the blue led pin of the RGB component
    if(query_string==1){
     tone(buzzer_pin, 1000);
    }else{
     noTone(buzzer_pin);
    }
  }
}
