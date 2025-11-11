#include <DHT.h>
#include <LiquidCrystal.h>

// ====== Configuración de pines ======
#define PIN_DHT   8        // Pin de datos del DHT22
#define DHTTYPE   DHT22
#define PIN_LM35  A0       // Salida analógica del LM35

// LCD: rs, en, d4, d5, d6, d7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
DHT dht(PIN_DHT, DHTTYPE);

// ====== Función: leer LM35 con promedio ======
float readLM35C(uint8_t samples = 10) {
  long sum = 0;
  for (uint8_t i = 0; i < samples; i++) {
    sum += analogRead(PIN_LM35);
    delay(5); // pequeño retardo para estabilizar
  }
  float avg = sum / (float)samples;

  // Si alimentas el LM35 con 3.3V, cambia 5.0 por 3.3
  float voltage = avg * (5.0 / 1023.0);
  return voltage * 100.0; // 10 mV/°C
}

// ====== Timings del DHT22 ======
unsigned long lastRead = 0;
const unsigned long DHT_INTERVAL = 2500; // >= 2s para DHT22

void setup() {
  lcd.begin(16, 2);
  lcd.print(F("Iniciando..."));
  dht.begin();
  delay(2000);
  lcd.clear();
  lcd.print(F("DHT22 + LM35"));
  delay(1500);
  lcd.clear();
}

void loop() {
  // Respetar el intervalo mínimo del DHT22
  if (millis() - lastRead < DHT_INTERVAL) return;
  lastRead = millis();

  // Lecturas
  float h     = dht.readHumidity();
  float t_dht = dht.readTemperature(); // °C
  float t_lm35 = readLM35C();

  // Manejo de error del DHT22
  if (isnan(h) || isnan(t_dht)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Error DHT22"));
    lcd.setCursor(0, 1);
    lcd.print(F("Reintentando..."));
    return; // en el próximo ciclo vuelve a intentar
  }

  // Mostrar en pantalla (16 caracteres por línea)
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("THD22:"));
  lcd.print(t_dht, 1);
  lcd.print(F("C H:"));
  lcd.print(h, 1);
  lcd.print(F("%"));

  lcd.setCursor(0, 1);
  lcd.print(F("LM35:"));
  lcd.print(t_lm35, 1);
  lcd.print(F("C"));
}
