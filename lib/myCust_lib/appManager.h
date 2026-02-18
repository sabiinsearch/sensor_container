#ifndef __APP_MANAGER_H__
#define __APP_MANAGER_H__

// #include "app_config.h"
// #include "receiverBoard.h"
 #include "connectionManager.h"
//#include "HX711.h"
// #include <HX711_ADC.h>

/*Application Manager's attributes*/

typedef struct {

     connectionManager* conManager;     
     
     float prev_hum;
     float prev_temp;
     float prev_load;
     float load_threshold;
     
} appManager;

void appManager_ctor(appManager * const me); // constructor

void initBoard(); 
void initScreen(); 
void initDHT(); 
void displayWelcomeScreen();
void readyScreen(); 
void printOnScreen(int, int, int, int, String); 
//HX711 setLoadCell(appManager*);
void connectCloud(appManager*);
void broadcast_appMgr(appManager*);
void checkConnections_and_reconnect(void * pvParameters);
void setBoardWithLC(appManager*);
void getSensorData_print_update(appManager*);
void loop_mgr(appManager*);
void checkButtonPressed(appManager*);



// functions to set LEDs as per status

/*

HEARTBEAT_LED             // Red
WIFI_LED                  // Blue
BLE_LED                   // Green
*/

#endif