#include <Arduino.h>
#include <esp_now.h>
#include "WiFi.h"

// my library
#include "receiverBoard.h"

// Peer MAC Address
uint8_t peerAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Replace

esp_now_peer_info_t peerInfo;


// Structure to send
typedef struct struct_message {
    int id;
    float temp;
} struct_message;

struct_message myData;


// Send callback
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  digitalWrite(HEARTBEAT_LED, HIGH);
  delay(200);  
  digitalWrite(HEARTBEAT_LED, LOW);
  delay(200);
}

// Receive callback
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.println(myData.temp);
  digitalWrite(HEARTBEAT_LED, HIGH);
  delay(200);  
  digitalWrite(HEARTBEAT_LED, LOW);
  delay(200);
}

void setupESP_NOW()
  {
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();  
    // Init ESP-NOW
    if (esp_now_init() != 0) 
      {
        Serial.println("Error initializing ESP-NOW");
        return;
      }
    // Set ESP-NOW Role
    //esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);
    esp_now_register_send_cb(OnDataSent);

    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
    // Register peer
    //esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
    Serial.println("ESP-NOW setup done.");
    delay(500);
  }

  void intBoard() {
    pinMode(HEARTBEAT_LED, OUTPUT);
    digitalWrite(HEARTBEAT_LED, LOW);
  }

void setup() {
  Serial.begin(115200);
  intBoard();
  setupESP_NOW();
}

void loop() {
    delay(1000);    
    myData.id = 1;
    myData.temp = 24.5;
    esp_err_t result = esp_now_send(peerAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {

        Serial.println("Sent with success");
    } else {
        Serial.println("Error sending the data");
    }
    delay(1000);

}