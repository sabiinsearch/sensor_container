#ifndef __APP_MANAGER_H__
#define __APP_MANAGER_H__

// #include "app_config.h"
// #include "receiverBoard.h"
 #include "connectionManager.h"

typedef struct {

     connectionManager* conManager;     
     
} appManager;

void appManager_ctor(appManager * const me); // constructor

void initBoard(); 
void connectCloud(appManager*);
//void broadcast_appMgr(appManager*);
//void checkConnections_and_reconnect(void * pvParameters);
void loop_mgr(appManager*);


#endif