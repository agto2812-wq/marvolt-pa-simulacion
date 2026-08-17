#define PIN_TEST 2

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TEST, INPUT_PULLUP);  // Activa resistencia pull-up interna (~20-50k)
}

void loop() {
  int v = digitalRead(PIN_TEST);
  Serial.println(v == LOW ? "CONECTADO  (continuidad OK)" : "ABIERTO    (sin continuidad)");
  delay(200);
}