// Pin definitions for each Charlieplex group
const int CP1 = 5;
const int CP2 = 6;
const int CP3 = 7;

const int CP4 = A0;
const int CP5 = A1;
const int CP6 = A2;

const int CP7 = 0;
const int CP8 = 1;
const int CP9 = 2;

void setup() {
  // Initialize all pins as input (high impedance)
  pinMode(CP1, INPUT);
  pinMode(CP2, INPUT);
  pinMode(CP3, INPUT);
  pinMode(CP4, INPUT);
  pinMode(CP5, INPUT);
  pinMode(CP6, INPUT);
  pinMode(CP7, INPUT);
  pinMode(CP8, INPUT);
  pinMode(CP9, INPUT);
}

void turnOffAllLEDs() {
  pinMode(CP1, INPUT);
  pinMode(CP2, INPUT);
  pinMode(CP3, INPUT);
  pinMode(CP4, INPUT);
  pinMode(CP5, INPUT);
  pinMode(CP6, INPUT);
  pinMode(CP7, INPUT);
  pinMode(CP8, INPUT);
  pinMode(CP9, INPUT);
}

void turnOnD1() {
  turnOffAllLEDs();
  pinMode(CP3, OUTPUT);
  pinMode(CP2, OUTPUT);
  digitalWrite(CP3, HIGH);
  digitalWrite(CP2, LOW);
}

void turnOnD2() {
  turnOffAllLEDs();
  pinMode(CP2, OUTPUT);
  pinMode(CP1, OUTPUT);
  digitalWrite(CP2, HIGH);
  digitalWrite(CP1, LOW);
}

void turnOnD3() {
  turnOffAllLEDs();
  pinMode(CP2, OUTPUT);
  pinMode(CP3, OUTPUT);
  digitalWrite(CP2, HIGH);
  digitalWrite(CP3, LOW);
}

void turnOnD4() {
  turnOffAllLEDs();
  pinMode(CP1, OUTPUT);
  pinMode(CP2, OUTPUT);
  digitalWrite(CP1, HIGH);
  digitalWrite(CP2, LOW);
}

void turnOnD5() {
  turnOffAllLEDs();
  pinMode(CP3, OUTPUT);
  pinMode(CP1, OUTPUT);
  digitalWrite(CP3, HIGH);
  digitalWrite(CP1, LOW);
}

void turnOnD6() {
  turnOffAllLEDs();
  pinMode(CP1, OUTPUT);
  pinMode(CP3, OUTPUT);
  digitalWrite(CP1, HIGH);
  digitalWrite(CP3, LOW);
}

void turnOnD7() {
  turnOffAllLEDs();
  pinMode(CP6, OUTPUT);
  pinMode(CP5, OUTPUT);
  digitalWrite(CP6, HIGH);
  digitalWrite(CP5, LOW);
}

void turnOnD8() {
  turnOffAllLEDs();
  pinMode(CP5, OUTPUT);
  pinMode(CP4, OUTPUT);
  digitalWrite(CP5, HIGH);
  digitalWrite(CP4, LOW);
}

void turnOnD9() {
  turnOffAllLEDs();
  pinMode(CP5, OUTPUT);
  pinMode(CP6, OUTPUT);
  digitalWrite(CP5, HIGH);
  digitalWrite(CP6, LOW);
}

void turnOnD10() {
  turnOffAllLEDs();
  pinMode(CP4, OUTPUT);
  pinMode(CP5, OUTPUT);
  digitalWrite(CP4, HIGH);
  digitalWrite(CP5, LOW);
}

void turnOnD11() {
  turnOffAllLEDs();
  pinMode(CP6, OUTPUT);
  pinMode(CP4, OUTPUT);
  digitalWrite(CP6, HIGH);
  digitalWrite(CP4, LOW);
}

void turnOnD12() {
  turnOffAllLEDs();
  pinMode(CP4, OUTPUT);
  pinMode(CP6, OUTPUT);
  digitalWrite(CP4, HIGH);
  digitalWrite(CP6, LOW);
}

void turnOnD13() {
  turnOffAllLEDs();
  pinMode(CP9, OUTPUT);
  pinMode(CP8, OUTPUT);
  digitalWrite(CP9, HIGH);
  digitalWrite(CP8, LOW);
}

void turnOnD14() {
  turnOffAllLEDs();
  pinMode(CP8, OUTPUT);
  pinMode(CP7, OUTPUT);
  digitalWrite(CP8, HIGH);
  digitalWrite(CP7, LOW);
}

void turnOnD15() {
  turnOffAllLEDs();
  pinMode(CP8, OUTPUT);
  pinMode(CP9, OUTPUT);
  digitalWrite(CP8, HIGH);
  digitalWrite(CP9, LOW);
}

void turnOnD16() {
  turnOffAllLEDs();
  pinMode(CP7, OUTPUT);
  pinMode(CP8, OUTPUT);
  digitalWrite(CP7, HIGH);
  digitalWrite(CP8, LOW);
}

// Example usage function
void cycleThroughAllLEDs() {
  int delayTime = 50;

  turnOnD7(); delay(delayTime);
  turnOnD9(); delay(delayTime);
  turnOnD8(); delay(delayTime);
  turnOnD10(); delay(delayTime);
  turnOnD11(); delay(delayTime);
  turnOnD12(); delay(delayTime);
  turnOnD1(); delay(delayTime);
  turnOnD3(); delay(delayTime);
  turnOnD4(); delay(delayTime);
  turnOnD2(); delay(delayTime);
  turnOnD5(); delay(delayTime);
  turnOnD6(); delay(delayTime);
  turnOnD13(); delay(delayTime);
  turnOnD15(); delay(delayTime);
  turnOnD14(); delay(delayTime);
  turnOnD16(); delay(delayTime);
  turnOffAllLEDs();
}

void loop() {
  cycleThroughAllLEDs();
}