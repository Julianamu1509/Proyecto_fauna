void setup() {
  pinMode(2, OUTPUT); // Configura el pin GPIO 2 como salida
  Serial.begin(9600); // Inicia la comunicación serial a 9600 baudios
}

void loop() {
  digitalWrite(2, HIGH);   // Enciende el LED interno
  Serial.println("LED encendido"); // Muestra un mensaje en el monitor serial
  delay(1000);             // Espera 1 segundo (1000 milisegundos)
  
  digitalWrite(2, LOW);    // Apaga el LED interno
  Serial.println("LED apagado"); // Muestra un mensaje en el monitor serial
  delay(1000);             // Espera 1 segundo
}
