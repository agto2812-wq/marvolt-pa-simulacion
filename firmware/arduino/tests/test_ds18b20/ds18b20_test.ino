/**
 * ============================================================
 *  TEST — Sensor de Temperatura DS18B20
 *  Hardware: Arduino MEGA 2560 (ELEGOO MEGA R3)
 *  Extraído de: arduinomega01.ino (Sistema Undimotriz v1.0)
 *  Propósito: Probar el DS18B20 de forma aislada antes de
 *             integrarlo al sistema completo.
 * ============================================================
 *
 *  CONEXIÓN (idéntica al sistema principal):
 *    DS18B20 DATA → Pin 26 (1-Wire)
 *    DS18B20 VCC  → 5V  (o 3.3V en modo parásito)
 *    DS18B20 GND  → GND
 *    Pull-up      → Resistencia 4.7 kΩ entre DATA y VCC
 *                   ¡Sin ella el bus 1-Wire no funciona!
 *
 *  ARQUITECTURA DE TIMING (misma lógica que el sistema principal):
 *    Cada 500 ms  → Solicitar conversión (no bloqueante)
 *    Tras 400 ms  → Leer resultado cuando ya está listo
 *    Cada 1000 ms → Imprimir estado completo por Serial
 *
 *  En el sistema principal estos tiempos los marca el pulso SYNC
 *  del Arduino UNO (1 tick = 20 ms). Aquí los reemplazamos con
 *  millis() para poder correr el test de forma independiente,
 *  sin necesidad de ningún otro hardware conectado.
 *
 *  LIBRERÍAS NECESARIAS (mismas que el sistema completo):
 *    OneWire          — Bus 1-Wire
 *    DallasTemperature — Protocolo DS18B20
 * ============================================================
 */

#include <OneWire.h>
#include <DallasTemperature.h>

// ============================================================
// PINES — Igual que en arduinomega01.ino
// ============================================================
#define PIN_DS18B20  26   // 1-Wire

// ============================================================
// PARÁMETROS DE TEMPORIZACIÓN
// ============================================================
// 375 ms es el tiempo real a 11 bits. Usamos 400 ms como margen
// de seguridad, igual que DS_CONVERSION_MS en el sistema principal.
#define DS_CONVERSION_MS  400UL   // Tiempo de conversión + margen
#define T_REQUEST_MS      500UL   // Intervalo entre solicitudes
#define T_PRINT_MS       1000UL   // Intervalo de impresión por Serial

// ============================================================
// DS18B20 — MÁQUINA DE ESTADOS (idéntica al sistema principal)
// ============================================================
// DS_IDLE:      Sensor en reposo, listo para nueva solicitud.
// DS_REQUESTED: Conversión en curso. No leer antes de 400 ms.
enum Ds18b20State { DS_IDLE, DS_REQUESTED };

Ds18b20State ds18State     = DS_IDLE;
uint32_t     dsRequestTime = 0;   // millis() en el momento de pedir conversión

// ============================================================
// VARIABLES GLOBALES
// ============================================================
float    temperatura  = -99.0f;   // Centinela: sin lectura válida aún
uint32_t tLastRequest =  0;       // Control de intervalo de solicitud
uint32_t tLastPrint   =  0;       // Control de intervalo de impresión
uint16_t cntOk        =  0;       // Lecturas válidas acumuladas
uint16_t cntError     =  0;       // Lecturas con error acumuladas

// ============================================================
// BUS 1-WIRE Y SENSOR
// ============================================================
OneWire           oneWire(PIN_DS18B20);
DallasTemperature dsSensor(&oneWire);

// ============================================================
// FUNCIÓN: requestDs18b20() — Solicitar conversión
// ============================================================
/**
 * Equivale directamente a requestDs18b20() de arduinomega01.ino.
 *
 * dsSensor.requestTemperatures() dispara la conversión en el
 * sensor y retorna INMEDIATAMENTE porque setWaitForConversion(false)
 * desactiva el delay() interno de la librería. La conversión sigue
 * ocurriendo en hardware; solo dejamos de bloquearnos esperándola.
 *
 * La guarda (ds18State != DS_IDLE) previene que dos solicitudes
 * se solapen, lo que produciría lecturas intermedias erróneas.
 */
void requestDs18b20() {
  if (ds18State != DS_IDLE) return;  // Conversión ya en curso, ignorar

  dsSensor.requestTemperatures();    // Disparo no bloqueante
  dsRequestTime = millis();
  ds18State     = DS_REQUESTED;
}

// ============================================================
// FUNCIÓN: readDs18b20() — Leer resultado
// ============================================================
/**
 * Equivale directamente a readDs18b20() de arduinomega01.ino.
 *
 * La doble guarda es la clave del diseño no bloqueante:
 *   1) Si no hay conversión en curso, no hay nada que leer.
 *   2) Si la conversión no terminó aún, esperar sin bloquearse.
 *      El loop() seguirá corriendo y atendiendo otras cosas.
 *
 * Validación del dato recibido:
 *   DEVICE_DISCONNECTED_C = -127.0  → sensor físicamente ausente.
 *   < -55°C o > +125°C             → dato corrupto (CRC falló
 *                                     o hubo ruido en el bus).
 *   Dentro del rango               → dato válido, actualizar temperatura.
 */
void readDs18b20() {
  if (ds18State != DS_REQUESTED) return;
  if (millis() - dsRequestTime < DS_CONVERSION_MS) return;  // Aún convirtiendo

  float t = dsSensor.getTempCByIndex(0);

  if (t == DEVICE_DISCONNECTED_C || t < -55.0f || t > 125.0f) {
    cntError++;
    Serial.print(F("[ERROR] Sensor no detectado o dato fuera de rango."));
    Serial.print(F(" Valor raw recibido: "));
    Serial.print(t, 2);
    Serial.println(F(" C"));
    Serial.println(F("        Verifica cableado y resistencia pull-up 4.7k."));
  } else {
    temperatura = t;
    cntOk++;
  }

  ds18State = DS_IDLE;  // Ciclo completo. Listo para la próxima solicitud.
}

// ============================================================
// FUNCIÓN: imprimirEstado() — Salida por Serial Monitor / Plotter
// ============================================================
/**
 * Imprime en formato "Etiqueta:Valor" compatible con el Serial
 * Plotter del IDE de Arduino, igual que enviarSerial() en el
 * sistema principal. Así puedes verificar visualmente la curva
 * de temperatura en tiempo real.
 */
void imprimirEstado() {
  // Formato compatible con Serial Plotter (etiqueta:valor,...)
  Serial.print(F("Temp_C:"));
  if (temperatura > -50.0f) {
    Serial.print(temperatura, 2);
  } else {
    Serial.print(F("---"));   // Centinela: todavía sin lectura válida
  }

  // Información de diagnóstico adicional (solo en Serial Monitor)
  Serial.print(F("  |  Estado:"));
  Serial.print(ds18State == DS_IDLE ? F("IDLE") : F("CONVIRTIENDO"));
  Serial.print(F("  OK:"));
  Serial.print(cntOk);
  Serial.print(F("  ERR:"));
  Serial.println(cntError);
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);   // Misma velocidad que el sistema principal

  // Inicializar el sensor
  dsSensor.begin();

  // 11 bits → resolución de 0.125°C, conversión en ~375 ms.
  // Mismo ajuste que en el sistema completo: mejor compromiso
  // entre velocidad de conversión y precisión.
  dsSensor.setResolution(11);

  // CRÍTICO: desactiva el delay() interno de la librería.
  // Sin esto, getTempCByIndex() bloquearía ~375 ms cada llamada,
  // rompiendo el diseño no bloqueante.
  dsSensor.setWaitForConversion(false);

  // ── Diagnóstico inicial ──────────────────────────────────
  Serial.println(F("============================================"));
  Serial.println(F("  TEST DS18B20 — Arduino MEGA 2560"));
  Serial.println(F("  Base: arduinomega01.ino (Sistema Undimotriz)"));
  Serial.println(F("============================================"));

  uint8_t n = dsSensor.getDeviceCount();
  Serial.print(F("  Sensores en bus 1-Wire (pin 26): "));
  Serial.println(n);

  if (n == 0) {
    // Guía de diagnóstico cuando no se detecta nada
    Serial.println(F("  !! Ningún sensor detectado. Causas comunes:"));
    Serial.println(F("     1) Cable DATA no conectado al pin 26."));
    Serial.println(F("     2) Falta la resistencia pull-up de 4.7 kΩ."));
    Serial.println(F("     3) VCC o GND del sensor sin conexión."));
    Serial.println(F("     4) Sensor dañado."));
  } else {
    // Mostrar dirección ROM de cada sensor detectado.
    // La dirección ROM es única por sensor de fábrica; sirve para
    // confirmar que el sensor es auténtico y para identificarlo
    // individualmente cuando hay varios en el mismo bus.
    DeviceAddress addr;
    for (uint8_t i = 0; i < n; i++) {
      if (dsSensor.getAddress(addr, i)) {
        Serial.print(F("  Sensor ["));
        Serial.print(i);
        Serial.print(F("] ROM: "));
        for (uint8_t b = 0; b < 8; b++) {
          if (addr[b] < 0x10) Serial.print(F("0"));  // Cero inicial para alinear
          Serial.print(addr[b], HEX);
          if (b < 7) Serial.print(F(":"));
        }
        Serial.println();
      }
    }
  }

  Serial.println(F("  Resolución: 11 bits (0.125°C) — no bloqueante"));
  Serial.println(F("  Mide cada 500 ms | Imprime cada 1000 ms"));
  Serial.println(F("============================================"));

  // Arrancar el primer ciclo inmediatamente sin esperar 500 ms
  requestDs18b20();
  tLastRequest = millis();
  tLastPrint   = millis();
}

// ============================================================
// LOOP
// ============================================================
/**
 * El loop reemplaza el scheduler de ticks SYNC del sistema principal.
 * En vez de contar pulsos del UNO, usamos millis() para disparar
 * cada tarea en su ventana de tiempo correspondiente.
 *
 * El flujo temporal resultante es:
 *
 *   t = 0 ms   → requestDs18b20() (ya disparado en setup)
 *   t = 400 ms → readDs18b20() lee el resultado (primer dato)
 *   t = 500 ms → requestDs18b20() segunda conversión
 *   t = 900 ms → readDs18b20() lee segundo resultado
 *   t = 1000 ms→ imprimirEstado()
 *   t = 1000 ms→ requestDs18b20() tercera conversión
 *   ...y así indefinidamente.
 *
 * Nótese que readDs18b20() se llama en CADA iteración del loop,
 * lo mismo que en el sistema principal. Eso es seguro porque la
 * función tiene sus propias guardas internas y solo actúa cuando
 * corresponde. Llamarla frecuentemente minimiza la latencia entre
 * el fin de la conversión y la actualización del valor.
 */
void loop() {
  uint32_t ahora = millis();

  // ── Solicitar nueva conversión cada T_REQUEST_MS ──────────
  if (ahora - tLastRequest >= T_REQUEST_MS) {
    tLastRequest = ahora;
    requestDs18b20();
  }

  // ── Intentar leer resultado — se auto-regula internamente ─
  // Barato de llamar: retorna inmediatamente si aún no es el momento.
  readDs18b20();

  // ── Imprimir estado cada T_PRINT_MS ──────────────────────
  if (ahora - tLastPrint >= T_PRINT_MS) {
    tLastPrint = ahora;
    imprimirEstado();
  }
}
