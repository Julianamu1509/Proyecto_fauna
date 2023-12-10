/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
//B0:A7:32:DA:A6:DC---juli
//0C:B8:15:75:5E:08---osuna 
uint8_t broadcastAddress[] = {0x0C, 0xB8, 0x15, 0x75, 0x5E,0x08};

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
   float presencia;
  float temperatura;
  float laser;
  float bateria;
  float busVolt;
  float shuntVolt;
  float curr;
  float pow;
  float tarjeta = 1; //identificar la tarjeta
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Presencia: ");
  Serial.println(myData.presencia);
  Serial.print("Temperatura: ");
  Serial.println(myData.temperatura);
  Serial.print("Laser: ");
  Serial.println(myData.laser);
  Serial.print("Bateria: ");
  Serial.println(myData.bateria);
  Serial.print("Bus voltage: ");
  Serial.println(myData.busVolt);
  Serial.print("Shunt voltage: ");
  Serial.println(myData.shuntVolt);
  Serial.print("Current: ");
  Serial.println(myData.curr);
  Serial.print("Power: ");
  Serial.println(myData.pow);
  Serial.println();
  Serial.print("Tarjeta: ");
  Serial.println(myData.tarjeta);
  Serial.println();
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {

}
