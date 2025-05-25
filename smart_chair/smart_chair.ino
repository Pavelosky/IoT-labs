#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Set the PORT for the web server
ESP8266WebServer server(80);

// The WiFi details 
const char* ssid = "TechLabNet";
const char* password =  "BC6V6DE9A8T9";

// For ESP8266 (ESP-E12 module)
const int red_led_pin = D2;    // GPIO4
const int green_led_pin = D3;  // GPIO0
const int blue_led_pin = D4;   // GPIO2

const int photo_sensor = A0;   // Analog input
int photoValue = 0;
int photoTreshold = 550;      

const int touch_sensor = D5;
int chair_busy;


void setup() {

  //Connect to the WiFi network
  WiFi.begin(ssid, password);  
  
  pinMode(red_led_pin, OUTPUT);
  pinMode(green_led_pin, OUTPUT);
  pinMode(blue_led_pin, OUTPUT);

  pinMode(touch_sensor, INPUT);

  pinMode(photo_sensor, INPUT);
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
  server.on("/setLEDStatus", setLEDStatus); // Get the setLed page
  
  server.begin(); //Start the server
  Serial.println("Server listening");
}

void loop() {
  // This will keep the server and serial monitor available 
  Serial.println("Server is running");

  //Handling of incoming client requests
  server.handleClient(); 
 
  
  if(isNight()) {
    police();
  }
  else {
    chairSignal();
  }
}

void rgbLed(int red_led_amount, 
            int green_led_amount, 
            int blue_led_amount) {
  analogWrite(red_led_pin, red_led_amount);
  analogWrite(green_led_pin, green_led_amount);
  analogWrite(blue_led_pin, blue_led_amount);
}

void lightShow() {
  rgbLed(255, 0, 0); // red
  delay(500);
  rgbLed(0, 255, 0); // green
  delay(500);
  rgbLed(0, 0, 255); // blue
  delay(500);
}

void police() {
  rgbLed(255, 0, 0); // red
  delay(200);
  rgbLed(0, 0, 255); // blue
  delay(200);
}

bool isNight() {
  photoValue = analogRead(photo_sensor);
  Serial.println(photoValue);

  if (photoValue < photoTreshold) {
    return false;
  }
  return true;
}

void chairSignal(){
  chair_busy = digitalRead(touch_sensor);
  Serial.println(chair_busy);
  if(chair_busy == HIGH){
    rgbLed(255, 0, 0); // red
  }
  else{
    rgbLed(0, 255, 0); // green
  }
}
  
// Utility function to serve the home page dashboard
void get_index() {

  String html ="<!DOCTYPE html> <html> ";
  html += "<head><meta http-equiv=\"refresh\" content=\"2\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>";
  html += "<body> <h1>The Smart Chair Dashboard</h1>";
  html +="<p> Welcome to the smart chair dashboard</p>";
  html += "<div> <p> <strong> The photoresistor value is: ";
  html += analogRead(photo_sensor);
  html +="</strong> </p>";
  html += "<p> <strong> The chair ";
  html += isNight()?"is in low light conditions":"is not in low light conditions";
  html +="</strong> </p> </div>";
  
  html += "<a href=\"/setLEDStatus?s=0\" target=\"_blank\"\"\"><button>Turn Off </button></a>";
  html += "<a href=\"/setLEDStatus?s=1\" target=\"_blank\"\"\"><button>Turn On </button></a>";
  
  html +="</body> </html>";
  
  //Print a welcoming message on the index page
  server.send(200, "text/html", html);
  
}

// Utility function to read the query string
void setLEDStatus(){
  
  int query_string = 0;
  
  // Check the query string 
  if (server.arg("s") != ""){ //Parameter found
    // Parse the value from the query
    query_string = server.arg("s").toInt();
    // Check the value and update the blue led pin of the RGB component
    if(query_string==1){
     analogWrite(blue_led_pin, 255);
     analogWrite(green_led_pin, 0);
     analogWrite(red_led_pin, 0);
     delay(1000);
    }else{
    analogWrite(blue_led_pin, 0);
    delay(1000);
    }
  }
}
