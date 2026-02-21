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
<<<<<<< HEAD
  Serial.println("Container Initialized..");
=======

  Serial.println("Board Initialized..");
>>>>>>> e34b1cb5e8f2721917795685fe7ae18728db1de8

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
<<<<<<< HEAD
  // Configuring Board pins

  pinMode(SDA, INPUT_PULLUP);
  pinMode(SCL, INPUT_PULLUP);

  digitalWrite(SDA, HIGH);
  digitalWrite(SCL, HIGH);

  pinMode(LOAD_CELL_RESET_PIN, INPUT_PULLUP); 
  digitalWrite(LOAD_CELL_RESET_PIN,HIGH);
  
  initRGB();

=======
  pinMode(HEARTBEAT_LED, OUTPUT);
  digitalWrite(HEARTBEAT_LED, LOW);
  Serial.println("Board Initialized..");
>>>>>>> e34b1cb5e8f2721917795685fe7ae18728db1de8
 }
 




 


 
