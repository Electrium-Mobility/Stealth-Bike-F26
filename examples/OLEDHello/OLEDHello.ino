#include <U8g2lib.h>

// Constructor order: SCL first, then SDA
U8G2_SSD1309_128X64_NONAME0_F_SW_I2C screen(
  U8G2_R0, 22, 21, U8X8_PIN_NONE
);

void setup() {
  delay(500);
  screen.setI2CAddress(0x3C << 1);
  screen.setBusClock(100000);
  screen.begin();
}

void loop() {
  screen.clearBuffer();
  screen.setFont(u8g2_font_ncenB14_tr);
  screen.drawStr(24, 38, "HELLO");
  screen.sendBuffer();
  delay(500);
}
