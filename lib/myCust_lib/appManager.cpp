#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ArduinoJson.h>

// libraries for Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>
// #include <Fonts/FreeMonoBold9pt7b.h>

//#include <Adafruit_BMP280.h> // Libraries for BMP280
#include <DHT.h>             // Libraries for DHT22     

// Custom Libraries
//#include "app_config.h"
#include "appManager.h"
#include "connectionManager.h"

#include "receiverBoard.h"
#include "sensor.h"
// Libraries for Load Cell
#include <Arduino.h> 
#include "EEPROM.h"
#include "Preferences.h"
#include "HX711.h"
#include "soc/rtc.h"
#include "esp32-hal-cpu.h"


connectionManager conManagerr;

// Create display object with custom I2C
Adafruit_SH1106G screen = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool screen_state = false;

// Create BMP280 object
//Adafruit_BMP280 bmp; // I2C
#define DHT_pin      4
#define DHT_type     DHT11

DHT dht(DHT_pin, DHT_type);   // DHT sensor object

Preferences pref;

/* constructor implementation */

void appManager_ctor(appManager * const me) {
 
    // Start I2C on custom pins (for ESP32)
  Wire.begin(SDA, SCL);

  
  initBoard();
  Serial.println("Board Initialized..");

  initScreen();  
  Serial.println("Display Initialized.."); 

  // initBMP280();
  initDHT();
  Serial.println("Sensor Initialized..");
  // me->scale = setLoadCell(me);
  // Serial.print("Scale set with appMgr.. ");
  // // broadcast_appMgr(me);

  me->conManager = connectionManager_ctor(&conManagerr);
  Serial.println("Connection Manager set with App Manager");

}

/* Function Implementation */

 void displayWelcomeScreen() {
         
        screen.clearDisplay();
        screen.display();
        delay(100);
        screen.setTextSize(2);
        screen.setTextColor(SH110X_WHITE);
        screen.setCursor(10,40);        
        screen.print(F("V E O R A"));
        screen.println();
        screen.display(); // actually display all of the above
        
 }

void readyScreen() {
 
       
    screen.clearDisplay(); 
    screen.display();

   delay(100);
   printOnScreen(20,5,1,1,"HUM  - ");
   printOnScreen(20,20,1,1,"TEMP - ");
   printOnScreen(7,35,1,1,"WEIGHT - ");

   printOnScreen(0,45,1,1,"--------------------");
   printOnScreen(30,55,1,1,"V E O R A ");

   screen.display();   
}

 void checkButtonPressed(appManager* appMgr) {
  
    if((digitalRead(reset_pin))==LOW) {   // check if the button is pressed
        long press_start = millis();
        long press_end = press_start;
        int count_press = 0;

    // count period of button pressed

        while (digitalRead(reset_pin) == LOW) {
          press_end = millis();
          count_press = press_end-press_start;  
          if(count_press>5000) {            
            break;
          }   
        }

            digitalWrite(reset_pin,HIGH); // unpress button        
            Serial.print("Button pressed for: ");
            Serial.println(count_press);

   // Action as per time period of pressing button

    //  if((count_press >0) && (count_press<1500)) {
        
    //     bool flag = true;  //  to check if control goes to On or Off only

    //       if (appMgr->switch_val == 1){
    //         Serial.println("Energy Monitoring Off..");
    //         setSwitchOff(appMgr);
    //         flag = false;
    //         Serial.print("Flag is set to false..");
    //       } 
          
    //       if((appMgr->switch_val == 0) && (flag==true)) {
    //           Serial.println("Energy Monitoring On..");
    //           setSwitchOn(appMgr);
    //         }
    //       delay(100);             
    //       broadcast_appMgr(appMgr);

    //   }
     
        
     if((count_press >10) && (count_press<3500)) {    // reset settings - wipe stored credentials for testing, these are stored by the esp library

            Serial.println("Wifi Resetting.."); 

//            screen.fillRect(27, 47, 81, 16, SH110X_WHITE); // To clear a specific area

            screen.setCursor(45,55); 
            // screen.setTextSize(1);
            // screen.setFont(&FreeMonoBold9pt7b);
            // screen.setTextColor(SH110X_BLACK);
            screen.print("No WiFi");
            screen.println();
            screen.display(); // actually display all of the above
            delay(20);

            screen_state = false;
            digitalWrite(WIFI_LED,HIGH);
            digitalWrite(HEARTBEAT_LED,LOW);
            resetWifi(appMgr->conManager);      
            connectWiFi(appMgr->conManager);

     }

    //  if((count_press >3400) && (count_press<6000)) {

    //           setBoardWithLC(appMgr);
    //  }

   }
    
 }

void initDHT() {
//    DHT dht(DHT_pin, DHT_type);
    dht.begin();
}

// void initBMP280() {
//   if (!bmp.begin(0x76)) {
//     Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
//     while (1);
//   }
// }

void initScreen() {

 //  Initialize the OLED display
  if (!screen.begin(ADD_OLED, 0x3C)) {
    Serial.println(F("SSD1306 initialization failed"));
    while (true);  // Stop execution if display fails to initialize
  }

  displayWelcomeScreen();

}

void printOnScreen(int x, int y, int textSize, int textColor, String text) {
        
        // screen.clearDisplay();
        // screen.display();
        // delay(100);
        screen.setCursor(x,y); 
        screen.setTextSize(textSize);
        screen.setTextColor(textColor);
        screen.print(text);
        screen.println();
        screen.display(); // actually display all of the above
        delay(20);

}

void connectCloud(appManager* appMgr) {
      connectWiFi(appMgr->conManager);
}


void loop_mgr(appManager* appMgr) {
     loop_con(appMgr->conManager);
}


float getTemp() {
    return dht.readTemperature();
}

float getHum() {
    return dht.readHumidity();
}

// float getPressure() {
//     return bmp.readPressure() / 100.0F;
// }

int getLoad() {
    return LOAD_Demo;
}


//function to get sensor data and update appManager

void getSensorData_print_update(appManager* appMgr) {
   
  // get data from sensors

  float hum = getHum();
  float temp = getTemp();
  int load = getLoad();

  char hum_Buff[5];
  char temp_Buff[5];
  char load_Buff[5];
  
  int ndigits=3;  


  // print on Serial Monitor

  Serial.print(F("Humidity: "));
  Serial.print(hum);
  Serial.print(F("% Temperature: "));
  Serial.print(temp);
  Serial.print(F("C "));
  Serial.print(F("\tWeight: "));
  Serial.println(load);

  // print on Screen
    
  // Convert the float to a string, storing it in buf
 
    snprintf(hum_Buff, sizeof(hum_Buff), "%.1f", hum);
    snprintf(temp_Buff, sizeof(temp_Buff), "%.1f", temp);   
    snprintf(load_Buff, sizeof(load_Buff), "%d", load);   
    
    // screen.clearDisplay();
    // screen.display();  
    if(!screen_state) {         
       readyScreen();
       screen_state = true;
    }
    
    if(WiFi.status() == WL_CONNECTED) {
      screen.fillRect(20, 105, 70, 10, SH110X_BLACK); // To clear a specific area
    }

    screen.fillRect(65, 5, 15, 15, SH110X_BLACK); // To clear a specific area
    screen.display();
    // printOnScreen(20,20,1,1,"Hum  - ");
    printOnScreen(65,5,1,1,hum_Buff);
    printOnScreen(95,5,1,1,F("% "));

    screen.fillRect(65, 20, 13, 13, SH110X_BLACK); // To clear a specific area
    screen.display();
    // printOnScreen(20,35,1,1,"Temp - ");
    printOnScreen(65,20,1,1,temp_Buff); 
    printOnScreen(95,20,1,1,F("C"));
    
    // printOnScreen(7,52,1,1,"Weight - ");
    screen.fillRect(65, 35, 15, 15, SH110X_BLACK); // To clear a specific area
    printOnScreen(0,45,1,1,"--------------------");
    screen.display();
    printOnScreen(65,35,1,1,load_Buff); 

    printOnScreen(95,35,1,1,F("grams "));
    screen.display();    


  // Update sensor data in cloud
 
  StaticJsonDocument<200> doc;

  doc["humidity"] = hum_Buff;
  doc["temperature"] = temp_Buff;
  doc["Load"] = load;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  publishOnMqtt(jsonBuffer, appMgr->conManager);
  // client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
 //appMgr->conManager-> client .publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
    delay(1000);
}


void initRGB(){
  pinMode(HEARTBEAT_LED, OUTPUT);
  pinMode(WIFI_LED, OUTPUT);
  pinMode(MQTT_LED, OUTPUT);
  
  digitalWrite(HEARTBEAT_LED,HIGH);
  digitalWrite(WIFI_LED,HIGH);
  digitalWrite(MQTT_LED,HIGH);

  //Serial.println("InitRGB : appManager.cpp");
 }

 void initBoard() {  
  // Configuring Board pins
  pinMode(reset_pin, OUTPUT);
  digitalWrite(reset_pin,HIGH);
  initRGB();

 }
 

// initialize the Scale
    
// HX711 setLoadCell(appManager * appMgr) {
   
//     HX711 scale_local;
    
//     //rtc_clk_cpu_freq_set_config(RTC_CPU_FREQ_80M);   //  RTC_CPU_FREQ_80M
//     setCpuFrequencyMhz(80); 

//     Serial.print("Initializing scale... ");  
//     scale_local.begin(data_pin,clk_pin);
//     scale_local.set_scale(CALIBRATION_FACTOR);
//     Serial.print("Scale Calibrated... ");  

//     if(scale_local.is_ready()) {
//        Serial.print("Scale is ready..");  
//     }
    
//     return scale_local;
//  }

 



 


 
