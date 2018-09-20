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

// byte data[] = { 0x01, 0x4f, 0x12, 0x06, 0x4c, 0x24, 0x20, 0x0f, 0x00, 0x04, 0x08, 0x60, 0x31, 0x42, 0x30, 0x38 };

// 4-bit hex decoder for common anode 7-segment display
// byte data[] = {0x81, 0xcf, 0x92, 0x86, 0xcc, 0xa4, 0xa0, 0x8f, 0x80, 0x84, 0x88, 0xe0, 0xb1, 0xc2, 0xb0, 0xb8};

// 4-bit hex decoder for common cathode 7-segment display
// byte data[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b, 0x77, 0x1f, 0x4e, 0x3d, 0x4f, 0x47 };

void setup() {
	pinMode(SHIFT_DATA, OUTPUT);
	pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);

  Serial.println("Programming EEPROM");

  byte digits[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b }; //0-9

  Serial.println("Programming ones place");
  for (int value = 0; value < 255; value += 1) {
    writeEEPROM(value, digits[value % 10]); //one's place
  }
  Serial.println("Programming tens place");
  for (int value = 0; value < 255; value += 1) {
    writeEEPROM(value + 256, digits[(value / 10) % 10]); //ten's place
  }
  Serial.println("Programming hundreds place");
  for (int value = 0; value < 255; value += 1) {
    writeEEPROM(value + 512, digits[(value / 100) % 10]); //hundreds's place
  }
  Serial.println("Programming sign");
  for (int value = 0; value < 255; value += 1) {
    writeEEPROM(value + 768, 0); //first 7 segment
  }
  Serial.println("Reading EEPROM");
  printContents();
}

void loop() {
}

// 7 Segment display decoder binary => hex representation

// 321
// A10 A9 A8   A7 A6 A5 A4 A3 A2 A1 A0          D7 D6 D5 D4 D3 D2 D1 D0
//   0  0  0    0  1  1  1  1  0  1  1    =>     0  1  0  0  1  1  1  1  =>  3
//   0  0  1    0  1  1  1  1  0  1  1    =>     0  1  0  1  1  0  1  1  =>  2
//   0  1  0    0  1  1  1  1  0  1  1    =>     0  0  0  0  0  0  1  1  =>  1
//   0  1  1    0  1  1  1  1  0  1  1    =>     0  0  0  0  0  0  0  0  =>  -
