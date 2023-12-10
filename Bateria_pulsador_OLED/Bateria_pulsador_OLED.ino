// Incluir bibliotecas para ESP32 Servo, comunicación I2C y pantalla OLED
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>

// Definiciones de pines
#define LM 25         // Pin no utilizado
#define bat 34        // Pin de voltaje de la batería
#define radar 32      // Pin del radar de presencia

// Configuración de la pantalla OLED
#define SCREEN_WIDTH 128    // Ancho de la pantalla OLED, en píxeles
#define SCREEN_HEIGHT 64    // Altura de la pantalla OLED, en píxeles
#define OLED_RESET -1       // Pin de reinicio # (o -1 si se comparte el pin de reinicio de Arduino)
#define SCREEN_ADDRESS 0x3c ///< Ver la hoja de datos para la dirección; 0x3D para 128x64, 0x3C para 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variable para hacer un seguimiento del tiempo de presencia
unsigned long presenciaTiempo = 0;

// Función para mostrar texto en la pantalla OLED
void showText(const char* text, int textSize, int x, int y, int delayTime) {
  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(x, y);
  display.println(text);
  display.display();
}

void setup() {
  // Inicializar la comunicación serial
  Serial.begin(115200);

  // Inicializar la pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Fallo en la asignación de SSD1306"));
    for (;;);
  }

  // Mostrar la pantalla de inicio
  display.display();
  delay(2000); // Esperar 2 segundos

  // Configurar modos de pin para la batería y el radar de presencia
  pinMode(bat, INPUT);
  pinMode(radar, INPUT);
}

// Función para leer el voltaje de la batería
float leer_bat() {
  return (analogRead(bat) * 3.3 / 4095) * 1.51;
}

void loop() {
  // Leer valores de la batería y el radar de presencia
  int bat_valor = analogRead(bat);
  int presencia = digitalRead(radar);

  // Calcular el voltaje de la batería
  float voltaje_calculado = leer_bat();

  // Borrar la pantalla OLED
  display.clearDisplay();

  // Mostrar información de presencia
  String presenciaString = "Pres: " + String(presencia);
  showText(presenciaString.c_str(), 2, 0, 0, 2000);

  // Mostrar información de voltaje de la batería
  String voltajeString = "Volt: " + String(voltaje_calculado);
  showText(voltajeString.c_str(), 2, 0, 20, 2000);

  delay(1000);

  // Enviar datos a la consola Serial
  Serial.print("Presencia: ");
  Serial.print(presencia);

  Serial.print(" Batería: ");
  Serial.print(voltaje_calculado);
  Serial.println("V");
}

