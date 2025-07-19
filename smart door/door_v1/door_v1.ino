#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>


////// WIFI //////
// Set the PORT for the web server
ESP8266WebServer server(80);
Servo myservo;

// The WiFi details 
const char* ssid = "TechLabNet";
const char* password =  "BC6V6DE9A8T9";

////// DISTANCE SENSOR //////
// Trigger Pin of Ultrasonic Sensor and  Echo Pin of Ultrasonic Sensor
const int trigPin = D4; 
const int echoPin = D8;

const int ledPin = D0; // LED Pin to indicate the door status

// Duration and distance variables
long duration = 0;
int distance = 0;

////// SERVO MOTOR //////
// Rotation angle var
int angle = 0;
char doorStatus[] = "closed"; // Variable to hold the door status
int minDistance = 20; // Minimum distance to consider the door closed

////// RFID //////
const int RST_PIN = D1; // Reset pin for RFID
const int SS_PIN = D2; // Slave Select pin for RFID

String cardId = ""; // Variable to hold the card ID
bool authenticated = false; // Variable to hold the authentication status

// Create an instance of the MFRC522 class
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create an instance of the MFRC522 class


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

  // Initialize the servo motor
  SPI. begin(); // Initialize SPI bus
  mfrc522.PCD_Init(); // Initialize the MFRC522 RFID reader

  //Delay
  delay(10);
}
// put your main code here, to run repeatedly:
void loop() {

  //Handling of incoming client requests
  server.handleClient(); 

  // Prints the distance on the Serial Monitor
  distanceCentimeter();

  // Control the LED based on the distance
  ledControl();

  openDoor();

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return; // No new card present
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return; // Failed to read card serial
  }
  // Read the card ID
  cardId = getCardId(); // Get the card ID from the RFID reader

  // Authenticate the card
  authentication(cardId); // Call the authentication function with the card ID

  
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
 
}

void get_index() {

  String html ="<!DOCTYPE html> <html> ";
  html += "<head><meta http-equiv=\"refresh\" content=\"10\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>";
  html += "<style> body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f4; } h1 { color: #333; } p { font-size: 18px; } </style>";
  html += "<body> <h1>The Smart Door Dashboard</h1>";
  html +="<p> Welcome to the smart door dashboard </p>";
  html += "<div> <p> <strong> The distance from the door is: ";
  html += distance;
  html +="</strong> cm. </p> </div>";
  html += "<div> <p> <strong> The door is: ";
  html += doorStatus;
  html += "</strong> </p> </div>";
  html += "<div> <p> <strong> The minimum distance to consider the door closed is: ";
  html += minDistance;
  html += "</strong> </p> </div>";
  html += "<form action=\"/changeMinDistance\" method=\"GET\">"; // Form with GET request
  html += "<label for=\"minDistance\">Set Minimum Distance:</label>";
  html += "<input type=\"text\" id=\"minDistance\" name=\"minDistance\" required>"; // Input field with name
  html += "<button type=\"submit\">Submit</button>"; // Submit button
  html += "</form>";
  html += "<div> <p> <strong> The card ID is: ";
  html += cardId;
  html += "</strong> </p> </div>";
  html +="</body> </html>";

  // Add a route to handle the form submission
  server.on("/changeMinDistance", []() {
    if (server.hasArg("minDistance")) {
      int newMinDistance = server.arg("minDistance").toInt(); // Get the input value
      setMinDistance(newMinDistance); // Update the minDistance
      server.send(200, "text/plain", "Minimum distance updated successfully!"); // Response
    } else {
      server.send(400, "text/plain", "Invalid input!"); // Error response
    }
  });


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
    digitalWrite(ledPin, LOW); // Turn on the LED
  } else {
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

void openDoor() {

  //Sets the servo position to 0 angle - door closed
  if (distance < minDistance) {
    angle = 0; // Door closed
    strcpy(doorStatus, "closed"); // Update door status
  } else {
    angle = 180; // Door open
    strcpy(doorStatus, "open"); // Update door status
  }
  myservo.write(angle);
}

void setMinDistance(int newMinDistance) {
  // Function to set a new minimum distance for the door to be considered closed
  minDistance = newMinDistance;
  Serial.print("Minimum distance set to: ");
  Serial.println(minDistance);
}

String getCardId() {
  // Function to read the card ID from the RFID reader
  // Convert the card ID to a string
  String cardId = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    cardId += String(mfrc522.uid.uidByte[i], HEX);
  }
  
  mfrc522.PICC_HaltA(); // Halt the PICC
  mfrc522.PCD_StopCrypto1(); // Stop encryption on the PCD
  
  Serial.print("Card ID: ");
  Serial.println(cardId); // Print the card ID to the serial monitor
  return cardId; // Return the card ID
}

void authentication(String cardId) {
  // Function to handle authentication logic
  // This function can be expanded to include actual authentication logic
  if (cardId == "e3def52") {
    authenticated = true;
    Serial.println("Card authenticated successfully!");
  } else {
    authenticated = false;
    Serial.println("Authentication failed!");
  }
}
