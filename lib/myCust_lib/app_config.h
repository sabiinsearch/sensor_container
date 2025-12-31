
#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include "Arduino.h"

//  Dummy Sensor Data 
#define TEMP_Demo      27.0
#define HUMIDITY_Demo  105.0
#define LOAD_Demo      100

// For Load Cell
#define CALIBRATION_FACTOR_LOADCELL 2230 // this calibration factor is adjusted according to my load cell

/******************************* Do Not Edit / update these values      **************************************/
//  Mqtt Configurations 
    #define THINGNAME               "container_sensors"        // Change this
    #define WIFI_SSID               "hukam"                    //change this
    #define WIFI_PASSWORD           "guest@13"                 //change this
    #define AWS_ENDPOINT            "am2i6420agb0f-ats.iot.ap-south-1.amazonaws.com"   // AWS MQTT Broker URL
    #define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"           //change this
    #define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"           //change this

    #define PUBLILISH_INTERVAL  10000 
    #define TIMEOUT_INTERVAL    2000
    #define DISPLAY_TIME        30000
#endif