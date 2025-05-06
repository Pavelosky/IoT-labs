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
  pinMode(red_led_pin, OUTPUT);
  pinMode(green_led_pin, OUTPUT);
  pinMode(blue_led_pin, OUTPUT);

  pinMode(touch_sensor, INPUT);

  pinMode(photo_sensor, INPUT);
  Serial.begin(9600);
}

void loop() {
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
