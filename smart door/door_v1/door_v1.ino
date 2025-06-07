#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Set the PORT for the web server
ESP8266WebServer server(80);

// The WiFi details 
const char* ssid = "TechLabNet";
const char* password =  "BC6V6DE9A8T9";


// Trigger Pin of Ultrasonic Sensor and  Echo Pin of Ultrasonic Sensor
const int trigPin = D4; 
const int echoPin = D8;

const int ledPin = D0; // LED Pin to indicate the door status

// Duration and distance variables
long duration = 0;
int distance= 0;

// put your setup code here, to run once:
void setup() {

  //Connect to the WiFi network
  WiFi.begin(ssid, password);  
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  Serial.begin(9600); // Starts the serial communication

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {  
      delay(500);
      Serial.println("Waiting to connect...");
  }

  //Print the board IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  

  server.on("/", get_index); // Get the index page on root route 

  server.begin(); //Start the server
  Serial.println("Server listening");
  
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, LOW);
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
