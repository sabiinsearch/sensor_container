// Libraries for AWS
#include "secrets.h"             //  for AWS Certificates and Keys
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

// Others



// my libraries
#include "appManager.h"
#include "receiverBoard.h"
#include "sensor.h"

// my Managers
appManager managr;


// DHT dht(DHTPIN, DHTTYPE);

// WiFiClientSecure net = WiFiClientSecure();
// PubSubClient client(net);



void setup()
{
  Serial.begin(115200);
 
    // Initiating Manager
  Serial.println("Initializing App Manager..");
  appManager_ctor(&managr);

  connectCloud(&managr);
  // dht.begin();
}

void loop()
{
  // h = dht.readHumidity();
  // t = dht.readTemperature();


  // if (isnan(h) || isnan(t) )  // Check if any reads failed and exit early (to try again).
  // {
  //   Serial.println(F("Failed to read from DHT sensor!"));
  //   return;
  // }

  getSensorData_print_update(&managr);
  //client.loop();
  loop_mgr(&managr);
  delay(1000);
}