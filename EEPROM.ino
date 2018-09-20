#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

void setAddress(int address, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address); // 0x55 => 01010101
  
  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}

byte readEEPROM(int address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }

  setAddress(address, /*outputEnable*/ true);
  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}

void writeEEPROM(int address, byte data) {
  setAddress(address, /*outputEnable*/ false);
  
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }

  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1)
  {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }

  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(1);
}

void printContents() {
  for (int base = 0; base <= 255; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }
    char buf[80];
    sprintf(buf, "%03x:   %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]); //%03x => filled by 3 digit hex with leading zeros
    Serial.println(buf);
  }
}

byte data[] = { 0x01, 0x4f, 0x12, 0x06, 0x4c, 0x24, 0x20, 0x0f, 0x00, 0x04, 0x08, 0x60, 0x31, 0x42, 0x30, 0x38 };

void setup() {
	pinMode(SHIFT_DATA, OUTPUT);
	pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);

  // Erase entire EEPROM
  Serial.print("Erasing EEPROM");
  for (int address = 0; address <= 2048; address += 1) {
    writeEEPROM(address, 0xff);

//    if (address % 64 == 0) {
//      Serial.print(".");
  //  }
  }
  Serial.println(" done");

// Program 16 bytes
  for (int address = 0; address <= 15; address += 1) {
    writeEEPROM(address, data[address]);
  }
  printContents();
}

void loop() {
}

// 7 Segment display decoder binary => hex representation

// d3 d2 d1 d0   a b c d e f g   hex
//  0  0  0  0   0 0 0 0 0 0 1    01
//  0  0  0  1   1 0 0 1 1 1 1    4f
//  0  0  1  0   0 0 1 0 0 1 0    12
//  0  0  1  1   0 0 0 0 1 1 0    06
//  0  1  0  0   1 0 0 1 1 0 0    4c
//  0  1  0  1   0 1 0 0 1 0 0    24
//  0  1  1  0   0 1 0 0 0 0 0    20
//  0  1  1  1   0 0 0 1 1 1 1    0f
//  1  0  0  0   0 0 0 0 0 0 0    00
//  1  0  0  1   0 0 0 0 1 0 0    04
//  1  0  1  0   0 0 0 1 0 0 0    08
//  1  0  1  1   1 1 0 0 0 0 0    60
//  1  1  0  0   0 1 1 0 0 0 1    31
//  1  1  0  1   1 0 0 0 0 1 0    42
//  1  1  1  0   0 1 1 0 0 0 0    30
//  1  1  1  1   0 1 1 1 0 0 0    38
