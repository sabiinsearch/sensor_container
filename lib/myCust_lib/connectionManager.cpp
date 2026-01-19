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
#include "HttpClient.h"
#include "sigv4.h"
#include "mbedtls/md.h"


#include <PubSubClient.h>   // for Mqtt
#include "secrets.h"             //  for AWS Certificates and Keys

#include "app_config.h"     // for Custom Configration
#include "receiverBoard.h"

/*  */

const char* AWS_ACCESS_KEY = ACCESS_KEY; 
const char* AWS_SECRET_KEY = SECRET_KEY;
const char* S3_REGION = AWS_SREGION; 
const char* BUCKET_NAME = S3_BUCKET_NAME;

String hostString = String(S3_BUCKET_NAME) + ".s3." + String(AWS_SREGION) + ".amazonaws.com";
const char* BUCKET_HOST = hostString.c_str();

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
  // connectEQMX(me);

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

// Helper function to calculate SHA256 and return as hex string
String getSHA256Hex(const char* payload) {
    // 32 bytes for the SHA256 hash result
    unsigned char shaResult[32]; 
    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

    const size_t payloadLen = strlen(payload);

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
    mbedtls_md_starts(&ctx);
    mbedtls_md_update(&ctx, (const unsigned char*)payload, payloadLen);
    mbedtls_md_finish(&ctx, shaResult);
    mbedtls_md_free(&ctx);

    // Convert the 32-byte binary hash into a 64-char hex string
    char hex[65]; // 64 chars + null terminator
    for (int i = 0; i < 32; i++) {
        sprintf(&hex[i * 2], "%02x", (int)shaResult[i]);
    }
    hex[64] = '\0'; // Ensure null termination

    return String(hex);
}


void uploadToS3(char* data, connectionManager* con) {
  // Placeholder for S3 upload functionality
  Serial.println("Uploading data to S3...");
  // Implement S3 upload logic here using AWS SDK or HTTP requests
  //net.setInsecure(); // Disable certificate verification
 
  /*
  net.setSectretKey(SECRET_KEY);
  net.setAccessKey(ACCESS_KEY);

    if (!net.connect(S3_BUCKET_NAME, 443)) {
       Serial.println("Connection failed!");
    return;
    }

   char jsonBuffer_to_cloud[512];
   strcpy(jsonBuffer_to_cloud, data);

     // 3. Send HTTPS PUT Request
  net.print("PUT /data.json HTTP/1.1\r\n");
  net.print("Host: " + String(S3_BUCKET_NAME) + "\r\n");
  net.print("Content-Type: application/json\r\n");
  net.print("Content-Length: " + String(strlen(jsonBuffer_to_cloud)) + "\r\n");
  // net.print("Authorization: " + authHeader + "\r\n"); // Essential for private buckets
  net.print("Connection: close\r\n\r\n");
  net.print(jsonBuffer_to_cloud);

  // 4. Read Response
  while (net.connected()) {
    String line = net.readStringUntil('\n');
    if (line == "\r") break;
    Serial.println(line);
  }
*/
    // Set Signing Parameters
    const char* amzDate = "20260120T120000Z"; // Use current NTP time in production 
        // Initialize the structure
    SigV4Credentials_t credentials = {
        ACCESS_KEY,                  // Value for the first field
        strlen(ACCESS_KEY),          // Value for the second field
        SECRET_KEY,                  // Value for the third field
        strlen(SECRET_KEY)           // Value for the fourth field
   };
    
  SigV4HttpParameters_t httpParams = {
    .pHttpMethod = "GET",
    .httpMethodLen = 3,
    .pPath = "/log/data.txt",
    .pathLen = 17,
    .pQuery = NULL,
    .queryLen = 0
  };

    SigV4Parameters_t sigParams = {
        .pCredentials = &credentials,
        .pDateIso8601 = amzDate,
        .pRegion = "ap-south-1",
        .regionLen = 9,
        .pService = "s3",
        .serviceLen = 2,         
        .pHttpParameters = &httpParams
    //    .pCryptoInterface = &myCryptoInterface
    };


    // 1. Prepare JSON Payload
    String jsonPayload = data;
    String payloadHash = getSHA256Hex(jsonPayload.c_str());
     

    // 3. Generate Authorization Header
    // 1. Existing variables
     char authHeader[1024]; 
     size_t authHeaderLen = sizeof(authHeader);

   // 2. NEW: Variables to receive signature metadata
    char * pSignatureLocation = NULL;
    size_t signatureValueLen = 0;

   // 3. Corrected function call with 5 parameters
    SigV4Status_t status = SigV4_GenerateHTTPAuthorization(
       &sigParams, 
       authHeader, 
       &authHeaderLen,
       &pSignatureLocation, // NEW: Pointer to signature location (char **)
       &signatureValueLen   // NEW: Pointer to signature length (size_t *) 
    );

    if (status == SigV4Success) {        
        String url = "https://" + String(S3_BUCKET_NAME) + ".s3." + String(AWS_SREGION) + ".amazonaws.com";
        HttpClient http(net);               
        http.beginRequest();
        http.get(url.c_str());
        http.addHeader("Authorization", authHeader);
        http.addHeader("x-amz-date", amzDate);
        http.addHeader("x-amz-content-sha256", payloadHash); // Mandatory for S3
        http.addHeader("Content-Type", "application/json");

        int httpCode = http.PUT(jsonPayload);
        Serial.printf("S3 PUT Result: %d\n", httpCode);
        http.end();
    }

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




