/*
  IoT BASED TEMPERATURE MONITORING SYSTEM
  CREATED BY: SYAZWAN AZRI & AFRINA SYUHADA
  FOR UTM DIGITAL KUALA LUMPUR
  FOR FULL RESOURCE & DOCUMENTATION PLEASE REFER TO THE GITHUB REPOSITORY
  https://github.com/syazwan-21/UTM_IoT

  Temperature monitoring system using Blynk IoT through static IP configuration
*/

//----------------------------LIBRARY---------------------------------------//
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include "DHT.h"
DHT dht;

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
BlynkTimer timer;

//-----------------------------BLYNK CONFIG-------------------------------//
#define BLYNK_TEMPLATE_ID "TMPL6-typWkmv"
#define BLYNK_TEMPLATE_NAME "Server Room"
#define BLYNK_AUTH_TOKEN "uN4nOM6N2Oc8BRfzDQpsKpOHY1YKMHwT"

//--------------------NETWORK CONFIG--------------------------//
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "UTMWiFi";  //Enter your WIFI ssid
char password[] = "";         //Enter your WIFI password (leave it blank for open network)

IPAddress device_ip  (10, 10, 234, 169);  // Enter network credential for static IP configuration
IPAddress dns        (161, 139,168, 168); //
IPAddress gateway    (10, 10, 232, 250);  //
IPAddress subnet     (255,255,248,0);     //

//---------------------I/O PINS--------------------------//
int green = 13; //components connected to GPIO pin, refer board pinout
int red = 15;
int sensor = 14;

//---------------------Variables-------------------------//
int lastOnline = 0;

int maxTemp = 25; //temperature for warning
int negate = 50; //invalid temperature to negate

int intervalCheck = 600000; //network check interval (value in millis, 1000 = 1s)

//-------------------MAIN PROGRAM STARTS HERE----------------------//
void setup() {

  Serial.begin (9600);
  dht.setup(sensor);
  lcd.begin();
  lcd.backlight();
  pinMode(green,OUTPUT);
  pinMode(red,OUTPUT);

  lcd.setCursor (0,0);
  lcd.print ("Connecting WiFi");
  
  lcd.setCursor (0,1);
  lcd.print ("Offline");
  digitalWrite(red,HIGH);
  digitalWrite(green,LOW);

  connection();

  lcd.setCursor (0,1);
  lcd.print ("Online ");
  digitalWrite(green,HIGH);
  digitalWrite(red,LOW);

  lastOnline = millis();
}

void loop() {

  int temperature = dht.getTemperature();

  if (temperature >= negate){ //check if sensor value have to be negated
    Serial.print (temperature);
    Serial.println (" bounce negate");
    temperature = 0;
  }

  Blynk.virtualWrite(V1, temperature);  //send data to Blynk datastream throgh pin V1
  
  Serial.println(temperature);
  lcd.setCursor (0,0);
  lcd.print ("Temperature:");
  lcd.print (temperature);
  lcd.print ("C ");

  if ( temperature >= maxTemp){  //send warning notification when temperature reaches the maxTemp value
    //Blynk.logEvent ("overheat"); //send notification based on event code in Blynk Dashboard > Events
    Serial.print ("Overheat");
  }

  if (millis () - lastOnline >= intervalCheck ){  
    Serial.println ("network check");
    if (WiFi.status() != WL_CONNECTED){ //check the device's internet connection
      
      lcd.setCursor (0,1);
      lcd.print ("Offline");
      digitalWrite(red,HIGH);
      digitalWrite(green,LOW);
      
      connection ();

      lcd.setCursor (0,1);
      lcd.print ("Online ");
      digitalWrite(green,HIGH);
      digitalWrite(red,LOW);
    }
    lastOnline = millis ();
  }

  Blynk.run();
}

void connection(){ //routine for connecting NodeMCU to the internet and Blynk server
  
  WiFi.config(device_ip, gateway, subnet, dns);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to ");
  Serial.println (ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println ("");
  Serial.print("WiFi Connected! IP address:");
  Serial.println(WiFi.localIP());

  Blynk.config(auth);  
  Blynk.connect();

}
