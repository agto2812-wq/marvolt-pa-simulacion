const int sensorPin = A0;
int rawValue = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  rawValue = analogRead(sensorPin);
  Serial.println(rawValue); // Enviaba la señal cruda para el Serial Plotter
  delay(2); // Pequeño retardo para estabilizar la gráfica
}
