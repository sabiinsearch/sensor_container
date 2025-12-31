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
 #include "HX711.h"
//#include <HX711_ADC.h>

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
// #include "HX711.h"
#include "soc/rtc.h"
#include "esp32-hal-cpu.h"


connectionManager conManagerr;
HX711  scale(SDA, SCL); // HX711 variable


// Create display object with custom I2C
Adafruit_SH1106G screen = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool screen_state = false;

// Create BMP280 object
//Adafruit_BMP280 bmp; // I2C
#define DHT_pin      4
#define DHT_type     DHT11

DHT dht(DHT_pin, DHT_type);   // DHT sensor object

Preferences pref;


   void initLoadCell(appManager* appMgr) {        
        scale.begin(SDA, SCL);
        scale.set_scale(CALIBRATION_FACTOR_LOADCELL);
        scale.tare();
        // Serial.print("Scale Initialized... ");
   }

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
   initLoadCell(me);
   Serial.println("Load Cell Initialized..");

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
        screen.setCursor(10,30);        
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

   screen.display();   // actually display all of the above
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

            screen.setCursor(85,65); 
            // screen.setTextSize(1);
            // screen.setFont(&FreeMonoBold9pt7b);
            // screen.setTextColor(SH110X_BLACK);
            screen.print("*");
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
     // Explicitly free the memory when done
      // free(press_start);
      // free(press_end);
      // free(count_press);

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
        
        screen.setCursor(x,y); 
        screen.setTextSize(textSize);
        screen.setTextColor(textColor);
        screen.print(text);
        screen.println();

}

void connectCloud(appManager* appMgr) {
      connectWiFi(appMgr->conManager);
}


void loop_mgr(appManager* appMgr) {
     loop_con(appMgr->conManager);
}


// float getTemp() {
//     return dht.readTemperature();
// }

// float getHum() {
//     return dht.readHumidity();
// }

// float getPressure() {
//     return bmp.readPressure() / 100.0F;
// }

// float getLoad(appManager* appMgr) {
  
//    float units; 
//      if(appMgr->scale.is_ready()) {
//       units = appMgr->scale.get_units(10);
//      } 
    
//   // if (units < 0)
//   // {
//   //   units = 0.00;
//   // }
// //  float ounces = units * 0.035274;
//   // Serial.print(units);
//   // Serial.print(" grams");
//      return units;
//   //  return LOAD_Demo; // for demo purpose;
    
    
// }


//function to get sensor data and update appManager

void getSensorData_print_update(appManager* appMgr) {
   
  // get data from sensors
   
  float hum = dht.readHumidity();
  delay(10);

  float temp = dht.readTemperature();
  delay(10);
  
  float load; 
  scale.power_up();
  //delay(500);
  initLoadCell(appMgr);
  while(!scale.is_ready()) {

  }
  // if(scale.is_ready()) {  
    load = scale.get_units(10);
    if (load < 0)
    {
      load = 0.00;
    }    
  //  delay(500);
  //  Serial.println(load);
  // }
  
  scale.power_down();
  


  // if(scale.is_ready()) {  
  //   load = scale.get_units();
  //   Serial.println(load);
  // }
  
  // char hum_Buff[6];
  // char temp_Buff[6];
  // char load_Buff[15];
  
  char *hum_Buff = (char*)malloc(100 * sizeof(char));
  char *temp_Buff = (char*)malloc(100 * sizeof(char));
  char *load_Buff = (char*)malloc(100 * sizeof(char));

  int ndigits=5;  


  // print on Serial Monitor

  Serial.print(F("H - "));
  Serial.print(hum);
  Serial.print(F("%  T - "));
  Serial.print(temp);
  Serial.print(F("C  W - "));  
  Serial.print(load);
  Serial.println(F(" g"));

  // print on Screen
    
  // Convert the float to a string, storing it in buf
 
    snprintf(hum_Buff, sizeof(hum_Buff), "%.0f", hum);
    snprintf(temp_Buff, sizeof(temp_Buff), "%.0f", temp);   
    snprintf(load_Buff, sizeof(load_Buff), "%.4f", load);   
     
    memset(&hum, 0, sizeof(hum));
    memset(&temp, 0, sizeof(temp));
    memset(&load, 0, sizeof(load));

    hum = NULL;
    temp = NULL;
    load = NULL;

    // screen.clearDisplay();
    // screen.display();  
    if(!screen_state) {         
       readyScreen();
       screen_state = true;
    }
    
    if(WiFi.status() == WL_CONNECTED) {
      screen.fillRect(20, 105, 70, 10, SH110X_BLACK); // To clear a specific area
    }

    screen.fillRect(65, 5, 15, 30, SH110X_BLACK); // To clear a specific area
    screen.display();
    
//    printOnScreen(65,5,1,1,hum_Buff);
        screen.setCursor(65,5); 
        screen.setTextSize(1);
        screen.setTextColor(1);
        screen.print(hum_Buff);
        screen.println();

//    printOnScreen(95,5,1,1,F("% "));
        screen.setCursor(95,5); 
        screen.setTextSize(1);
        screen.setTextColor(1);
        screen.print(F("% "));
        screen.println();

    // screen.fillRect(65, 20, 13, 13, SH110X_BLACK); // To clear a specific area
    // screen.display();
    
//   printOnScreen(65,20,1,1,temp_Buff); 
        screen.setCursor(65,20); 
        screen.setTextSize(1);
        screen.setTextColor(1);
        screen.print(temp_Buff);
        screen.println();
    
//  printOnScreen(95,20,1,1,F("C"));
        screen.setCursor(95,20); 
        screen.setTextSize(1);
        screen.setTextColor(1);
        screen.print(F("C"));
        screen.println();

     screen.fillRect(65, 35, 20, 10, SH110X_BLACK); // To clear a specific area
    // screen.display();
    
  //  printOnScreen(65,35,1,1,load_Buff); 
        screen.setCursor(65,35); 
        screen.setTextSize(1);
        screen.setTextColor(1);
        screen.print(load_Buff);
        screen.println();

   //printOnScreen(95,35,1,1,F("grams "));
        screen.setCursor(95,35); 
        screen.setTextSize(1);
        screen.setTextColor(1);
        screen.print(F("grams "));
        screen.println();

   //printOnScreen(0,45,1,1,"--------------------");

     screen.display(); // actually display all of the above   

  // Update sensor data in cloud
 
  StaticJsonDocument<100> doc;

  doc["humidity"] = hum_Buff;
  doc["temperature"] = temp_Buff;
  doc["Load"] = load_Buff;

  char jsonBuffer[150];
  serializeJson(doc, jsonBuffer); // print to client
  publishOnMqtt(jsonBuffer, appMgr->conManager);
  // client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
 //appMgr->conManager-> client .publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
        
  // Serial.println(jsonBuffer);

  // Free the allocated memory
  memset(hum_Buff, 0, 100);
  memset(temp_Buff, 0, 100);
  memset(load_Buff, 0, 100);  

  free(hum_Buff);
  free(temp_Buff);
  free(load_Buff);

  hum_Buff = NULL;
  temp_Buff = NULL; 
  load_Buff = NULL;
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

  pinMode(SDA, INPUT_PULLUP);
  pinMode(SCL, INPUT_PULLUP);

  digitalWrite(SDA, HIGH);
  digitalWrite(SCL, HIGH);

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

 



 


 
