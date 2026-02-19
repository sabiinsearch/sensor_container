#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ArduinoJson.h>
#include <time.h>

#include <Arduino.h> 
#include "EEPROM.h"
#include "Preferences.h"

//#include "app_config.h"
#include "appManager.h"
#include "receiverBoard.h"
#include "connectionManager.h"

#include "soc/rtc.h"
#include "esp32-hal-cpu.h"


connectionManager conManagerr;

Preferences pref;

void appManager_ctor(appManager * const me) {


  initBoard();

  Serial.println("Board Initialized..");

   me->conManager = connectionManager_ctor(&conManagerr);
  Serial.println("Connection Manager set with App Manager");
  
  
}

/* Function Implementation */

void connectCloud(appManager* appMgr) {
      connectWiFi(appMgr->conManager);
}


void loop_mgr(appManager* appMgr) {
     loop_con(appMgr->conManager);
}

 void initBoard() {  
  pinMode(HEARTBEAT_LED, OUTPUT);
  digitalWrite(HEARTBEAT_LED, LOW);
  Serial.println("Board Initialized..");
 }
 




 


 
