#include <WiFi.h> // Incluye la librería para manejar la conexión WiFi en el ESP32
#include <PubSubClient.h> // Incluye la librería para utilizar el cliente MQTT
#include <esp_now.h> //Incluye libreria para la comunicación ESP-NOW
#include <esp_wifi.h> //Incluye libreria para manejo de canales WiFi

const char* ssid = "iPhone de Juliana"; // SSID de tu red WiFi
const char* password = "lindura1"; // Contraseña de tu red WiFi
const char* mqtt_server = "test.mosquitto.org"; // Servidor MQTT

WiFiClient espClient; // Crea un cliente WiFi para la comunicación
PubSubClient client(espClient); // Crea un cliente MQTT con el cliente WiFi especificado
unsigned long lastMsg = 0; // Variable para almacenar el tiempo del último mensaje publicado

//Dirección MAC de la tarjeta emisora
uint8_t broadcastAddress[] = {0x0C, 0xB8, 0x15, 0x75, 0x5E,0x08};

//Estructura de los datos recibidos
typedef struct struct_message {
  float presencia;
  float temperatura;
  float laser;
  float bateria;
  float busVolt;
  float shuntVolt;
  float curr;
  float pow;
  float tarjeta; //identificar la tarjeta
} struct_message;

//creacion de la estructura 
struct_message myData;

//callback que se ejecuta cuando los datos son recibidos
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

//función para obtener el canal WiFi
int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}

//función para establecer caracteristicas WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);


  WiFi.mode(WIFI_STA); // Configura el ESP32 en modo estación (conexión a un punto de acceso WiFi)
  WiFi.begin(ssid, password); // Intenta conectarse a la red WiFi con el SSID y la contraseña proporcionados


  while (WiFi.status() != WL_CONNECTED) { // Espera a que se establezca la conexión WiFi
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP:");
  Serial.println(WiFi.localIP()); // Imprime la dirección IP asignada al ESP32
}


void callback(char* topic, byte* payload, unsigned int length) {
  // Esta función se ejecuta cuando se recibe un mensaje en los topics suscritos
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) { // Intenta conectar al servidor MQTT con un ID de cliente único
      Serial.println("Conectado");
      client.publish("/Publish", "Bienvenido"); // Publica un mensaje de bienvenida en el topic "/Publish"
      client.subscribe("/Subscribe"); // Se suscribe al topic "/Subscribe" para recibir mensajes
    } else {
      Serial.print("falló, rc=");
      Serial.print(client.state()); // Imprime el estado de la conexión MQTT
      Serial.println(" intentando de nuevo en 5 segundos");
      delay(5000); // Espera 5 segundos antes de intentar nuevamente la conexión MQTT
    }
  }
}


void setup() {
  Serial.begin(115200); // Inicia la comunicación serial a 115200 baudios
  setup_wifi(); // Llama a la función para conectar a la red WiFi
  int32_t channel = getWiFiChannel(ssid);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  client.setServer(mqtt_server, 1883); // Configura el servidor MQTT y el puerto
  client.setCallback(callback); // Establece la función de retrollamada para el cliente MQTT

  //inicia protocolo ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //si la inicialización es exitosa se añade un dispostivo
  esp_now_register_recv_cb(OnDataRecv);
}


void loop() {
  if (!client.connected()) {
    reconnect(); // Si el cliente MQTT no está conectado, intenta reconectar
  }
  client.loop(); // Maneja la comunicación MQTT en cada iteración del bucle principal


  unsigned long now = millis();
  if (now - lastMsg > 300) { // Publicar datos cada 2 segundos
    lastMsg = now;
    
    //vector chat para la conversion de los valroes flotantes
    char preStr[8];
    char tempStr[8];
    char laStr[8];
    char batStr[8];
    char bvStr[8];
    char svStr[8];
    char cStr[8];
    char pStr[8];
    char tarStr[8];
    
    //se realiza la conversión de flotante a char
    dtostrf(myData.presencia, 4, 2, preStr); // Convierte el float a una cadena con 2 decimales
    dtostrf(myData.temperatura, 4, 2, tempStr); // Convierte el float a una cadena con 2 decimales
    dtostrf(myData.laser, 4, 2, laStr); // Convierte el float a una cadena con 2 decimales
    dtostrf(myData.bateria, 4, 2, batStr); // Convierte el float a una cadena con 2 decimales
    dtostrf(myData.busVolt, 4, 2, bvStr); // Convierte el float a una cadena con 2 decimales
    dtostrf(myData.shuntVolt, 4, 2, svStr); // Convierte el float a una cadena con 2 decimales
    dtostrf(myData.curr, 4, 2, cStr); // Convierte el float a una cadena con 2 decimales
    dtostrf(myData.pow, 4, 2, pStr); // Convierte el float a una cadena con 2 decimales
    dtostrf(myData.tarjeta, 4, 2, tarStr); // Convierte el float a una cadena con 2 decimales

    client.publish("/presencia", preStr); // Publica la temperatura en el topic "/temp" a través del cliente MQTT
    Serial.print("Presencia: ");
    Serial.println(myData.presencia);
    
    client.publish("/temp", tempStr); // Publica la temperatura en el topic "/temp" a través del cliente MQTT
    Serial.print("Temperatura: ");
    Serial.println(myData.temperatura);

    client.publish("/laser", laStr); // Publica la temperatura en el topic "/temp" a través del cliente MQTT
    Serial.print("Laser: ");
    Serial.println(myData.laser);

    client.publish("/bateria", batStr); // Publica la temperatura en el topic "/temp" a través del cliente MQTT
    Serial.print("Bateria: ");
    Serial.println(myData.bateria);

    client.publish("/busVolt", bvStr); // Publica la temperatura en el topic "/temp" a través del cliente MQTT
    Serial.print("Bus volt: ");
    Serial.println(myData.busVolt);

    client.publish("/shuntVolt", svStr); // Publica la temperatura en el topic "/temp" a través del cliente MQTT
    Serial.print("Shunt volt: ");
    Serial.println(myData.shuntVolt);

    client.publish("/curr", cStr); // Publica la temperatura en el topic "/temp" a través del cliente MQTT
    Serial.print("Curr: ");
    Serial.println(myData.curr);

    client.publish("/pow", pStr); // Publica la temperatura en el topic "/temp" a través del cliente MQTT
    Serial.print("Pow: ");
    Serial.println(myData.pow);

    client.publish("/tarjeta", tarStr); // Publica la temperatura en el topic "/temp" a través del cliente MQTT
    Serial.print("Tarjeta: ");
    Serial.println(myData.tarjeta);
    Serial.println(" ");

  }
}