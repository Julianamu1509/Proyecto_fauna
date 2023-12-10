// Definimos el pin al que está conectado el sensor LM35
const int lm35Pin = 35; // Cambia este valor según la conexión física que hayas realizado
// Definimos el pin al que está conectado el switch
const int switchPin = 25; // Cambia este valor según la conexión física que hayas realizado


void setup() {
  // Iniciamos la comunicación serial
  Serial.begin(9600);
  // Configuramos el pin del switch como entrada
  pinMode(switchPin, INPUT);
}


void loop() {
  // Leemos el valor analógico del sensor LM35
  int sensorValue = analogRead(lm35Pin);
 
  // Convertimos el valor analógico a voltaje
  float voltage = sensorValue * (5 / 4095.0); // En ESP32, la resolución es de 12 bits (4095 niveles) y funciona a 3.3V
 
  // Convertimos el voltaje a temperatura en grados Celsius
  float temperatureC = voltage * 100; // LM35 tiene una escala de 10mV/°C
 
  // Mostramos la temperatura en la consola serial
  Serial.print("Temperatura: ");
  Serial.print(temperatureC);
  Serial.println(" °C");


  // Leemos el estado del switch
  int switchState = digitalRead(switchPin);


  // Mostramos el estado del switch en la consola serial
  Serial.println(switchState);


  // Esperamos un breve periodo antes de la próxima lectura
  delay(100);
 
  // Esperamos un breve periodo antes de la próxima lectura
  delay(1000);
}


















