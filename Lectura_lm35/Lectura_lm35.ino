// Definimos el pin al que está conectado el sensor LM35
const int lm35Pin = 35; // Cambia este valor según la conexión física que hayas realizado

// Función para leer la temperatura del sensor LM35
float leerTemperatura() {
  return (analogRead(lm35Pin) * (5 / 4095.0)) * 100; // LM35 tiene una escala de 10mV/°C
}

void setup() {
  // Iniciamos la comunicación serial
  Serial.begin(9600);
}

void loop() {
  // Llamamos a la función para leer la temperatura
  float temperatura = leerTemperatura();
 
  // Mostramos la temperatura en la consola serial
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");
 
  // Esperamos un breve periodo antes de la próxima lectura
  delay(1000);
}

