const int sensorPin = A0;      // Pin conectado a OUT del ACS712
const float VCC = 5.0;         // Voltaje del Arduino
const int mVperAmp = 185;      // Para el modelo de 5A

void setup() {
  Serial.begin(9600);          
  Serial.println("Corriente (A)"); // Etiqueta para el Serial Plotter
}

void loop() {
  int sensorValue = analogRead(sensorPin);
  float voltage = (sensorValue / 1023.0) * VCC;
  float current = (voltage - 2.5) * 1000 / mVperAmp;

  Serial.println(current, 3);  // Envío de datos al Plotter

  delay(100);                  // Intervalo de actualización
}
