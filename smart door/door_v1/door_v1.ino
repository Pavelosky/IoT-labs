#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <Servo.h>

// Set the PORT for the web server
ESP8266WebServer server(80);
Servo myservo;

// The WiFi details 
const char* ssid = "TechLabNet";
const char* password =  "BC6V6DE9A8T9";


// Trigger Pin of Ultrasonic Sensor and  Echo Pin of Ultrasonic Sensor
const int trigPin = D4; 
const int echoPin = D8;

const int ledPin = D0; // LED Pin to indicate the door status

// Duration and distance variables
long duration = 0;
int distance = 0;

// Rotation angle var
int angle = 0;

// Allocate the JSON document
// Allows to allocated memory for the JSON document
DynamicJsonDocument doc(1024);

// put your setup code here, to run once:
void setup() {

  //Connect to the WiFi network
  WiFi.begin(ssid, password);
  
  
  // Sonic Sensor Pin Setup
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  Serial.begin(9600); // Starts the serial communication

  // Starting server for the dashboard site
  while (WiFi.status() != WL_CONNECTED) {  
      delay(500);
      Serial.println("Waiting to connect...");  // Wait for connection
  }
  Serial.print("IP address: "); //Print the board IP address
  Serial.println(WiFi.localIP());  
  server.on("/", get_index); // Get the index page on root route 
  server.on("/json", get_json); // Get the JSON data on /json route
  server.begin(); //Start the server
  Serial.println("Server listening");
  
  // Led pin setup
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, LOW);

  myservo.attach(D3);  
}
// put your main code here, to run repeatedly:
void loop() {

  // This will keep the server and serial monitor available 
  Serial.println("Server is running");

  //Handling of incoming client requests
  server.handleClient(); 

  // Prints the distance on the Serial Monitor
  distanceCentimeter();

  // Control the LED based on the distance
  ledControl();

  servoMovement(angle);
  
}

// Calculates the distance in cm
void distanceCentimeter() {
  
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  // Clears the trigPin
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance in cm
  distance = (duration * 0.034)/2;

  angle = map(distance,0,100,0,180);

  // Prints distance to Serial Monitor
  Serial.print(distance);
  Serial.println(": Centimeters");
 
}

void get_index() {

  String html ="<!DOCTYPE html> <html> ";
  html += "<head><meta http-equiv=\"refresh\" content=\"2\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>";
  html += "<body> <h1>The Smart Door Dashboard</h1>";
  html +="<p> Welcome to the smart door dashboard </p>";
  html += "<div> <p> <strong> The distance from the door is: ";
  html += distance;
  html +="</strong> cm. </p> </div>";
  html +="</body> </html>";
  
  //Print a welcoming message on the index page
  server.send(200, "text/html", html);
  
}

void ledControl(){
  
  int mappedValue;
  // Map the distance to a value between 0 and 255
  // mappedValue = map(distance, 19, 20, 0, 255);
  // analogWrite(ledPin, mappedValue); // Write the mapped value to the LED pin

  // If the distance is less than 20 cm, turn on led and print messaage
  if (distance < 20) {
    Serial.println("Door is closed");
    digitalWrite(ledPin, LOW); // Turn on the LED
  } else {
    Serial.println("Door is open");
    digitalWrite(ledPin, HIGH); // Turn off the LED
  }
}

void jsonDistanceSensor() {
  
  //Add JSON request data 
  doc["Content-Type"] = "application/json";
  doc["Status"] = 200;

  // Add distance data to the JSON document
  JsonObject distanceSensor = doc.createNestedObject("Sensor");
  distanceSensor["sensorName"] = "Distance Sensor";

  // Add the distance value to the JSON document
  JsonArray pins = distanceSensor.createNestedArray("sensorPins");
  pins.add(trigPin);
  pins.add(echoPin);
  distanceSensor["sensorValue"] = distance;
}

void get_json() {
  
  // Call the function to get the distance sensor data
  jsonDistanceSensor();

  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(doc, jsonString);

  // Send the JSON response
  server.send(200, "application/json", jsonString);
  
  // Clear the JSON document for next request
  // doc.clear();
}

void servoMovement(int angle){

  //Sets the servo position to 0 angle - door closed
  myservo.write(angle);
  

}
