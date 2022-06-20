#include <WiFiNINA.h>
#include "Firebase_Arduino_WiFiNINA.h"

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>


#define trigPinPlyn 2
#define echoPinPlyn 3
#define trigPinRuch 4
#define echoPinRuch 5
#define PWM 6
#define ster1 7
#define ster2 8
#define LED 9
#define przycisk 10

unsigned long StartS=0; //start silnik
unsigned long StartL=0; //start led
unsigned long StartP=0; //start do wysylania procentow
unsigned long CS=1000; //czas podawania plynu
unsigned long CLF=10000; //czas swiecenia na fioletowo
unsigned long CLZ=3000; //czas swiecenia na zielono

// Database Config
#define DATABASE_URL "smartdevice-app-default-rtdb.europe-west1.firebasedatabase.app"
#define DATABASE_SECRET "aQI84AF2i2WaSJIbN1Wrq1Puk2bhfAOSruoqpHFC"

// TO DO
#define WIFI_SSID "Erwin"
#define WIFI_PASSWORD "1122334455"

// Firebase Open
FirebaseData fbdo;

Adafruit_NeoPixel linijka = Adafruit_NeoPixel(8, LED, NEO_GRB + NEO_KHZ800);

LiquidCrystal_I2C lcd(0x27,16,4);

void setup() {
  Serial.begin(9600);
  pinMode(trigPinPlyn, OUTPUT);
  pinMode(echoPinPlyn, INPUT);

  pinMode(trigPinRuch, OUTPUT);
  pinMode(echoPinRuch, INPUT);

  pinMode(PWM, OUTPUT); //Sygnał PWM silnika nr 1
  digitalWrite(PWM, 255); //Ustawiamy (na stałe) stan wysoki na pinie 6

  digitalWrite(ster1, HIGH);
  digitalWrite(ster2, HIGH);
  
  pinMode(ster1, OUTPUT); //Sygnały sterujące kierunkiem obrotów silnika nr 1
  pinMode(ster2, OUTPUT);
  
  linijka.begin();

  color();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Zdezynfekuj");
  lcd.setCursor(0,1);
  lcd.print("rece!");

  pinMode(przycisk, INPUT_PULLUP);

  StartS=millis();
  StartP=millis();


  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED)
  {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(100);
  }
  Serial.println("Connected to WiFi");

  // Autntication Config
  Firebase.begin(DATABASE_URL, DATABASE_SECRET, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);

}


void loop() {
  //timowanie
  if(StartS+CS+3000 <  millis()){
    if(zmierzOdleglosc() < 10 && zmierzOdleglosc() > 2){
      
      StartS=millis();
  
      // Device name
      String path = "/dozownik1";
    
//      // TO DO
//      // Time Stamp Sending
      if (Firebase.pushTimestamp(fbdo, path + "/stamps"))
      { Serial.println("New stamp sent"); }
      else { Serial.println(fbdo.errorReason()); }
  
      
    }
  }
  
    if (digitalRead(przycisk) == 1){
      StartL=millis();
    }

 
  //odpalanie
  if(StartS+CS>millis()){
    digitalWrite(ster1, LOW);
    digitalWrite(ster2, HIGH);
  } else {
    digitalWrite(ster1, HIGH);
    digitalWrite(ster2, HIGH);
  }
  if(digitalRead(przycisk)== 0){
    if(StartL+CLF > millis()){
      purple();
    } else if (StartL+CLF+CLZ > millis()){
      green();
    } else {
      color();
    }
  } else {
    color();
  }

  if (StartP+60000<millis()){

    // Device name
    String path = "/dozownik1";
  
    // TO DO
    // Liquid Level Sending
    int level = zmierzPlyn();
    if (Firebase.setInt(fbdo, path + "/level", level))
    { Serial.println("New level sent"); }
    else { Serial.println(fbdo.errorReason()); }
  
    // Firebase Close
    fbdo.clear();

    StartP=millis();
    
  }

}

void green()
{
   linijka.setPixelColor(0, linijka.Color(0, 255, 0));
    linijka.setPixelColor(1, linijka.Color(0, 255, 0));
    linijka.setPixelColor(2, linijka.Color(0, 255, 0));
    linijka.setPixelColor(3, linijka.Color(0, 255, 0));
    linijka.setPixelColor(4, linijka.Color(0, 255, 0));
    linijka.setPixelColor(5, linijka.Color(0, 255, 0));
    linijka.setPixelColor(6, linijka.Color(0, 255, 0));
    linijka.setPixelColor(7, linijka.Color(0, 255, 0));
    linijka.show();
}

void purple()
{
   linijka.setPixelColor(0, linijka.Color(181, 3, 226));
    linijka.setPixelColor(1, linijka.Color(181, 3, 226));
    linijka.setPixelColor(2, linijka.Color(181, 3, 226));
    linijka.setPixelColor(3, linijka.Color(181, 3, 226));
    linijka.setPixelColor(4, linijka.Color(181, 3, 226));
    linijka.setPixelColor(5, linijka.Color(181, 3, 226));
    linijka.setPixelColor(6, linijka.Color(181, 3, 226));
    linijka.setPixelColor(7, linijka.Color(181, 3, 226));
    linijka.show();
}
void color()
{
   linijka.setPixelColor(0, linijka.Color(12,88, 201));
    linijka.setPixelColor(1, linijka.Color(12, 88, 201));
    linijka.setPixelColor(2, linijka.Color(12, 88, 201));
    linijka.setPixelColor(3, linijka.Color(12, 88, 201));
    linijka.setPixelColor(4, linijka.Color(12, 88, 201));
    linijka.setPixelColor(5, linijka.Color(12, 88, 201));
    linijka.setPixelColor(6, linijka.Color(12, 88, 201));
    linijka.setPixelColor(7, linijka.Color(12, 88, 201));
    linijka.show();
}
void no_color()
{
   linijka.setPixelColor(0, linijka.Color(0, 0, 0));
    linijka.setPixelColor(1, linijka.Color(0, 0, 0));
    linijka.setPixelColor(2, linijka.Color(0, 0, 0));
    linijka.setPixelColor(3, linijka.Color(0, 0, 0));
    linijka.setPixelColor(4, linijka.Color(0, 0, 0));
    linijka.setPixelColor(5, linijka.Color(0, 0, 0));
    linijka.setPixelColor(6, linijka.Color(0, 0, 0));
    linijka.setPixelColor(7, linijka.Color(0, 0, 0));
    linijka.show();
}

int zmierzOdleglosc() {
  long czas, dystans;
 
  digitalWrite(trigPinRuch
  , LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinRuch, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinRuch, LOW);
 
  czas = pulseIn(echoPinRuch, HIGH);
  dystans = czas / 58;
 
  return dystans;
}

int zmierzPlyn(){
  long czas, dystans, procent;
  digitalWrite(trigPinPlyn, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinPlyn, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinPlyn, LOW);
 
  czas = pulseIn(echoPinPlyn, HIGH);
  dystans = czas / 58;

  procent = 100 - (dystans * 50 / 6);
  

  return procent;
}
