#include <ESP32Servo.h> //libreria para el servo
#include <Wire.h> //libreria para comunicación I2C
#include <Adafruit_GFX.h> //libreria para la OLED
#include <Adafruit_SSD1306.h> //libreria para la OLED
#include "DFRobot_INA219.h" //libreria para el manejo del sensor INA219
#include <esp_now.h> //libreria para protocolo esp-now
#include <WiFi.h> //libreria para conexión wi-fi

//definición de pines
#define lm35Pin 35
//#define lm35Pin 25 //pra el sensor LM35
#define laserPin 33 //para el laser
#define bat 34 //para el nivel de bateria
#define radar 32 //para el radar
#define servoPin 14 //para el servo

#define SCREEN_WIDTH 128 //ancho de la OLED
#define SCREEN_HEIGHT 64 //alto de la OLED

#define OLED_RESET     -1 //pin de reset
#define SCREEN_ADDRESS 0x3c //dirección de la OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//se crea objeto de tipo DFRobot_INA219_IIC con comunicación I2C para el sensor
DFRobot_INA219_IIC     ina219(&Wire, INA219_I2C_ADDRESS4);

//valor de corriente en mV para calibrar el sensor
float ina219Reading_mA = 1000; 
 //valor de corriente dada por medidor externo para la calibración
float extMeterReading_mA = 1000;
//vector para almacenar los valores del sensor INA
float vector_ina[4];
//creación de objeto tipo servo
Servo myServo;
//dirección MAC de la tarjeta receptora
//08:D1:F9:99:D3:08
uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0x99, 0xD3,0x08};

//estructura de los mensajes a enviar
typedef struct struct_message {
  float presencia;
  float temperatura;
  float laser;
  float bateria;
  float busVolt;
  float shuntVolt;
  float curr;
  float pow;
  float tarjeta;
} struct_message;

//mensaje tipo estructura
struct_message myData;

esp_now_peer_info_t peerInfo;

//callback, cuando los datos se nevian
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//función para mostrar un texto en la pantalla OLED
void showText(const char* text, int textSize, int x, int y, int delayTime) {
  display.setTextSize(textSize); //tamaño del texto
  display.setTextColor(SSD1306_WHITE); //color de texto
  display.setCursor(x, y); //ubicación del texto
  display.println(text); //impresión del texto
  display.display(); //actualiza pantalla OLED
}

//función para rotar el servo
void rotateServo(int degrees) {
  int pos = map(degrees, 0, 180, 0, 180); // Mapea los grados a valores de pulso
  myServo.write(pos); // Mueve el servo a la posición correspondiente
  delay(15); // Pequeña pausa para permitir que el servo se mueva
}

//funución para la lectura de las variables del sensor ina
void ina(){
  Serial.print("BusVoltage:   ");
  vector_ina[0] = ina219.getBusVoltage_V();
  //se imprime el voltaje del bus
  Serial.print(ina219.getBusVoltage_V(), 2);
  Serial.println("V");
  Serial.print("ShuntVoltage: ");
  //se imprime la tensión del bus
  vector_ina[1] = ina219.getShuntVoltage_mV();
  Serial.print(ina219.getShuntVoltage_mV(), 3);
  Serial.println("mV");
  Serial.print("Current:      ");
  //se imprime la corriente
  vector_ina[2] = ina219.getCurrent_mA();
  Serial.print(ina219.getCurrent_mA(), 1);
  Serial.println("mA");
  Serial.print("Power:        ");
  //se imprime la potencia
  vector_ina[3] = ina219.getPower_mW();
  Serial.print(ina219.getPower_mW(), 1);
  Serial.println("mW");
  Serial.println("");
  delay(2000);
}

// Función para leer la temperatura del sensor LM35
float leerTemperatura() {
  return (analogRead(lm35Pin) * (5 / 4095.0)) * 100; // LM35 tiene una escala de 10mV/°C
}

// Función para leer el voltaje de la batería
float leer_bat() {
  return (analogRead(bat) * 3.3 / 4095) * 1.51;
}

void setup() {
  //inicializa puerto serial
  Serial.begin(9600);

  //inicializa el display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.display();  // Mostrar pantalla de inicio
  delay(2000);        // Esperar 2 segundos

  //definir el modo de los pines
  pinMode(lm35Pin, INPUT);
  pinMode(bat, INPUT);
  pinMode(radar, INPUT);
  pinMode(laserPin, OUTPUT);

  myServo.attach(servoPin);

  while(!Serial);

  Serial.println();

  //inicialización del sensor INA219 
  while(ina219.begin() != true) {
    Serial.println("INA219 begin faild");
    delay(2000);
  }

  //Calibración del sensor con los valores pre-establecidos
  ina219.linearCalibrate(ina219Reading_mA, extMeterReading_mA);
  Serial.println();

  //ESPNOW

  //estabecer el dispositivo como una estación WiFi
  WiFi.mode(WIFI_STA);

  //inicializacion ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //cuando ESP-NOW es exitoso se registra el dispositivo
  //recibir el estado del paquete enviado
  esp_now_register_send_cb(OnDataSent);
  
  //registro peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  //añadir peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}



void loop() {
  int presencia = digitalRead(radar);
  int act_laser = 0;

  // Calcular el voltaje de la batería
  float voltaje_calculado = leer_bat();

  // Llamamos a la función para leer la temperatura
  float temperaturaCelsius = leerTemperatura();
  //float temperaturaCelsius = (analogRead(lm35Pin) * (5 / 4095.0)) * 100;

  //impresión del estado de la presencia en la OLED
  display.clearDisplay();
  String presenciaString = "Pres: " + String(presencia);
  showText(presenciaString.c_str(), 2, 0, 0, 2000);

  //impresion del estado de la bateria en la OLED
  String voltajeString = "Volt: " + String(voltaje_calculado);
  showText(voltajeString.c_str(), 2, 0, 20, 2000);
  
   // Rotar el servo en base a la presencia
  if(presencia == 1){
    rotateServo(360); //rotar servo
    digitalWrite(laserPin, HIGH);   // poner el Pin del laser en HIGH
    act_laser = 1;
  }else{
    rotateServo(0); //devolver el servo
    digitalWrite(laserPin, LOW);   // poner el Pin en LOW
    act_laser = 0;
  }

  // Enviar datos a la consola Serial
  Serial.print("Presencia: ");
  Serial.print(presencia);

  Serial.print(" Temperatura: ");
  Serial.print(temperaturaCelsius);
  Serial.print(" °C");

  Serial.print(" Laser: ");
  Serial.print(act_laser);

  Serial.print(" Batería: ");
  Serial.print(voltaje_calculado);
  Serial.println("V");

  //llamado a la función del sensor INA
  ina();

  //ESP-NOW
  //establecer valores a enviar por medio de ESP-NOW
  myData.presencia = presencia;
  myData.temperatura = temperaturaCelsius;
  myData.laser = act_laser;
  myData.bateria = voltaje_calculado;
  myData.busVolt = vector_ina[0];
  myData.shuntVolt = vector_ina[1];
  myData.curr = vector_ina[2];
  myData.pow = vector_ina[3];
  myData.tarjeta = 1; //identificar la tarjeta
  
  //enviar mensaje por esp-now
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  //confirmar si el envio fue exitoso o fallido
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}

