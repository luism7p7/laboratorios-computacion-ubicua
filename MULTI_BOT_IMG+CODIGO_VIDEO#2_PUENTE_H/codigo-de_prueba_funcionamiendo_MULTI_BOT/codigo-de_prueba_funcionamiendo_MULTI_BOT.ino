// --- Motor Shield L298P ---
// Motor A conectado a M1
// Motor B conectado a M2

// Pines Motor A
#define IN1 12
#define IN2 13
#define ENA 10

// Pines Motor B
#define IN3 8
#define IN4 9
#define ENB 11

void setup() {
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  Serial.println("Iniciando...");
}

void loop() {
  Adelante(200, 200, 2000);
  Parar(1000);
  Atras(200, 200, 2000);
  Parar(1000);
  Derecha(200, 200, 1500);
  Parar(1000);
  Izquierda(200, 200, 1500);
  Parar(2000);
}

// --- FUNCIONES ---

void Adelante(int velIzq, int velDer, int Time) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, velIzq);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, velDer);

  delay(Time);
}

void Atras(int velIzq, int velDer, int Time) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, velIzq);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, velDer);

  delay(Time);
}

void Izquierda(int velIzq, int velDer, int Time) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, velIzq);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, velDer);

  delay(Time);
}

void Derecha(int velIzq, int velDer, int Time) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, velIzq);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, velDer);

  delay(Time);
}

void Parar(int Time) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  delay(Time);
}