#include "WiFi.h"

void setup() {
  // Inicialización de la comunicación serial a 115200 baudios
  Serial.begin(115200);
  
  // Configuración del modo WiFi a estación (WIFI_MODE_STA)
  WiFi.mode(WIFI_MODE_STA);
  
  // Imprimir la dirección MAC del módulo WiFi en la consola serial
  Serial.println(WiFi.macAddress());
}

void loop() {
  // En el bucle, se imprime continuamente la dirección MAC del módulo WiFi en la consola serial
  Serial.println(" la MAC de la tarjeta es: ");
  Serial.println(WiFi.macAddress());
  Serial.println(" ");
  delay (100);
}

