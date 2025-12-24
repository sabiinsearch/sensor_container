#include <Arduino.h>

#ifndef __RECEIVER_BOARD_H__
#define __RECEIVER_BOARD_H__

// Sensors Config

// Didplay
#define ADD_OLED           0x3C
#define SCREEN_WIDTH       128
#define SCREEN_HEIGHT      64
#define OLED_RESET         4

// Temparatur & Humidity
#define ADD_BME280         0x77


#define SDA                21
#define SCL                22

// #define RGB LEDs
#define HEARTBEAT_LED       27         // Red
#define WIFI_LED            14         // Green
#define MQTT_LED            26         // Blue

// # define Level LEDs



// DECLARE OTHER PARTS OF RECEIVER BOARD

// #define SW_pin             21

// #define reset_pin          22

   unsigned long int getBoard_ID();

#endif