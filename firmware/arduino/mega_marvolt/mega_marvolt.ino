/**
 * ============================================================
 *  TEST INTEGRADO DE SENSORES — Sistema Undimotriz
 *  Hardware: Arduino MEGA 2560 (ELEGOO MEGA R3)
 *  Propósito: Probar TODOS los sensores juntos, SIN el Arduino
 *             UNO (sin pulso SYNC) y con UN SOLO OLED conectado
 *             directo por I2C (sin TCA9548A / multiplexor).
 *
 *  Reemplaza el scheduler de ticks (que depende del SYNC) por
 *  temporizadores millis(), igual que en ds18b20_test.ino.
 *  Cuando este test funcione bien, esta misma lógica de lectura
 *  se vuelve a integrar al sistema principal con el UNO.
 * ============================================================
 *
 *  CONEXIONES:
 *    ACS712 (corriente)   → OUT: A0   | VCC: 5V | GND: GND
 *    ZMPT101B (voltaje)   → OUT: A1   | VCC: 5V | GND: GND
 *    HC-SR04 (distancia)  → TRIG: 22  | ECHO: 18 | VCC: 5V | GND: GND
 *    DS18B20 (temp)       → DATA: 26  | VCC: 5V | GND: GND
 *                            + resistencia pull-up 4.7kΩ entre DATA y VCC
 *    OLED SSD1306 128x64  → SDA: 20   | SCL: 21  | VCC: 5V* | GND: GND
 *                            (*revisa tu módulo: la mayoría acepta 5V,
 *                             pero algunos son solo 3.3V)
 *
 *  LIBRERÍAS NECESARIAS:
 *    Wire, Adafruit_GFX, Adafruit_SSD1306, OneWire, DallasTemperature
 * ============================================================
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ============================================================
// PINES
// ============================================================
#define PIN_CORRIENTE  A0
#define PIN_VOLTAJE    A1
#define TRIG_SR04      22
#define ECHO_SR04      18
#define PIN_DS18B20    26

// ============================================================
// OLED — UN SOLO DISPLAY, I2C DIRECTO (sin TCA9548A)
// ============================================================
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT   64
#define OLED_ADDR      0x3C   // Si no detecta, probar 0x3D
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
bool oledOk = false;

// ============================================================
// DS18B20 — máquina de estados no bloqueante (igual que el sistema principal)
// ============================================================
OneWire           oneWire(PIN_DS18B20);
DallasTemperature dsSensor(&oneWire);
enum Ds18State { DS_IDLE, DS_REQUESTED };
Ds18State ds18State     = DS_IDLE;
uint32_t  dsRequestTime = 0;
#define DS_CONVERSION_MS 400UL

// ============================================================
// PARÁMETROS DE SENSORES (idénticos a arduinomega01.ino)
// ============================================================
#define ACS712_VREF   2.5f
#define ACS712_mVA    0.185f
#define ACS712_MAX    5.0f
#define ZMPT_SCALE    100.0f
#define ZMPT_MAX      250.0f
#define EMA_ACS       0.15f
#define EMA_ZMPT      0.15f
#define SR04_TIMEOUT_US 30000UL

// ============================================================
// VARIABLES DE LECTURA
// ============================================================
float corriente   = 0.0f;
float voltaje     = 0.0f;
float potencia    = 0.0f;
float distancia   = -1.0f;    // Centinela: -1 = aún sin eco válido
float temperatura = -99.0f;   // Centinela: sin lectura válida aún

uint16_t dsOk    = 0;
uint16_t dsError = 0;

uint16_t sr04Ok           = 0;
uint16_t sr04Miss         = 0;
uint8_t  sr04MissSeguidos = 0;
#define SR04_MAX_MISS_SEGUIDOS 10   // ~1 s sin eco (10 × 100 ms) → invalidar

// ============================================================
// TEMPORIZADORES (reemplazan el tick SYNC)
// ============================================================
uint32_t tLastAnalog = 0;
uint32_t tLastSR04   = 0;
uint32_t tLastDsReq  = 0;
uint32_t tLastOled   = 0;
uint32_t tLastSerial = 0;

#define T_ANALOG_MS   20
#define T_SR04_MS    100
#define T_DSREQ_MS   500
#define T_OLED_MS    300
#define T_SERIAL_MS  200

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(TRIG_SR04, OUTPUT);
  pinMode(ECHO_SR04, INPUT);
  digitalWrite(TRIG_SR04, LOW);

  dsSensor.begin();
  dsSensor.setResolution(11);
  dsSensor.setWaitForConversion(false);  // No bloqueante

  Serial.println(F("============================================"));
  Serial.println(F("  TEST INTEGRADO DE SENSORES — MEGA 2560"));
  Serial.println(F("  ACS712=A0  ZMPT101B=A1  SR04=22/18  DS18B20=26"));
  Serial.println(F("============================================"));

  // --- DS18B20: diagnóstico inicial ---
  uint8_t n = dsSensor.getDeviceCount();
  Serial.print(F("  Sensores DS18B20 detectados: "));
  Serial.println(n);
  if (n == 0) {
    Serial.println(F("  !! Revisa: pull-up 4.7k, cable DATA en pin 26,"));
    Serial.println(F("     VCC/GND firmes (evitar cocodrilos sueltos en"));
    Serial.println(F("     las patas TO-92, mejor usar protoboard directo)."));
  }

  // --- OLED: inicialización ---
  if (oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    oledOk = true;
    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(0, 0);
    oled.println(F("TEST SENSORES"));
    oled.println(F("Undimotriz - MEGA"));
    oled.println(F("Iniciando..."));
    oled.display();
    delay(800);
  } else {
    Serial.println(F("  !! OLED no detectado en 0x3C. Probar 0x3D"));
    Serial.println(F("     o revisar SDA(20)/SCL(21)."));
  }
}

// ============================================================
// LECTURA DE CORRIENTE (ACS712 5A)
// ============================================================
void leerCorriente() {
  float vSensor = (analogRead(PIN_CORRIENTE) * 5.0f) / 1024.0f;
  float c = fabsf((vSensor - ACS712_VREF) / ACS712_mVA);
  if (c > ACS712_MAX * 1.1f) return;  // Rechazo de picos imposibles
  corriente = EMA_ACS * c + (1.0f - EMA_ACS) * corriente;
}

// ============================================================
// LECTURA DE VOLTAJE (ZMPT101B — aproximación de pico)
// ============================================================
void leerVoltaje() {
  float vSensor = (analogRead(PIN_VOLTAJE) * 5.0f) / 1024.0f;
  float vAC = fabsf(vSensor - 2.5f) * ZMPT_SCALE;
  if (vAC > ZMPT_MAX) return;
  voltaje = EMA_ZMPT * vAC + (1.0f - EMA_ZMPT) * voltaje;
}

// ============================================================
// HC-SR04 — medición (modo simple con pulseIn, con timeout)
// ============================================================
void medirSR04() {
  digitalWrite(TRIG_SR04, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_SR04, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_SR04, LOW);

  long duracion = pulseIn(ECHO_SR04, HIGH, SR04_TIMEOUT_US);  // 0 si timeout
  bool valido = false;

  if (duracion > 0) {
    float d = duracion * 0.0343f / 2.0f;
    if (d > 2.0f && d < 400.0f) {
      // Si veníamos de "sin eco" (centinela -1), arrancar el filtro
      // limpio en d en vez de mezclarlo con el -1 anterior.
      if (distancia < 0.0f) distancia = d;
      distancia = 0.3f * d + 0.7f * distancia;  // Filtro suave
      valido = true;
    }
  }

  if (valido) {
    sr04Ok++;
    sr04MissSeguidos = 0;
  } else {
    sr04Miss++;
    if (sr04MissSeguidos < 255) sr04MissSeguidos++;
    // Se dispara UNA vez al cruzar el umbral (no en cada fallo posterior)
    if (sr04MissSeguidos == SR04_MAX_MISS_SEGUIDOS) {
      distancia = -1.0f;  // Invalida: ya no se muestra un número viejo
      Serial.println(F("[HC-SR04] Sin eco valido por 1s+. Revisar TRIG(22)/ECHO(18) y obstruccion frontal."));
    }
  }
}

// ============================================================
// DS18B20 — solicitar / leer (no bloqueante)
// ============================================================
void requestDs18b20() {
  if (ds18State != DS_IDLE) return;
  dsSensor.requestTemperatures();
  dsRequestTime = millis();
  ds18State = DS_REQUESTED;
}

void readDs18b20() {
  if (ds18State != DS_REQUESTED) return;
  if (millis() - dsRequestTime < DS_CONVERSION_MS) return;

  float t = dsSensor.getTempCByIndex(0);
  if (t != DEVICE_DISCONNECTED_C && t > -55.0f && t < 125.0f) {
    temperatura = t;
    dsOk++;
  } else {
    dsError++;
    Serial.println(F("[DS18B20] -127C / desconectado — revisar cableado"));
  }
  ds18State = DS_IDLE;
}

// ============================================================
// OLED — UNA sola pantalla con resumen de todos los sensores
// ============================================================
void actualizarOled() {
  if (!oledOk) return;
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);

  oled.setCursor(0, 0);
  oled.println(F("TEST SENSORES MEGA"));

  oled.setCursor(0, 14);
  oled.print(F("I:")); oled.print(corriente, 2); oled.print(F("A "));
  oled.print(F("V:")); oled.print(voltaje, 0); oled.println(F("V"));

  oled.setCursor(0, 26);
  oled.print(F("P:")); oled.print(potencia, 1); oled.println(F("W"));

  oled.setCursor(0, 38);
  oled.print(F("Dist:"));
  if (distancia >= 0.0f) { oled.print(distancia, 1); oled.println(F("cm")); }
  else                    { oled.println(F("S/ECO")); }

  oled.setCursor(0, 50);
  oled.print(F("Temp:"));
  if (temperatura > -50.0f) oled.print(temperatura, 1);
  else                       oled.print(F("---"));
  oled.print(F("C "));
  oled.print(F("OK:")); oled.print(dsOk); oled.print(F(" E:")); oled.print(dsError);

  oled.display();
}

// ============================================================
// SALIDA SERIAL (compatible con Serial Plotter)
// ============================================================
void enviarSerial() {
  Serial.print(F("Corriente:"));   Serial.print(corriente, 3);   Serial.print(F(","));
  Serial.print(F("Voltaje:"));     Serial.print(voltaje, 2);     Serial.print(F(","));
  Serial.print(F("Potencia:"));    Serial.print(potencia, 3);    Serial.print(F(","));
  Serial.print(F("Distancia_cm:"));Serial.print(distancia, 1);   Serial.print(F(","));
  Serial.print(F("Temp_C:"));      Serial.println(temperatura, 1);
}

// ============================================================
// LOOP — temporizado con millis(), sin SYNC
// ============================================================
void loop() {
  uint32_t ahora = millis();

  // Corriente + voltaje cada 20 ms
  if (ahora - tLastAnalog >= T_ANALOG_MS) {
    tLastAnalog = ahora;
    leerCorriente();
    leerVoltaje();
    potencia = corriente * voltaje;
  }

  // HC-SR04 cada 100 ms
  if (ahora - tLastSR04 >= T_SR04_MS) {
    tLastSR04 = ahora;
    medirSR04();
  }

  // DS18B20: solicitar cada 500 ms, leer cuando esté listo (~400 ms después)
  if (ahora - tLastDsReq >= T_DSREQ_MS) {
    tLastDsReq = ahora;
    requestDs18b20();
  }
  readDs18b20();

  // OLED cada 300 ms
  if (ahora - tLastOled >= T_OLED_MS) {
    tLastOled = ahora;
    actualizarOled();
  }

  // Serial Plotter cada 200 ms
  if (ahora - tLastSerial >= T_SERIAL_MS) {
    tLastSerial = ahora;
    enviarSerial();
  }
}
