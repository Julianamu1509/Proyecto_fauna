#include <WiFi.h> // Incluye la librería para manejar la conexión WiFi en el ESP32
#include <PubSubClient.h> // Incluye la librería para utilizar el cliente MQTT


const int LM35_PIN = 35; // Pin del sensor LM35
const int SWITCH_PIN = 25; // Pin del switch
const char* ssid = "iPhone de Juliana"; // SSID de tu red WiFi
const char* password = "lindura1"; // Contraseña de tu red WiFi
const char* mqtt_server = "test.mosquitto.org"; // Servidor MQTT


WiFiClient espClient; // Crea un cliente WiFi para la comunicación
PubSubClient client(espClient); // Crea un cliente MQTT con el cliente WiFi especificado
unsigned long lastMsg = 0; // Variable para almacenar el tiempo del último mensaje publicado
float tempCelsius = 0; // Variable para almacenar la temperatura en grados Celsius
int switchState = 0; // Variable para almacenar el estado del switch


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
  // Aquí puedes agregar acciones específicas basadas en los mensajes recibidos
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
  pinMode(LM35_PIN, INPUT); // Configura el pin del sensor LM35 como entrada
  pinMode(SWITCH_PIN, INPUT_PULLUP); // Configura el pin del switch como entrada con resistencia pull-up
  Serial.begin(115200); // Inicia la comunicación serial a 115200 baudios
  setup_wifi(); // Llama a la función para conectar a la red WiFi
  client.setServer(mqtt_server, 1883); // Configura el servidor MQTT y el puerto
  client.setCallback(callback); // Establece la función de retrollamada para el cliente MQTT
}


void loop() {
  if (!client.connected()) {
    reconnect(); // Si el cliente MQTT no está conectado, intenta reconectar
  }
  client.loop(); // Maneja la comunicación MQTT en cada iteración del bucle principal


  unsigned long now = millis();
  if (now - lastMsg > 2000) { // Publicar datos cada 2 segundos
    lastMsg = now;


    int sensorValue = analogRead(LM35_PIN); // Lee el valor analógico del sensor LM35
    float voltage = sensorValue * (5.0 / 4095.0); // Convierte el valor a voltaje
    float tempCelsius = voltage * 100.0; // Convierte el voltaje a temperatura en grados Celsius


    char tempStr[8];
    dtostrf(tempCelsius, 4, 2, tempStr); // Convierte el float a una cadena con 2 decimales


    client.publish("/temp", tempStr); // Publica la temperatura en el topic "/temp" a través del cliente MQTT
    Serial.print("Temperatura: ");
    Serial.println(tempCelsius);


    switchState = digitalRead(SWITCH_PIN); // Lee el estado del switch


    // Publica el estado del switch en MQTT
    if (switchState == HIGH) {
      client.publish("/switch", "1"); // Si el switch está presionado, publica "1"
      Serial.println("Switch presionado");
    } else {
      client.publish("/switch", "0"); // Si el switch no está presionado, publica "0"
      Serial.println("Switch no presionado");
    }
  }
}



