#ifndef __CONNNECTION_MANAGER_H__
#define __CONNNECTION_MANAGER_H__

#include "Arduino.h"
#include <WiFiManager.h>
#include "app_config.h"


/*Connection Manager's attributes*/
typedef struct {

  WiFiManager wifi_manager;
  bool radio_status;  
  bool ble_status;
  bool Wifi_status;
  bool mqtt_status; 
//  struct configuration_t config; 

} connectionManager;


/*Connection Manager's operations*/
 
   connectionManager * const  connectionManager_ctor(connectionManager * const me);     // Constructor

   void print_communication(); 
   void checkConnections_and_reconnect(void *);
   void initWiFi();
   void mqtt_loop();
   bool connectMQTT(connectionManager*);
   bool connectWiFi(connectionManager*);
   void reconnectWiFi(connectionManager*);
   void resetWifi(connectionManager*);
   void initRadio(connectionManager*);
   void checkDataOnRadio();
   void mqttCallback(char*, byte*, unsigned int);
   void publishData(String, connectionManager*);
   void publishOnRadio(String,connectionManager*);
   void publishOnMqtt(String, connectionManager*);
   char* string2char(String);
   void initConfig(connectionManager*);
   void print_default_config();
   bool getWiFi_Availability(connectionManager*);


#endif