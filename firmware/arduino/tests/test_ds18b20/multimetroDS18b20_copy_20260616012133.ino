void setup() {
  Serial.begin(115200);
}
void loop() {
  float v = analogRead(A0) * (5.0 / 1023.0);
  Serial.print("DATA = "); Serial.print(v, 2); Serial.println(" V");
  delay(300);
}