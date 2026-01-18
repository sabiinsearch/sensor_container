#include <ArduinoJson.h>
#include "connectionManager.h"
#include "Preferences.h"
#include "appManager.h"

// for WiFi and mqtt

#include <WiFiManager.h> 
#include "WiFi.h"
#include "WiFiGeneric.h"
#include "WiFiSTA.h"
#include "WiFiClientSecure.h"



#include <PubSubClient.h>   // for Mqtt
#include "secrets.h"             //  for AWS Certificates and Keys

#include "app_config.h"     // for Custom Configration
#include "receiverBoard.h"

/*  */

Preferences preferences;

WiFiClientSecure  net;

PubSubClient pub_sub_client(net);

WiFiManager wm; // WiFi Manager 

String sub_topic = AWS_IOT_SUBSCRIBE_TOPIC;
String pub_topic = AWS_IOT_PUBLISH_TOPIC;

char server[50] = AWS_ENDPOINT;
// char mqttUser[20] = MQTT_USER;
// char mqttPassword[20] = MQTT_PASSWORD;



/* constructor implementation */

connectionManager * const connectionManager_ctor(connectionManager * const me ) {
   
  //  initConfig(me);
   //resetWifi(me);       // To Reset Wifi
   connectWiFi(me);
  // connectAWS(me);
   connectEQMX(me);

   return me;
}

/* Function implementation */

  void messageHandler(char* topic, byte* payload, unsigned int length)
  {
   Serial.print("incoming: ");
   Serial.println(topic);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);
  }

void publishOnMqtt(char* data, connectionManager* con) {
   char jsonBuffer_to_cloud[512];
   strcpy(jsonBuffer_to_cloud, data);
      
  // client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
   // pub_sub_client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer_to_cloud);
    pub_sub_client.publish(MQTT_PUBLISH_TOPIC, jsonBuffer_to_cloud);

}

void connectAWS(connectionManager * con) {

  // WiFi.mode(WIFI_STA);
  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  
  // Serial.println("Connecting to Wi-Fi");

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  // net.setCACert(AWS_CERT_CA);
  // net.setCertificate(AWS_CERT_CRT);
  // net.setPrivateKey(AWS_CERT_PRIVATE);
  

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  pub_sub_client.setServer(AWS_ENDPOINT, 8883);

  // Create a message handler
  pub_sub_client.setCallback(messageHandler);

  Serial.println("Connecting to AWS IOT");

  while (!pub_sub_client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }

  if (!pub_sub_client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  pub_sub_client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void connectEQMX(connectionManager * con) {
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  
  net.setInsecure(); // Disable certificate verification

  pub_sub_client.setServer(EQMX_ENDPOINT, MQTT_PORT);

  // Create a message handler
  pub_sub_client.setCallback(messageHandler);
  
  String client_id = "esp32-client-" + String(WiFi.macAddress());

  Serial.println("Connecting to EMQX");

    if (pub_sub_client.connect(CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.print("EMQX Connected - "); 
      Serial.println(CLIENT_ID);
    } else {
      delay(2000);
    }


  // Subscribe to a topic
  pub_sub_client.subscribe(MQTT_SUBSCRIBE_TOPIC);
  pub_sub_client.publish(MQTT_PUBLISH_TOPIC, "Hello from myContainer");

}

void loop_con(connectionManager* con) {
  pub_sub_client.loop();
}

void initWiFi() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  //wm.setWiFiAutoReconnect(true);
}



void reconnectWiFi(connectionManager  * con){
  bool res;
  res = wm.autoConnect("myContainer"); // anonymous ap
    if(!res) {
        con->Wifi_status = false;
        digitalWrite(WIFI_LED,HIGH);
        Serial.println("Failed to connect");
        delay(3000);
      //  ESP.restart();
        delay(5000);
    } 
    else {
        //if you get here you have connected to the WiFi  
        digitalWrite(WIFI_LED,LOW);  
        con->Wifi_status = true;   
      //  Serial.println("Wifi connected...yeey :)");       
    }
}

bool connectWiFi(connectionManager * con) {
  bool res;
  digitalWrite(HEARTBEAT_LED,LOW);  
  wm.setConfigPortalTimeout(TIMEOUT_INTERVAL); // If no access point name has been previously entered disable timeout
  // wm.setConnectTimeout(TIMEOUT_INTERVAL);
  res = wm.autoConnect("myContainer"); // auto generated AP name from chipid
    while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

    if(res) {
      //if you get here you have connected to the WiFi         
        con->Wifi_status = true;
        digitalWrite(HEARTBEAT_LED,HIGH);
        digitalWrite(WIFI_LED,LOW);   
      //  Serial.println("Wifi connected...yeey :)");           
    }
    return res;
}

void resetWifi(connectionManager * con) {
    con->Wifi_status = false;
    // wm.setConnectTimeout(TIMEOUT_INTERVAL);
    wm.setConfigPortalTimeout(TIMEOUT_INTERVAL); // If no access point name has been previously entered disable timeout
    wm.resetSettings(); // reset settings - wipe stored credentials for testing, these are stored by the esp library
    // ESP.restart();
    digitalWrite(WIFI_LED,HIGH);
}




