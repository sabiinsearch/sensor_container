Project to collect data from various sensors and push to cloud

Terminal command to Erase ESP32 "pio run --target erase"


**** To get MQTT in terminal ****
Go to terminal in command prompt and Change directory to folder with certificates

$mosquitto_pub --cafile AmazonRootCA1.pem --cert Device-certificate.pem.crt --key Device-private.pem.key -h "a3txmb12xz0kci-ats.iot.ap-south-1.amazonaws.com" -p 8883 -i "container_sensors" -q 1 -t "container/sensors/data" -m "{"message":"Hello"}" --tls-version tlsv1.2 -d

$mosquitto_sub --cafile AmazonRootCA1.pem --cert "Device-certificate.pem .crt" --key Device-private.pem.key -h "a3txmb12xz0kci-ats.iot.ap-south-1.amazonaws.com" -p 8883 -i "container_sensors" -q 1 -t "container/sensors/data" -d