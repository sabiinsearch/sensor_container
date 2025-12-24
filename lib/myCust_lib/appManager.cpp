#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ArduinoJson.h>

// libraries for Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>

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
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Preferences pref;
/* constructor implementation */

void appManager_ctor(appManager * const me) {
 
    // Start I2C on custom pins (for ESP32)
  Wire.begin(SDA, SCL);
  
  initBoard();
  Serial.println("Board Initialized..");
  initScreen();

  // me->scale = setLoadCell(me);
  // Serial.print("Scale set with appMgr.. ");
  // // broadcast_appMgr(me);

  me->conManager = connectionManager_ctor(&conManagerr);
  Serial.println("Connection Manager set with App Manager");

}

/* Function Implementation */
void initScreen() {

 //  Initialize the OLED display
  if (!display.begin(ADD_OLED, 0x3C)) {
    Serial.println(F("SSD1306 initialization failed"));
    while (true);  // Stop execution if display fails to initialize
  }
  display.clearDisplay();
  display.display();

}
void connectCloud(appManager* appMgr) {
      connectAWS(appMgr->conManager);
}

void loop_mgr(appManager* appMgr) {
     loop_con(appMgr->conManager);
}


//function to get sensor data and update appManager
void getSensorData_and_update(appManager* appMgr) {
   
  StaticJsonDocument<200> doc;

  doc["humidity"] = getHum();
  doc["temperature"] = getTemp();

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  publishOnMqtt(jsonBuffer, appMgr->conManager);
  // client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
 //appMgr->conManager-> client .publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
    
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
  initRGB();

 }
 
 void publishMessage(appManager * appMgr) {
    

  StaticJsonDocument<200> doc;
  doc["humidity"] = HUMIDITY_Demo;
  doc["temperature"] = TEMP_Demo;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  //  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);

  publishOnMqtt(jsonBuffer, appMgr->conManager);
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

 



 


 
