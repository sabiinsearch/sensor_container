
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

    #define EQMX_ENDPOINT          "p6f666e4.ala.asia-southeast1.emqxsl.com"   // EMQX MQTT Broker URL
    #define MQTT_USERNAME          "sinny777"
    #define MQTT_PASSWORD          "1SatnamW"
    #define MQTT_PORT              8883
    #define MQTT_PUBLISH_TOPIC   "container/sensors/data"
    #define MQTT_SUBSCRIBE_TOPIC "container/sensors/cmd"
    #define CLIENT_ID             "myContainer_1"          // Change this

    #define PUBLILISH_INTERVAL  600 
    #define TIMEOUT_INTERVAL    120       // seconds
    #define DISPLAY_TIME        10000      // milliseconds
#endif