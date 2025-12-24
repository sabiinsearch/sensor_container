#ifndef __APP_MANAGER_H__
#define __APP_MANAGER_H__

#include "app_config.h"
#include "connectionManager.h"
#include "HX711.h"

/*Application Manager's attributes*/

typedef struct {

     connectionManager* conManager;
     HX711 scale;
     float threshold;

} appManager;

void appManager_ctor(appManager * const me); // constructor

void initBoard(); 
void initScreen(); 
void printOnScreen(int, int, int, int); 
HX711 setLoadCell(appManager*);
void connectCloud(appManager*);
void broadcast_appMgr(appManager*);
void checkConnections_and_reconnect(void * pvParameters);
void setBoardWithLC(appManager*);
void getSensorData_print_update(appManager*);
void loop_mgr(appManager*);


// functions to set LEDs as per status

/*

HEARTBEAT_LED             // Red
WIFI_LED                  // Blue
BLE_LED                   // Green
*/

#endif