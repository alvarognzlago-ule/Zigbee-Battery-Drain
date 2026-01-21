#include <M5Unified.h>

// Smoothing configuration to avoid spikes in the measurement
const int NUM_LECTURAS = 10;
int lecturas[NUM_LECTURAS];
int lecturaIndex = 0;
long total = 0;
int promedioNivel = 0;
int numLecturas = 0;  // Count of valid readings

// Screen refresh interval
const unsigned long INTERVALO_REFRESCO = 1000;

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  Serial.begin(115200);
  delay(100);

  // Display configuration
  M5.Display.setRotation(1);
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextDatum(MC_DATUM); // Middle-Center

  // Initialize the readings buffer
  for (int i = 0; i < NUM_LECTURAS; i++) {
    lecturas[i] = 0;
  }

  // First reading to initialize buffer
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

  // --- 1. DATA ACQUISITION ---
  int nivelActual = M5.Power.getBatteryLevel();
  int voltajeMv = M5.Power.getBatteryVoltage();
  bool isCharging = M5.Power.isCharging();

  // --- 2. MOVING AVERAGE ---
  total = total - lecturas[lecturaIndex];
  lecturas[lecturaIndex] = nivelActual;
  total = total + lecturas[lecturaIndex];
  lecturaIndex = (lecturaIndex + 1) % NUM_LECTURAS;

  promedioNivel = total / NUM_LECTURAS;

  // --- 3. DRAW ON SCREEN ---
  M5.Display.fillScreen(TFT_BLACK);

  int centerX = M5.Display.width() / 2;
  int centerY = M5.Display.height() / 2;

  // Color according to battery level
  uint16_t colorBateria;
  if (promedioNivel > 60) {
    colorBateria = TFT_GREEN;
  } else if (promedioNivel > 20) {
    colorBateria = TFT_ORANGE;
  } else {
    colorBateria = TFT_RED;
  }

  // A) SHOW LARGE PERCENTAGE
  M5.Display.setTextColor(colorBateria, TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.setFont(&fonts::FreeSerifBold24pt7b);  // Large compatible font

  String porcentajeStr = String(promedioNivel) + "%";
  M5.Display.drawString(porcentajeStr, centerX, centerY);

  // B) SHOW VOLTAGE
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setFont(&fonts::Font2);
  String infoVoltaje = String(voltajeMv) + " mV";
  M5.Display.drawString(infoVoltaje, centerX, centerY + 50);

  // C) CHARGING STATUS
  M5.Display.setFont(&fonts::Font2);
  if (isCharging) {
    M5.Display.setTextColor(TFT_CYAN, TFT_BLACK);
    M5.Display.drawString("CHARGING", centerX, 20);
  } else {
    M5.Display.setTextColor(TFT_DARKGREY, TFT_BLACK);
    M5.Display.drawString("BATTERY", centerX, 20);
  }

  // Serial output
  Serial.printf("Battery: %d%%, Voltage: %d mV\n", promedioNivel, voltajeMv);

  delay(INTERVALO_REFRESCO);
}