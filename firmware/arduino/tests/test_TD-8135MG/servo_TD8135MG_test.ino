/**
 * ============================================================
 *  TEST INDIVIDUAL — Servo CR TD-8135MG (360° rotación continua)
 *  Extraído de: servo_encoder_oled_v3_final_SYNC02.ino
 *  Hardware  : Arduino UNO
 *  Pin señal : D9  (igual que en el código original)
 *  Alimentación: EXTERNA 5 V o 6 V — NO usar el 5 V del Arduino
 *               (el TD-8135MG puede consumir hasta 3 400 mA en stall)
 * ============================================================
 *
 *  ESPECIFICACIONES TD-8135MG (verificadas):
 *    Rango PWM  : 500 – 2 500 µs
 *    STOP       : 1 500 µs  (deadband ±5 µs)
 *    < 1 500 µs → sentido antihorario (más lento conforme se acerca a 1 500)
 *    > 1 500 µs → sentido horario     (más lento conforme se acerca a 1 500)
 *    Tensión    : 4.8 V – 7.2 V
 *    Par máx    : 32.7 – 35.2 kg·cm
 *
 *  DIFERENCIA CLAVE CON EL CÓDIGO ORIGINAL:
 *    El código completo usaba servo.write(grados) con rango 0–180.
 *    En modo CR, 90° ≈ 1 500 µs (STOP), lo que funciona, pero los
 *    extremos quedan en ~544 µs (0°) y ~2 400 µs (180°), sin cubrir
 *    los 500 µs y 2 500 µs del TD-8135MG.
 *    Aquí usamos servo.writeMicroseconds() con attach extendido para
 *    aprovechar el rango completo del servo.
 *
 *  COMANDOS SERIAL (115 200 baud, terminar con Enter):
 *    pwm:<500-2500>   Pulso directo en µs,  ej: pwm:1700
 *    stop             Detener servo (1 500 µs)
 *    secuencia        Ejecutar secuencia de prueba automática
 * ============================================================
 */

#include <Servo.h>

// ── Pin ────────────────────────────────────────────────────
#define SERVO_PIN   9

// ── Pulsos de referencia TD-8135MG (µs) ───────────────────
//   Ajusta PWM_STOP si el servo no queda completamente quieto:
//   puede variar ±10–30 µs entre unidades individuales.
const int PWM_STOP      = 1500;   // Punto muerto  → detenido
const int PWM_CW_SLOW   = 1600;   // Horario lento
const int PWM_CW_MED    = 1800;   // Horario medio
const int PWM_CW_FAST   = 2000;   // Horario rápido
const int PWM_CW_MAX    = 2500;   // Horario máximo
const int PWM_CCW_SLOW  = 1400;   // Antihorario lento
const int PWM_CCW_MED   = 1200;   // Antihorario medio
const int PWM_CCW_FAST  = 1000;   // Antihorario rápido
const int PWM_CCW_MAX   =  500;   // Antihorario máximo

Servo servo;

// Buffer de comandos serial (igual que en código original)
char    cmdBuf[32];
uint8_t cmdLen = 0;

// ── Prototipos ─────────────────────────────────────────────
void setServo(int pulseUs);
void secuenciaPrueba();
void procesarComandoSerial();


// ===========================================================
//  SETUP
// ===========================================================
void setup() {
  Serial.begin(115200);

  // attach con rango extendido para cubrir los 500–2500 µs del TD-8135MG.
  // Sin esto, el Servo.h limita internamente a ~544–2400 µs.
  servo.attach(SERVO_PIN, 500, 2500);
  setServo(PWM_STOP);   // Asegurarse de que arranca detenido

  Serial.println(F("============================================"));
  Serial.println(F("  TEST Servo CR TD-8135MG  — Listo"));
  Serial.println(F("============================================"));
  Serial.println(F("Comandos disponibles:"));
  Serial.println(F("  pwm:<500-2500>   Pulso directo en us"));
  Serial.println(F("  stop             Detener (1500 us)"));
  Serial.println(F("  secuencia        Prueba automatica completa"));
  Serial.println(F("--------------------------------------------"));
  Serial.println(F("Pulsos de referencia:"));
  Serial.println(F("   500 us  antihorario MAX"));
  Serial.println(F("  1000 us  antihorario rapido"));
  Serial.println(F("  1200 us  antihorario medio"));
  Serial.println(F("  1400 us  antihorario lento"));
  Serial.println(F("  1500 us  STOP (punto muerto)"));
  Serial.println(F("  1600 us  horario lento"));
  Serial.println(F("  1800 us  horario medio"));
  Serial.println(F("  2000 us  horario rapido"));
  Serial.println(F("  2500 us  horario MAX"));
  Serial.println(F("============================================"));
}


// ===========================================================
//  LOOP — solo escucha el puerto serial, sin scheduler
// ===========================================================
void loop() {
  procesarComandoSerial();
}


// ===========================================================
//  FUNCIONES
// ===========================================================

/**
 * Aplica un pulso en µs con constrain de seguridad y lo reporta.
 */
void setServo(int pulseUs) {
  pulseUs = constrain(pulseUs, 500, 2500);
  servo.writeMicroseconds(pulseUs);
  Serial.print(F(">> Servo: "));
  Serial.print(pulseUs);
  Serial.println(F(" us"));
}

/**
 * Secuencia de prueba para caracterizar el servo paso a paso:
 *   STOP → horario (lento → medio → rápido → MAX)
 *        → STOP
 *        → antihorario (lento → medio → rápido → MAX)
 *        → STOP
 *
 * Durante cada paso, observar:
 *   - Si el servo realmente se detiene en 1 500 µs
 *   - Suavidad de arranque y frenado
 *   - Temperatura y ruido mecánico
 */
void secuenciaPrueba() {
  Serial.println(F("\n--- INICIO SECUENCIA DE PRUEBA ---"));

  Serial.println(F("[1] STOP (1500 us) — 2 s"));
  setServo(PWM_STOP);
  delay(2000);

  Serial.println(F("[2] Horario LENTO (1600 us) — 3 s"));
  setServo(PWM_CW_SLOW);
  delay(3000);

  Serial.println(F("[3] Horario MEDIO (1800 us) — 3 s"));
  setServo(PWM_CW_MED);
  delay(3000);

  Serial.println(F("[4] Horario RAPIDO (2000 us) — 3 s"));
  setServo(PWM_CW_FAST);
  delay(3000);

  Serial.println(F("[5] Horario MAX (2500 us) — 2 s"));
  setServo(PWM_CW_MAX);
  delay(2000);

  Serial.println(F("[6] STOP — 2 s (separacion de direcciones)"));
  setServo(PWM_STOP);
  delay(2000);

  Serial.println(F("[7] Antihorario LENTO (1400 us) — 3 s"));
  setServo(PWM_CCW_SLOW);
  delay(3000);

  Serial.println(F("[8] Antihorario MEDIO (1200 us) — 3 s"));
  setServo(PWM_CCW_MED);
  delay(3000);

  Serial.println(F("[9] Antihorario RAPIDO (1000 us) — 3 s"));
  setServo(PWM_CCW_FAST);
  delay(3000);

  Serial.println(F("[10] Antihorario MAX (500 us) — 2 s"));
  setServo(PWM_CCW_MAX);
  delay(2000);

  Serial.println(F("[11] STOP final (1500 us)"));
  setServo(PWM_STOP);

  Serial.println(F("--- FIN SECUENCIA ---\n"));
  Serial.println(F("Si el servo no quedo quieto en [1] o [11],"));
  Serial.println(F("ajusta PWM_STOP +/- 10 us hasta que se detenga."));
}

/**
 * Procesamiento no bloqueante de comandos serial.
 * Lógica idéntica a la del código original SYNC02.
 */
void procesarComandoSerial() {
  while (Serial.available()) {
    char c = (char)Serial.read();

    if (c == '\n' || c == '\r') {
      if (cmdLen == 0) continue;
      cmdBuf[cmdLen] = '\0';

      if (strncmp(cmdBuf, "pwm:", 4) == 0) {
        setServo(atoi(cmdBuf + 4));

      } else if (strcmp(cmdBuf, "stop") == 0) {
        setServo(PWM_STOP);
        Serial.println(F(">> Servo detenido."));

      } else if (strcmp(cmdBuf, "secuencia") == 0) {
        secuenciaPrueba();

      } else {
        Serial.print(F(">> Comando no reconocido: "));
        Serial.println(cmdBuf);
      }

      cmdLen = 0;

    } else if (cmdLen < sizeof(cmdBuf) - 1) {
      cmdBuf[cmdLen++] = c;
    }
  }
}
