#include <M5Unified.h>

// Configuración de suavizado para evitar saltos en la medición
const int NUM_LECTURAS = 10; 
int lecturas[NUM_LECTURAS];      
int lecturaIndex = 0;              
long total = 0;                  
int promedioNivel = 0;
int numLecturas = 0;  // Contador de lecturas válidas

// Intervalo de actualización de pantalla
const unsigned long INTERVALO_REFRESCO = 1000; 

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  Serial.begin(115200);
  delay(100);

  // Configuración de pantalla
  M5.Display.setRotation(1);
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextDatum(MC_DATUM); // Middle-Center
  
  // Inicializar el buffer de lecturas
  for (int i = 0; i < NUM_LECTURAS; i++) {
    lecturas[i] = 0;
  }

  // Primera lectura para inicializar
  int primerNivel = M5.Power.getBatteryLevel();
  for (int i = 0; i < NUM_LECTURAS; i++) {
    lecturas[i] = primerNivel;
    total += primerNivel;
  }
  numLecturas = NUM_LECTURAS;
  promedioNivel = primerNivel;
}

void loop() {
  M5.update();

  // --- 1. OBTENCIÓN DE DATOS ---
  int nivelActual = M5.Power.getBatteryLevel();
  int voltajeMv = M5.Power.getBatteryVoltage();
  bool isCharging = M5.Power.isCharging();

  // --- 2. PROMEDIO MÓVIL ---
  total = total - lecturas[lecturaIndex];
  lecturas[lecturaIndex] = nivelActual;
  total = total + lecturas[lecturaIndex];
  lecturaIndex = (lecturaIndex + 1) % NUM_LECTURAS;
  
  promedioNivel = total / NUM_LECTURAS;

  // --- 3. DIBUJADO EN PANTALLA ---
  M5.Display.fillScreen(TFT_BLACK);

  int centerX = M5.Display.width() / 2;
  int centerY = M5.Display.height() / 2;

  // Color según nivel de batería
  uint16_t colorBateria;
  if (promedioNivel > 60) {
    colorBateria = TFT_GREEN;
  } else if (promedioNivel > 20) {
    colorBateria = TFT_ORANGE;
  } else {
    colorBateria = TFT_RED;
  }

  // A) MOSTRAR PORCENTAJE GRANDE
  M5.Display.setTextColor(colorBateria, TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.setFont(&fonts::FreeSerifBold24pt7b);  // Fuente grande compatible
  
  String porcentajeStr = String(promedioNivel) + "%";
  M5.Display.drawString(porcentajeStr, centerX, centerY);

  // B) MOSTRAR VOLTAJE
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setFont(&fonts::Font2);
  String infoVoltaje = String(voltajeMv) + " mV";
  M5.Display.drawString(infoVoltaje, centerX, centerY + 50);

  // C) ESTADO DE CARGA
  M5.Display.setFont(&fonts::Font2);
  if (isCharging) {
    M5.Display.setTextColor(TFT_CYAN, TFT_BLACK);
    M5.Display.drawString("CARGANDO", centerX, 20);
  } else {
    M5.Display.setTextColor(TFT_DARKGREY, TFT_BLACK);
    M5.Display.drawString("BATERIA", centerX, 20);
  }

  // Salida Serial
  Serial.printf("Bateria: %d%%, Voltaje: %d mV\n", promedioNivel, voltajeMv);

  delay(INTERVALO_REFRESCO);
}