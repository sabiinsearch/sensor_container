#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ArduinoJson.h>
#include <time.h>

// libraries for Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>

// Library for Load Sensor
 #include "HX711.h"
 #include "Preferences.h"

 // Library for Gyro Sensor
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

//#include <Adafruit_BMP280.h> // Libraries for BMP280
#include <DHT.h>             // Libraries for DHT22     

// Custom Libraries
//#include "app_config.h"
#include "appManager.h"
#include "connectionManager.h"

#include "receiverBoard.h"
// #include "sensor.h"
// Libraries for Load Cell
#include <Arduino.h> 
#include "EEPROM.h"
#include "Preferences.h"
// #include "HX711.h"
#include "soc/rtc.h"
#include "esp32-hal-cpu.h"


connectionManager conManagerr;

Preferences pref;

// Create Load Cell 
HX711  scale(SDA, SCL); 

// Create display object with custom I2C
Adafruit_SH1106G screen = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Create Gyro Sensor object
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;  // to store movemets

bool screen_state = false;

bool displayOn = false;

long displayOn_start;
//float x_now,y_now,x_pre,y_prev;
float x_start;
float y_start;

bool updateNeeded = false;

int counter;

// Create BMP280 object
//Adafruit_BMP280 bmp; // I2C
#define DHT_pin      4
#define DHT_type     DHT11

DHT dht(DHT_pin, DHT_type);   // DHT sensor object

   
  void initGyroSensor(appManager* appMgr) {


  if (!mpu.begin()) {
      Serial.println("Failed to find MPU6050 chip");
        while (1) {
          delay(10);
        }
  }

  for(counter = 0;counter<45;counter++)
  {  
    mpu.getEvent(&a, &g, &temp);
    delay(20);  
  }
       
  // Storing x and y at start     
  x_start = a.acceleration.x/0.10;
  y_start = a.acceleration.y/0.10;
  

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }


  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  }

   void initLoadCell(appManager* appMgr) {   

        // Initiate preferences   
      pref.begin("app_conf",false);

    // get threshold from cloud
     if((pref.getFloat("threshold",0))==0) {      
        appMgr->load_threshold = 0.0;
     }
      else {
        appMgr->load_threshold = pref.getFloat("threshold");
      }

      Serial.print("Preferences Threshold: ");
      Serial.println(appMgr->load_threshold);    
        scale.begin(SDA, SCL);
        scale.set_scale(CALIBRATION_FACTOR_LOADCELL);
        scale.tare();

        appMgr->scale = scale; // set the scale object in appManager
       
        pref.end();
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

   initGyroSensor(me);
  
   Serial.println("Gyro Sensor Initialized..");


   me->conManager = connectionManager_ctor(&conManagerr);
  Serial.println("Connection Manager set with App Manager");
  
  

  // clear Screen after Welcome

       delay(1000);
       screen.clearDisplay();
       screen.display();

       me->prev_hum = -0.0;
       me->prev_temp = -0.0;
       me->prev_load = -0.0;

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
        delay(1000);

        // screen.clearDisplay();
        // screen.display();
        
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
  
    if((digitalRead(LOAD_CELL_RESET_PIN))==LOW) {   // check if the button is pressed
        long press_start = millis();
        long press_end = press_start;
        int count_press = 0;

    // count period of button pressed

        while (digitalRead(LOAD_CELL_RESET_PIN) == LOW) {
          press_end = millis();
          count_press = press_end-press_start;  
          if(count_press>4000) {            
            break;
          }   
        }

            digitalWrite(LOAD_CELL_RESET_PIN,HIGH); // unpress button  
            Serial.print("Button pressed for: ");
            Serial.println(count_press);            



   // Action as per time period of pressing button

     if((count_press >100) && (count_press<10000)) {
        
        Serial.println("Resetting Load Cell..");
        reset_Load_Cell(appMgr);

       }
     
        
//      if((count_press >2000) && (count_press<4000)) {
//           // reset settings - wipe stored credentials for testing, these are stored by the esp library
            
//             Serial.println("Wifi Resetting.."); 

// //            screen.fillRect(27, 47, 81, 16, SH110X_WHITE); // To clear a specific area
//             screen.setCursor(100,48); 
//             screen.setTextSize(2);
//             // screen.setFont(&FreeMonoBold9pt7b);
//             screen.setTextColor(SH110X_WHITE);
//             screen.print(".");
//             screen.println();
//             screen.display(); // actually display all of the above
//             delay(20);
            
//             screen_state = false;
//             digitalWrite(WIFI_LED,HIGH);
//             digitalWrite(HEARTBEAT_LED,LOW);
            
//             resetWifi(appMgr->conManager);      
//             connectWiFi(appMgr->conManager);            

//      }

   // if((count_press >10) && (count_press<4000)) {} // For another operation

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
    while (true) { delay(100); }  // Stop execution if display fails to initialize, add delay for WDT
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

// function to initialize to reset load cell and set it in appManager


void reset_Load_Cell(appManager* appMgr) {

  HX711 scale_local;
    
    //rtc_clk_cpu_freq_set_config(RTC_CPU_FREQ_80M);   //  RTC_CPU_FREQ_80M
    setCpuFrequencyMhz(80); 

    Serial.print("Initializing scale... ");  
    scale_local.begin(SDA, SCL);
    scale_local.set_scale(CALIBRATION_FACTOR_LOADCELL);
    Serial.print("Scale Calibrated... ");  

    if(scale_local.is_ready()) {
       Serial.print("Scale is ready..");  
    }
    
  appMgr->scale = scale_local; // set the scale object in appManager

  pref.begin("app_conf");

  Serial.println("Sync container with LC.");

  float reading;

  long loop_start = millis();
    while(!appMgr->scale.is_ready()) {
      delay(10); 
      if (millis() - loop_start > 2000) break; // 2s timeout
  }

  if(appMgr->scale.is_ready()) { 

   reading = (float)(scale_local.get_units(10));
  }

  if(reading<0) {
     reading = reading * (-1);
  }

  
  pref.putFloat("threshold",reading);
  appMgr->load_threshold = reading;
  Serial.print("Threshold set in Preferences and appManager as per Load Cell..");
  //Serial.println(appMgr->load_threshold);
  Serial.println(reading);
  pref.end();
}

//function to get sensor data and update appManager

void getSensorData_print_update(appManager* appMgr) {
   
  // get data from sensors

  /* Get new sensor events with the readings */
  for(counter = 0;counter<45;counter++)
  {  
    mpu.getEvent(&a, &g, &temp);
    delay(5);  
  }
  delay(10);
  
  // Use stack variables instead of malloc
  float x_now = (float)(a.acceleration.x/.10);
  float y_now = (float)(a.acceleration.y/.10);

  float hum = dht.readHumidity();
  delay(10);

  float temperature = dht.readTemperature();
  delay(10);
  
  float load = 0.00; 

  appMgr->scale.power_up();

  initLoadCell(appMgr);
  
  // Add timeout or delay to prevent infinite loop WDT reset
  long loop_start = millis();
  while(!scale.is_ready()) {
      delay(10); 
      if (millis() - loop_start > 2000) break; // 2s timeout
  }
  
  
  if(appMgr->scale.is_ready()) {  
    load = (float)(appMgr->scale.get_units(10));
    if (load < 0)
    {
      load = load + appMgr->load_threshold; // Adjust load with threshold
    }    
    else {
      load = load - appMgr->load_threshold; // Adjust load with threshold
    }
  }

  appMgr->scale.power_down();
  
  // check if all sensor data is changed
  if (((hum - (appMgr->prev_hum)) > 0.01) || (((appMgr->prev_hum) - hum) > 0.01)) {
          appMgr->prev_hum = hum;
          updateNeeded = true;
      }
  if (((temperature - (appMgr->prev_temp)) > 0.01) || (((appMgr->prev_temp) - temperature) > 0.01)) {
    appMgr->prev_temp = temperature;
    updateNeeded = true;
  }
  if (((load - (appMgr->prev_load)) > 0.01) || (((appMgr->prev_load) - load) > 0.01)) {
    appMgr->prev_load = load;
    updateNeeded = true;
  }

 
  char hum_Buff[20];
  char temp_Buff[20];
  char load_Buff[20];

  int ndigits=5;  

  // print on OLED
    
  // Convert the float to a string, storing it in buf
 
    snprintf(hum_Buff, sizeof(hum_Buff), "%.0f", hum);
    snprintf(temp_Buff, sizeof(temp_Buff), "%.0f", temperature);   
    snprintf(load_Buff, sizeof(load_Buff), "%.1f", load);   
      

if(((x_now) > x_start + 8) || ((x_now)<x_start-8) || ((y_now)>y_start+8) || ((y_now)<y_start-8)) {
   displayOn = true; 
   displayOn_start = millis();
   
   x_start = x_now;
   y_start = y_now;
}

if(displayOn) {  
 
   if((WiFi.status() != WL_CONNECTED)) {   // Print Not connected symbol "." 
   
            screen.setCursor(100,48); 
            screen.setTextSize(2);
            // screen.setFont(&FreeMonoBold9pt7b);
            screen.setTextColor(SH110X_WHITE);
            screen.print(".");
            screen.println();
            screen.display(); // actually display all of the above
            delay(10);

   }  

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

    //  delay(DISPLAY_TIME);  
    if((millis()-displayOn_start) > DISPLAY_TIME) {
     
     displayOn_start = 0; 
     
     displayOn = false;     // set Display off     

     screen.clearDisplay();  // Clear Display
     screen.display();
     screen_state = false;
    }

  }

  // Update sensor data in cloud if there is any change in sensor data

  if (updateNeeded) {

      // Increase size for time string
      StaticJsonDocument<512> doc;  
      
      doc["UID"] = UNIQUE_ID;
      
      // Get current time
      struct tm timeinfo;
      if(getLocalTime(&timeinfo)){
        char timeStringBuff[50];
        strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", &timeinfo);
        doc["time"] = timeStringBuff;
      } else {
        doc["time"] = "NTP_SYNC_FAILED";
      }
      
      
      doc["humidity"] = hum_Buff;
      doc["temperature"] = temp_Buff;
      doc["Load"] = load_Buff;
    

      char jsonBuffer[512]; // Increased buffer size
      serializeJson(doc, jsonBuffer); // print to client

      if(!(appMgr->conManager->client.connected())) {

           connectAWS(appMgr->conManager); 
      }
         publishOnMqtt(jsonBuffer, appMgr->conManager);
         Serial.println("Published ");
         updateNeeded = false;
      
      
      // client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
      //appMgr->conManager-> client .publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
      

  } else {
      //Serial.println("No significant change in sensor data. Skipping publish.");
  }
         
  // Serial.println(jsonBuffer);
 
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

  pinMode(SDA, INPUT_PULLUP);
  pinMode(SCL, INPUT_PULLUP);

  digitalWrite(SDA, HIGH);
  digitalWrite(SCL, HIGH);

  pinMode(LOAD_CELL_RESET_PIN, INPUT_PULLUP); 
  digitalWrite(LOAD_CELL_RESET_PIN,HIGH);
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

 



 


 
