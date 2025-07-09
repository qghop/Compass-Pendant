#include <Wire.h>
#include <EEPROM.h>

// --- Charlieplex LEDs Pin Definitions ---
const int CP1 = 5;
const int CP2 = 6;
const int CP3 = 7;
const int CP4 = A0;
const int CP5 = A1;
const int CP6 = A2;
const int CP7 = 0;
const int CP8 = 1;
const int CP9 = 2;

// --- MMC5633 I2C Setup ---
#define MMC5633_ADDRESS 0x30
#define MMC5633_XOUT_0      0x00
#define MMC5633_XOUT_1      0x01
#define MMC5633_YOUT_0      0x02
#define MMC5633_YOUT_1      0x03
#define MMC5633_ZOUT_0      0x04
#define MMC5633_ZOUT_1      0x05
#define MMC5633_XOUT_2      0x06
#define MMC5633_YOUT_2      0x07
#define MMC5633_ZOUT_2      0x08
#define MMC5633_STATUS_1    0x18
#define MMC5633_CTRL_0      0x1B
#define MMC5633_CTRL_1      0x1C
#define MMC5633_PRODUCT_ID  0x39

// --- Calibration Values ---
float mag_offset_x = 524288.0;
float mag_offset_y = 524288.0;
float mag_offset_z = 524288.0;
float mag_scale_x = 1.0;
float mag_scale_y = 1.0;
float mag_scale_z = 1.0;

// --- EEPROM Storage ---
#define EEPROM_MAGIC 0x42

void saveCalibrationToEEPROM() {
  EEPROM.update(0, EEPROM_MAGIC);
  int addr = 1;
  EEPROM.put(addr, mag_offset_x); addr += sizeof(float);
  EEPROM.put(addr, mag_offset_y); addr += sizeof(float);
  EEPROM.put(addr, mag_offset_z); addr += sizeof(float);
  EEPROM.put(addr, mag_scale_x);  addr += sizeof(float);
  EEPROM.put(addr, mag_scale_y);  addr += sizeof(float);
  EEPROM.put(addr, mag_scale_z);  addr += sizeof(float);
}

bool loadCalibrationFromEEPROM() {
  if (EEPROM.read(0) != EEPROM_MAGIC) return false;
  int addr = 1;
  EEPROM.get(addr, mag_offset_x); addr += sizeof(float);
  EEPROM.get(addr, mag_offset_y); addr += sizeof(float);
  EEPROM.get(addr, mag_offset_z); addr += sizeof(float);
  EEPROM.get(addr, mag_scale_x);  addr += sizeof(float);
  EEPROM.get(addr, mag_scale_y);  addr += sizeof(float);
  EEPROM.get(addr, mag_scale_z);  addr += sizeof(float);
  return true;
}

// --- Magnetometer ---
bool initMagnetometer() {
  Wire.begin();
  delay(20);
  Wire.beginTransmission(MMC5633_ADDRESS);
  Wire.write(MMC5633_PRODUCT_ID);
  if (Wire.endTransmission() != 0) return false;
  Wire.requestFrom(MMC5633_ADDRESS, 1);
  if (!Wire.available()) return false;
  if (Wire.read() != 0x10) return false;

  Wire.beginTransmission(MMC5633_ADDRESS);
  Wire.write(MMC5633_CTRL_1);
  Wire.write(0x80); // Soft reset
  Wire.endTransmission();
  delay(20);

  Wire.beginTransmission(MMC5633_ADDRESS);
  Wire.write(MMC5633_CTRL_0);
  Wire.write(0x20); // Auto_SR_en
  Wire.endTransmission();
  delay(1);
  return true;
}

bool readRawMagnetometer(uint32_t &x, uint32_t &y, uint32_t &z) {
  Wire.beginTransmission(MMC5633_ADDRESS);
  Wire.write(MMC5633_CTRL_0);
  Wire.write(0b00100001);
  Wire.endTransmission();

  unsigned long start = millis();
  while (millis() - start < 20) {
    Wire.beginTransmission(MMC5633_ADDRESS);
    Wire.write(MMC5633_STATUS_1);
    Wire.endTransmission();
    Wire.requestFrom(MMC5633_ADDRESS, 1);
    if (Wire.available() && (Wire.read() & 0b01000000)) break;
    delay(2);
  }

  uint8_t data[9];
  Wire.beginTransmission(MMC5633_ADDRESS);
  Wire.write(MMC5633_XOUT_0);
  Wire.endTransmission();
  if (Wire.requestFrom(MMC5633_ADDRESS, 6) != 6) return false;
  for (int i = 0; i < 6; i++) data[i] = Wire.read();

  Wire.beginTransmission(MMC5633_ADDRESS);
  Wire.write(MMC5633_XOUT_2);
  Wire.endTransmission();
  if (Wire.requestFrom(MMC5633_ADDRESS, 3) != 3) return false;
  for (int i = 0; i < 3; i++) data[i + 6] = Wire.read();

  x = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | (data[6] >> 4);
  y = ((uint32_t)data[2] << 12) | ((uint32_t)data[3] << 4) | (data[7] >> 4);
  z = ((uint32_t)data[4] << 12) | ((uint32_t)data[5] << 4) | (data[8] >> 4);
  return true;
}

// --- Calibrate and Store ---
void calibrateMagnetometer() {
  uint32_t min_x = 0xFFFFFFFF, max_x = 0;
  uint32_t min_y = 0xFFFFFFFF, max_y = 0;
  uint32_t min_z = 0xFFFFFFFF, max_z = 0;

  unsigned long startTime = millis();
  // originally 30000 seconds, now 15000
  while (millis() - startTime < 15000) {
    uint32_t x, y, z;
    if (readRawMagnetometer(x, y, z)) {
      min_x = min(min_x, x); max_x = max(max_x, x);
      min_y = min(min_y, y); max_y = max(max_y, y);
      min_z = min(min_z, z); max_z = max(max_z, z);
    }
    delay(50);
  }

  mag_offset_x = (max_x + min_x) / 2.0;
  mag_offset_y = (max_y + min_y) / 2.0;
  mag_offset_z = (max_z + min_z) / 2.0;

  float range_x = max_x - min_x;
  float range_y = max_y - min_y;
  float avg_range = (range_x + range_y) / 2.0;

  mag_scale_x = avg_range / range_x;
  mag_scale_y = avg_range / range_y;
  mag_scale_z = 1.0;
}

// --- Get North ---
float getCompassHeading() {
  uint32_t raw_x, raw_y, raw_z;

  if (readRawMagnetometer(raw_x, raw_y, raw_z)) {
    // Apply calibration values to raw data before calculating heading
    float calibrated_x = ((float)raw_x - mag_offset_x) * mag_scale_x;
    float calibrated_y = ((float)raw_y - mag_offset_y) * mag_scale_y;
    
    // Calculate heading from the calibrated XY plane data.
    // atan2 handles all quadrants correctly.
    float heading = atan2(calibrated_y, calibrated_x) * 180.0 / PI;
    
    // Normalize to 0-360 degrees
    if (heading < 0) {
      heading += 360.0;
    }
    return heading;
  } else {
    return -1.0; // Error reading magnetometer
  }
}

float getNorthDirection() {
  float heading = getCompassHeading();
  if (heading < 0) {
    return -1.0; // Error
  }
  
  // The direction TO North is the opposite of our current heading.
  float northDirection = 360.0 - heading;
  if (northDirection >= 360.0) {
      northDirection -= 360.0;
  }
  
  return northDirection;
}

void showNorthDirection() {
  float northDirection = getNorthDirection();
  
  if (northDirection < 0) {
    // Indicate an error by flashing an LED
    turnOnD7();
    delay(200);
    turnOffAllLEDs();
    delay(200);
    return;
  }

  // Shift the direction 90 degrees clockwise 
  // Quick fix, getNorthDirection gets direction of West for whatever reason
  northDirection += 90.0;
  if (northDirection >= 360.0) northDirection -= 360.0;
  
  // Convert degrees to LED index (16 LEDs = 22.5° each).
  // Add 11.25deg offset for proper rounding to the nearest LED.
  int ledIndex = (int)((northDirection + 11.25) / 22.5) % 16;
  
  switch(ledIndex) {
    case 0:  turnOnD7(); break;  // N
    case 1:  turnOnD9(); break;  // NNE
    case 2:  turnOnD8(); break;  // NE
    case 3:  turnOnD10(); break; // ENE
    case 4:  turnOnD11(); break; // E
    case 5:  turnOnD12(); break; // ESE
    case 6:  turnOnD1(); break;  // SE
    case 7:  turnOnD3(); break;  // SSE
    case 8:  turnOnD4(); break;  // S
    case 9:  turnOnD2(); break;  // SSW
    case 10: turnOnD5(); break; // SW
    case 11: turnOnD6(); break; // WSW
    case 12: turnOnD13(); break;// W
    case 13: turnOnD15(); break;// WNW
    case 14: turnOnD14(); break;// NW
    case 15: turnOnD16(); break;// NNW
    default: turnOffAllLEDs(); break;
  }
}

// --- LED Control Functions ---
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

void cycleThroughAllLEDs() {
  int delayTime = 200;
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

void calibrationSignal() {
  turnOffAllLEDs();
  for (int i = 0; i <= 2; i++) {
    turnOnD7(); delay(200);
    turnOnD11(); delay(200);
    turnOnD4(); delay(200);
    turnOnD13(); delay(200);
  }
  turnOffAllLEDs();
}

// --- Setup ---
void setup() {
  pinMode(CP1, INPUT);
  pinMode(CP2, INPUT);
  pinMode(CP3, INPUT);
  pinMode(CP4, INPUT);
  pinMode(CP5, INPUT);
  pinMode(CP6, INPUT);
  pinMode(CP7, INPUT);
  pinMode(CP8, INPUT);
  pinMode(CP9, INPUT);

  if (!initMagnetometer()) {
    // Magnetometer failed to initialize, cycle LEDs
    while(1) {
      cycleThroughAllLEDs();
    };
  }

  // If no calibration data in EEPROM, run calibration
  if (!loadCalibrationFromEEPROM()) {
    calibrationSignal();
    calibrateMagnetometer();
    saveCalibrationToEEPROM();
    calibrationSignal();
  }
}

void loop() {
  showNorthDirection();
  delay(50);
}