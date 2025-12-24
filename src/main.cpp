// Libraries for AWS
#include "secrets.h"             //  for AWS Certificates and Keys
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

// Others

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>

// my libraries
#include "appManager.h"
#include "receiverBoard.h"
#include "sensor.h"

// my Managers
appManager managr;

// Create display object with custom I2C
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DHT dht(DHTPIN, DHTTYPE);

// WiFiClientSecure net = WiFiClientSecure();
// PubSubClient client(net);



void setup()
{
  Serial.begin(115200);
 
    // Initiating Manager
  Serial.println("Initializing App Manager..");
  appManager_ctor(&managr);
  // Start I2C on custom pins (for ESP32)
  Wire.begin(SDA, SCL);

 //  Initialize the OLED display
  if (!display.begin(ADD_OLED, 0x3C)) {
    Serial.println(F("SSD1306 initialization failed"));
    while (true);  // Stop execution if display fails to initialize
  }
  display.clearDisplay();
  display.display();

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

  Serial.print(F("Humidity: "));
  Serial.print(HUMIDITY_Demo);
  Serial.print(F("% Temperature: "));
  Serial.print(TEMP_Demo);
  Serial.println(F("°C "));

  publishMessage(&managr);
  //client.loop();
  loop_mgr(&managr);
  delay(1000);
}