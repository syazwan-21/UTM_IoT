
#define BLYNK_TEMPLATE_ID "TMPLn3Kr8zf8"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "wbIby4DlxU6MIbWtEs-c8yK0ne4aYsgf"

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include "DHT.h"

DHT dht;

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
BlynkTimer timer;

char auth[] = "wbIby4DlxU6MIbWtEs-c8yK0ne4aYsgf";
char ssid[] = "Lia74@unifi";//Enter your WIFI name
char pass[] = "66554433aja";//Enter your WIFI password

int green = 2; //yellow
int red = 16; //red 
int sensor = 14;

int x = 0;

void setup() {

  lcd.begin();
  lcd.backlight();
  lcd.clear ();
  pinMode(green,OUTPUT);
  pinMode(red,OUTPUT);

  lcd.setCursor (0,1);
  lcd.print ("Offline");
  digitalWrite(red,HIGH);
  digitalWrite(green,LOW);
  
  dht.setup(sensor);
  Serial.begin (9600);

 if (WiFi.status() != WL_CONNECTED)
  {
      WiFi.begin(ssid, pass); // connect to the network
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    delay (200);
    Serial.println(".");       
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    WiFi.mode(WIFI_STA);
    Serial.println(F("Wi-Fi CONNECTED"));
    Serial.println();
    Serial.print(F("IP Address = "));
    Serial.println(WiFi.localIP());
    Serial.println();    
  }
  
  Blynk.config(auth);
  Blynk.connect();

  lcd.setCursor (0,1);
  lcd.print ("Online ");
  digitalWrite(green,HIGH);
  digitalWrite(red,LOW);
}

void loop() {

  int temperature = dht.getTemperature();

  Blynk.virtualWrite(V1, temperature);
  
  Serial.println(temperature);
  lcd.setCursor (0,0);
  lcd.print ("Temperature:");
  lcd.print (temperature);
  lcd.print ("C ");

  if ( temperature >= 30 ){
    Blynk.logEvent ("overheat");
    Serial.print ("Overheat");
  }

  Blynk.run();
  
}
