#define BLYNK_TEMPLATE_ID "TMPL6huAEonWe"
#define BLYNK_TEMPLATE_NAME "Plant watering system"
#define BLYNK_AUTH_TOKEN "6mbA0_MxN4H6Za427p0jt4kHBXM8cefd"
 
#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
 
// Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);
 
char auth[] = "6mbA0_MxN4H6Za427p0jt4kHBXM8cefd"; // Enter your Auth token
char ssid[] = "Redmi Note 9"; // Enter your WIFI name
char pass[] = "12345678"; // Enter your WIFI password
 
BlynkTimer timer;
bool Relay = 0;
bool isAutoMode = true; // Variable to track the mode
 
// Define component pins
#define sensor A0
#define waterPump D3
 
void setup() {
  Serial.begin(115200);
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH);
  lcd.init();
  lcd.backlight();
 
Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
 
  lcd.setCursor(1, 0);
  lcd.print("System Loading");
  for (int a = 0; a <= 15; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();
 
  // Call the function
  timer.setInterval(100L, soilMoistureSensor);
}
 
// Get the button value for manual control
BLYNK_WRITE(V1) {
  if (!isAutoMode) { // Only allow manual control in manual mode
    Relay = param.asInt();
    controlMotor(Relay);
  }
}
 
// Get the mode (Auto/Manual)
BLYNK_WRITE(V2) {
  int mode = param.asInt();
  isAutoMode = (mode == 1); // 1 for Auto, 0 for Manual
  if (isAutoMode) {
    lcd.setCursor(0, 1);
    lcd.print("Mode: Auto   ");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Mode: Manual ");
  }
}
// Control the motor based on the Relay state
void controlMotor(bool state) {
  if (isAutoMode) {
    lcd.setCursor(0, 1);
    lcd.print("Motor is Auto ");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Motor is Manual");
  }
 
  if (state) {
    digitalWrite(waterPump, LOW);
  } else {
    digitalWrite(waterPump, HIGH);
  }
}
 
// Get the soil moisture values
void soilMoistureSensor() {
  int value = analogRead(sensor);
  value = map(value, 650, 1024, 0, 100);
  value = (value - 100) * -1;
 
  Blynk.virtualWrite(V0, value);
  lcd.setCursor(0, 0);
  lcd.print("Moisture :");
  lcd.print(value);
  lcd.print("%");
 
  if (isAutoMode) {
    // Automatically control motor in Auto mode
    if (value < 60) {
      controlMotor(true); // Turn on motor if moisture is below 60%
    } else {
      controlMotor(false); // Turn off motor if moisture is above 60%
    }
  }
}
 
void loop() {
Blynk.run(); // Run the Blynk library
timer.run(); // Run the Blynk timer
}