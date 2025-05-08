
#include "rgb_lcd.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include <DHT11.h>
#include "ThingSpeak.h"

//Wi-Fi Setup 
const char* ssid = "Phil";              
const char* password = "phillip@1";     

String serverName="https://api.thingspeak.com/update?api_key=20DOVLPVBWMG4HSZ";

WiFiClient client;
HTTPClient http;

//ThingSpeak Setup
unsigned long channelID = 2953126;       // ThingSpeak Channel ID
const char* writeAPIKey = "20DOVLPVBWMG4HSZ";  // ThingSpeak Write API Key
const char* readAPIKey = "OPB81U1UQIR0AC52";

//DHT Sensor Setup 
int sensorpin=26;
DHT11 dht11(sensorpin);

float humidity;
float tempC;
float tempF;
int delaytime = 900;

//LCD Setup 
rgb_lcd lcd;
int dT=1500;
int sT=1500;
const int colorR = 85;
const int colorG = 205;
const int colorB = 128;

//LED Pins
int dt = 500;
int safemodeled = 17;        // Green LED
int middleled = 5;          // Yellow LED
int cautionled = 16;         // Red LED

//Buzzer Pin
int Bp = 19;                 // Buzzer pin

void setup() {
  Serial.begin(115200);
  delay(delaytime);

  //This is for the Display
   lcd.begin(16, 2);
   lcd.setRGB(colorR, colorG, colorB); 

  // This is for the LEDs
  pinMode(safemodeled, OUTPUT);
  pinMode(middleled, OUTPUT);
  pinMode(cautionled, OUTPUT);
  pinMode(Bp, OUTPUT);

//connect to wifi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi....");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Attempting to connect to SSID: ");
  }
  Serial.println();
  Serial.print("\nConnected to Wi-Fi, IP Adress: ");
  Serial.print(WiFi.localIP());
  ThingSpeak.begin(client);
}

void loop() {
if(WiFi.status() == WL_CONNECTED){
  humidity = dht11.readHumidity();
  tempC = dht11.readTemperature(); 
  tempF = (tempC * 1.8) + 32;

   // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(tempC) || isnan(tempF)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
}

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" Temperature "); 
  Serial.print(tempC); 
  Serial.print(" C ");
  Serial.print(tempF); 
  Serial.println(" F ");

  //LCD Printing
  lcd.setCursor(0, 0);
  lcd.print(tempC);
  lcd.print(char(0xDF));
  lcd.print("C");

  lcd.setCursor(9, 0);
  lcd.print(tempF);
  lcd.print(char(0xDF));
  lcd.print("F");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.setCursor(10, 1);
  lcd.print(humidity);
  lcd.print("%");

   delay(3000);
   lcd.clear();
  
  
  //Room Condition Messages
  if (tempC >= 30 && tempC <= 35) {
    lcd.setCursor(0, 0);
    lcd.print("Room Temp:Normal");
    lcd.setCursor(0, 1);
    lcd.print("All Good - Safe");
  } else if (tempC <= 29) {
    lcd.setCursor(0, 0);
    lcd.print("Room Temp: Low!");
    lcd.setCursor(0, 1);
    lcd.print("Alert: Too Cold");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Room Temp: High");
    lcd.setCursor(0, 1);
    lcd.print("Alert: Too Hot");
     }
    delay(3000);
    lcd.clear();

  //LEDs and Buzzer
  if (tempC >= 30 && tempC <= 35) {
    digitalWrite(cautionled, LOW);
    digitalWrite(middleled, LOW);
    digitalWrite(safemodeled, HIGH);
    digitalWrite(Bp, LOW);
  } else if (tempC <= 29) {
    digitalWrite(safemodeled, LOW);
    digitalWrite(cautionled, LOW);
    digitalWrite(middleled, HIGH);
    digitalWrite(Bp, LOW);
  } else {
    digitalWrite(safemodeled, LOW);
    digitalWrite(middleled, LOW);
    digitalWrite(cautionled, HIGH);
    digitalWrite(Bp, HIGH);
  }

  //Upload to ThingSpeak
  ThingSpeak.setField(1, tempC);
  ThingSpeak.setField(2, tempF);
  ThingSpeak.setField(3, humidity);

  int response = ThingSpeak.writeFields(channelID, writeAPIKey);
  if (response == 200) {
    Serial.println("Data sent to ThingSpeak successfully.");
  } else {
    Serial.print("Error sending data: ");
    Serial.println(response);
  }

  delay(150); // ThingSpeak requires 15 sec delay between uploads
}

