#include <LiquidCrystal_I2C.h>

#include <NTPClient.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <Firebase_ESP_Client.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);
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
// WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
// Define NTP Client to get time

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");


//Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};


void setup() {
    Serial.begin(9600);
    
    lcd.init();
    lcd.backlight();

    pinMode(A0,INPUT); 
    pinMode(D0,OUTPUT); 
    pinMode(D1,OUTPUT);    
    pinMode(D7,OUTPUT);    

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
  
    // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(28800);
    lcd.setCursor(0,0);
    lcd.print("TEAM JOLLYCODE");
    lcd.setCursor(0,1);
    lcd.println("Loading...");
}

int times =0;
int i =0;

void loop() { 
   
    int count = analogRead(A0); 
 
    timeClient.update();
    lcd.clear();
        lcd.setCursor(0,0);
    lcd.print("TEAM JOLLYCODE");
    lcd.setCursor(0,1);
    lcd.println("C02 VAL:"+String(count));

 if(count>200){
    Serial.print("Low: ");
    Serial.println(count);
    digitalWrite(D0, LOW);
  delay(100);
  }else{
    Serial.print("High: ");
    Serial.println(count);
    digitalWrite(D0, HIGH);
delay(100);
  }
 
  time_t epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);  

  int currentHour = timeClient.getHours();
  Serial.print("Hour: ");
  Serial.println(currentHour);  

  int currentMinute = timeClient.getMinutes();
  Serial.print("Minutes: ");
  Serial.println(currentMinute); 
   
  int currentSecond = timeClient.getSeconds();
  Serial.print("Seconds: ");
  Serial.println(currentSecond);  

  String weekDay = weekDays[timeClient.getDay()];
  Serial.print("Week Day: ");
  Serial.println(weekDay);    

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(monthDay);

  int currentMonth = ptm->tm_mon+1;
  Serial.print("Month: ");
  Serial.println(currentMonth);

  String currentMonthName = months[currentMonth-1];
  Serial.print("Month name: ");
  Serial.println(currentMonthName);

  int currentYear = ptm->tm_year+1900;
  Serial.print("Year: ");
  Serial.println(currentYear);

  //Print complete date:
  String currentDate = String(count) + "-" +String(currentYear) + "/" + String(currentMonth) + "/" + String(monthDay)+"-"+String(currentHour)+":"+String(currentMinute)+":"+String(currentSecond);
  Serial.print("Current date: ");
  String timeStamp = currentDate;


      Firebase.RTDB.setInt(&fbdo, "currenVal", count);
      Serial.println(count);
        if(i==3000){
          Firebase.RTDB.pushString(&fbdo,"History",timeStamp);
              i=0;
        }else{
          i=i+1000;
        }
          
  if(count>200){
    Serial.print("Low: ");
    Serial.println(count);
    digitalWrite(D7, LOW);
  delay(100);
  }else{
    Serial.print("High: ");
    Serial.println(count);
    digitalWrite(D7, HIGH);
delay(100);
  }
  
    delay(1000);
}


