#include <LiquidCrystal.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <Firebase_ESP_Client.h>


LiquidCrystal lcd(0x27, 16, 2);
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCeI3DVoJPVbu6py7TdGjyxTpr2tmdeNd0"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://c02capturer-default-rtdb.firebaseio.com" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

const long utcOffsetInSeconds = 28800;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
    Serial.begin(9600);
    
    lcd.begin();
    lcd.backlight();

    pinMode(A0,INPUT); 
    pinMode(D0,OUTPUT); 
    pinMode(D1,OUTPUT);    

    WiFiManager wifiManager;
    wifiManager.resetSettings();
    wifiManager.autoConnect("Jollycode Connection");
    Serial.println("connected :)");

   /* Assign the api key (required) */
    config.api_key = API_KEY;

   /* Assign the RTDB URL (required) */
    config.database_url = DATABASE_URL;

   /* Sign up */
    if (Firebase.signUp(&config, &auth, "", "")){
      Serial.println("ok");
      signupOK = true;
    }
    else{
      Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }

   /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
  
    timeClient.begin();
}

int times =0;

void loop() { 
    while(!timeClient.update()) {
      timeClient.forceUpdate();
    }

    String formattedDate = timeClient.getFormattedDate();
    int splitT = formattedDate.indexOf("T");
    String dayStamp = formattedDate.substring(0, splitT);
    
    Serial.println(dayStamp);
    
    // Extract time
    String timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
    Serial.println(timeStamp);
    int count = analogRead(A0); 
      
      Firebase.RTDB.setInt(&fbdo, "currenVal", count)
        if(i==50000){
          Firebase.RTDB.setString(&fbdo,"History",count+","+dayStamp+","+timeStamp);
              i=0;
        }
          i=i+1000;
          
        if(count>200){
          digitalWrite(D0,HIGH);
          digitalWrite(D1,HIGH);
        }else{
          digitalWrite(D0,LOW);
          digitalWrite(D1,LOW);
        }
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("TEAM JOLLYOCDE");
    lcd.setCursor(0,1);
    lcd.println("C02 VAL: "+count)
  
    delay(1000);
}

