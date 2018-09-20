#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4

void setAddress(int address) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address >> 8); // 0x55 => 01010101
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address); // 0x55 => 01010101
  
  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}
void setup() {
	pinMode(SHIFT_DATA, OUTPUT);
	pinMode(SHIFT_CLK, OUTPUT);
	pinMode(SHIFT_LATCH, OUTPUT);
	shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, 0x55); // 0x55 => 01010101
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, 0x55); // 0x55 => 01010101
  
  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
  setAddress(1234);
}

void loop() {
}
