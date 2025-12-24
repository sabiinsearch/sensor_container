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
Adafruit_SH1106G screen = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Preferences pref;
/* constructor implementation */

void appManager_ctor(appManager * const me) {
 
    // Start I2C on custom pins (for ESP32)
  Wire.begin(SDA, SCL);
  
  initBoard();
  Serial.println("Board Initialized..");
  initScreen();
  Serial.println("Display Initialized.."); 
  // me->scale = setLoadCell(me);
  // Serial.print("Scale set with appMgr.. ");
  // // broadcast_appMgr(me);

  me->conManager = connectionManager_ctor(&conManagerr);
  Serial.println("Connection Manager set with App Manager");

}

/* Function Implementation */
void initScreen() {

 //  Initialize the OLED display
  if (!screen.begin(ADD_OLED, 0x3C)) {
    Serial.println(F("SSD1306 initialization failed"));
    while (true);  // Stop execution if display fails to initialize
  }

  screen.clearDisplay();
  screen.display();
  delay(100);
        screen.setTextSize(2);
        screen.setTextColor(SH110X_WHITE);
        screen.setCursor(20,10);        
        screen.print(F("Hukam.."));
        screen.println();
        screen.display(); // actually display all of the above
//        delay(500);  
        
        // screen.clearDisplay();
        // screen.display();

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
      

}

void connectCloud(appManager* appMgr) {
      connectAWS(appMgr->conManager);
}

void loop_mgr(appManager* appMgr) {
     loop_con(appMgr->conManager);
}


float getTemp() {
    return TEMP_Demo;
}

float getHum() {
    return HUMIDITY_Demo;
}

float getPressure() {
    return PRESSURE_DEMO;
}

int getLoad() {
    return LOAD_Demo;
}


//function to get sensor data and update appManager

void getSensorData_print_update(appManager* appMgr) {
   
  // get data from sensors

  float hum = getHum();
  float temp = getTemp();
  int load = getLoad();

  char hum_Buff[10];
  char temp_Buff[10];
  char load_Buff[10];
  
  int ndigits=6;  


  // print on Serial Monitor

  Serial.print(F("Humidity: "));
  Serial.print(hum);
  Serial.print(F("% Temperature: "));
  Serial.print(temp);
  Serial.print(F("°C "));
  Serial.print(F("\tWeight: "));
  Serial.println(load);

  // print on Screen
    
  // Convert the float to a string, storing it in buf
 
    snprintf(hum_Buff, sizeof(hum_Buff), "%f", hum);
    snprintf(temp_Buff, sizeof(temp_Buff), "%f", temp);   
    snprintf(load_Buff, sizeof(load_Buff), "%f", load);   
    
    screen.clearDisplay();
    screen.display();           

    printOnScreen(20,25,1,1,"Hum  - ");
    printOnScreen(65,25,1,1,hum_Buff);
    

    printOnScreen(20,40,1,1,"Temp - ");
    printOnScreen(65,40,1,1,temp_Buff); 

    
    printOnScreen(7,52,1,1,"Weight - ");
    printOnScreen(65,52,1,1,load_Buff); 
    
    screen.display();    


  // Update sensor data in cloud

  StaticJsonDocument<200> doc;

  doc["humidity"] = hum;
  doc["temperature"] = temp;
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

 



 


 
