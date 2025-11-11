#include <SPI.h>       // Librería para comunicación SPI (usada por el lector RFID)
#include <MFRC522.h>   // Librería para manejar el módulo RFID

// Pines para el lector RFID
#define SS_PIN 10
#define RST_PIN 9

// Pines para los LEDs
#define LED_GREEN 2
#define LED_RED   3
#define LED_BLUE  4

// Pin para el motor (se conecta a la base del TIP120)
#define MOTOR_PIN 6  // este pin permite PWM (control de potencia)

// Se crea el objeto del lector RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);

// ---- Lista de tarjetas RFID autorizadas ----
// Aquí se guardan los códigos UID de las tarjetas que sí pueden activar el sistema.
// Cada UID es un conjunto de 4 números en hexadecimal.
const byte authorizedUIDs[][4] = {
  {0xD3, 0x5D, 0x3E, 0x02},  // Tarjeta 1 (autorizada)
  {0xDE, 0x43, 0xD5, 0x02}   // Tarjeta 2 (autorizada)
  // Se pueden agregar más líneas si tienes más tarjetas
};

// Calcula automáticamente cuántas tarjetas hay en la lista
const int authorizedCount = sizeof(authorizedUIDs) / sizeof(authorizedUIDs[0]);

// Tiempo que el motor estará encendido cuando se detecta una tarjeta válida (en milisegundos)
const unsigned long MOTOR_RUN_TIME = 5000UL; // 5000 ms = 5 segundos

// ---- Variables para hacer parpadear el LED azul mientras el sistema espera una tarjeta ----
unsigned long previousBlink = 0;         // Guarda el tiempo del último parpadeo
const unsigned long BLINK_INTERVAL = 500; // Cada cuánto parpadea (0.5 segundos)
bool blinkState = false;                 // Guarda si el LED está encendido o apagado

// ---- Variables para controlar el motor ----
unsigned long motorStart = 0;  // Guarda el momento en que el motor fue encendido
bool motorRunning = false;     // Dice si el motor está funcionando o no


// -------------------- CONFIGURACIÓN INICIAL --------------------
void setup() {

  Serial.begin(115200); // Inicia la comunicación con el PC para ver mensajes
  while (!Serial);       // Espera hasta que el monitor serial esté listo

  SPI.begin();           // Inicia la comunicación SPI
  mfrc522.PCD_Init();    // Inicia el lector RFID

  // Define los pines como salidas
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);

  // Apaga todo al inicio
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);
  analogWrite(MOTOR_PIN, 0);

  // Mensaje de bienvenida
  Serial.println(F("RFID listo. Acerque tarjeta..."));
}


// -------------------- BUCLE PRINCIPAL --------------------
void loop() {
  unsigned long now = millis(); // Guarda el tiempo actual desde que se encendió el Arduino

  // (1) Parpadeo del LED azul (modo "en espera")
  if (now - previousBlink >= BLINK_INTERVAL) {
    previousBlink = now;
    blinkState = !blinkState; // Cambia el estado (encendido ↔ apagado)
    digitalWrite(LED_BLUE, blinkState ? HIGH : LOW);
  }

  // (2) Si el motor está encendido y ya pasó el tiempo, se apaga automáticamente
  if (motorRunning && (now - motorStart >= MOTOR_RUN_TIME)) {
    stopMotor();
  }

  // (3) Revisa si hay una nueva tarjeta cerca del lector
  if (!mfrc522.PICC_IsNewCardPresent()) return;   // Si no hay, vuelve al inicio del loop
  if (!mfrc522.PICC_ReadCardSerial()) return;     // Si no se puede leer bien, vuelve al inicio

  // Apaga el LED azul mientras se procesa la lectura
  digitalWrite(LED_BLUE, LOW);

  // Muestra el UID de la tarjeta detectada en el monitor serial
  Serial.print(F("UID: "));
  printUID(mfrc522.uid.uidByte, mfrc522.uid.size);

  // Comprueba si el UID leído pertenece a una tarjeta autorizada
  if (isAuthorized(mfrc522.uid.uidByte, mfrc522.uid.size)) {
    Serial.println(F(" -> AUTORIZADO"));
    digitalWrite(LED_GREEN, HIGH); // Enciende verde
    digitalWrite(LED_RED, LOW);    // Apaga rojo
    startMotor();                  // Enciende el motor
  } else {
    Serial.println(F(" -> NO AUTORIZADO"));
    digitalWrite(LED_GREEN, LOW);  // Apaga verde
    digitalWrite(LED_RED, HIGH);   // Enciende rojo
  }

  // Libera la tarjeta para poder leer otra después
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  // Pequeña pausa para evitar que lea la misma tarjeta muchas veces seguidas
  delay(400);
}


// -------------------- FUNCIONES AUXILIARES --------------------

// Muestra el UID de una tarjeta en el monitor serial
void printUID(byte *uid, byte uidSize) {
  for (byte i = 0; i < uidSize; i++) {
    if (uid[i] < 0x10) Serial.print("0"); // Agrega un 0 si es menor a 16 (para formato bonito)
    Serial.print(uid[i], HEX);            // Lo muestra en hexadecimal
    if (i < uidSize - 1) Serial.print(":"); // Separa con dos puntos
  }
  Serial.println();
}

// Verifica si la tarjeta leída está en la lista de tarjetas autorizadas
bool isAuthorized(byte *uid, byte uidSize) {
  if (uidSize != 4) return false; // Solo compara si el UID tiene 4 bytes

  // Recorre cada tarjeta en la lista de autorizadas
  for (int i = 0; i < authorizedCount; i++) {
    bool match = true;
    for (int j = 0; j < 4; j++) {
      if (authorizedUIDs[i][j] != uid[j]) { // Si un byte no coincide, no es igual
        match = false;
        break;
      }
    }
    if (match) return true; // Si todos los bytes coinciden, la tarjeta está autorizada
  }
  return false; // Si ninguna coincide, no está autorizada
}

// Enciende el motor por el tiempo establecido
void startMotor() {
  const int motorSpeed = 200; // Potencia del motor (de 0 a 255)
  analogWrite(MOTOR_PIN, motorSpeed);
  motorStart = millis();      // Guarda el momento de inicio
  motorRunning = true;        // Marca que el motor está encendido
  Serial.println(F("Motor ON"));
}

// Apaga el motor y los LEDs
void stopMotor() {
  analogWrite(MOTOR_PIN, 0);  // Detiene el motor
  motorRunning = false;
  Serial.println(F("Motor OFF"));

  // Apaga los LEDs después de finalizar
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
}

