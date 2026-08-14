// Sensor Ultrasónico HC-SR04 con Arduino Mega
// Configuración para Serial Plotter

const int trigPin = 7;    // Pin Trigger conectado al pin 7
const int echoPin = 6;    // Pin Echo conectado al pin 6

long duracion;
int distancia;

void setup() {
  // Configuración de pines
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Inicialización de comunicación serial
  Serial.begin(9600);
}

void loop() {
  // Limpiar el pin Trigger
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Enviar pulso de 10 microsegundos
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Leer el tiempo de retorno del Echo
  duracion = pulseIn(echoPin, HIGH);
  
  // Calcular distancia en centímetros
  distancia = duracion * 0.034 / 2;
  
  // Enviar datos con etiqueta para Serial Plotter
  if (distancia >= 2 && distancia <= 400) {
    Serial.print("Distancia_cm:");
    Serial.println(distancia);
  } else {
    Serial.print("Distancia_cm:");
    Serial.println(0); // Valor por defecto para lecturas inválidas
  }
  
  // Pausa entre mediciones
  delay(100);
}
