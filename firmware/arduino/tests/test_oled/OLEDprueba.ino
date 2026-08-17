#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // Ancho de la pantalla OLED en píxeles
#define SCREEN_HEIGHT 64 // Alto de la pantalla OLED en píxeles

// Declaración para la pantalla OLED SSD1306 conectada por I2C
#define OLED_RESET     -1 // Pin de reset (-1 si compartes el reset del Arduino)
#define SCREEN_ADDRESS 0x3C // Dirección I2C típica: 0x3C o 0x3D
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  
  // Inicializa el bus I2C (SDA = pin 20, SCL = pin 21 en Arduino Mega)
  Wire.begin();
  
  // Inicializa la pantalla OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Error en la inicialización del SSD1306"));
    for(;;); // Bucle infinito si falla
  }
  
  // Limpia el buffer
  display.clearDisplay();
  
  // Muestra texto inicial
  mostrarTexto();
  
  // Espera un momento para ver el texto inicial
  delay(2000);
}

void loop() {
  // Animación simple
  for(int i = 0; i < 4; i++) {
    display.clearDisplay();
    
    switch(i) {
      case 0:
        dibujarCirculo();
        break;
      case 1:
        dibujarRectangulo();
        break;
      case 2:
        dibujarTriangulo();
        break;
      case 3:
        mostrarTexto();
        break;
    }
    
    delay(1500);
  }
}

void mostrarTexto() {
  display.clearDisplay();
  
  // Configuración del texto
  display.setTextSize(1);             // Tamaño normal 1:1
  display.setTextColor(SSD1306_WHITE); // Color blanco
  display.setCursor(0, 0);            // Posición superior izquierda
  display.println(F("OLED 0.96\" I2C"));
  
  display.setTextSize(2);             // Tamaño más grande
  display.setCursor(0, 16);
  display.println(F("Arduino"));
  display.setCursor(0, 32);
  display.println(F("MEGA 2560"));
  
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.println(F("Test de pantalla"));
  
  display.display(); // Actualiza la pantalla
}

void dibujarCirculo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 0);
  display.println(F("CIRCULO"));
  
  display.drawCircle(display.width()/2, display.height()/2, 20, SSD1306_WHITE);
  display.display();
}

void dibujarRectangulo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 0);
  display.println(F("RECTANGULO"));
  
  display.drawRect(34, 20, 60, 30, SSD1306_WHITE);
  display.display();
}

void dibujarTriangulo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 0);
  display.println(F("TRIANGULO"));
  
  display.drawTriangle(
    display.width()/2, 20,
    display.width()/2 - 20, 50,
    display.width()/2 + 20, 50,
    SSD1306_WHITE);
  display.display();
}