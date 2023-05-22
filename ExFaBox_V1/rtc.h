#include <DS3231.h>
#include "lcd.h"
DS3231  rtc(SDA, SCL);
Time  t;
int detik;

void rtc_begin() {
  lcd.setCursor(0, 0);
  lcd.print("Check RTC");
  rtc.begin();
}

void rtc_update() {
  t = rtc.getTime();
  detik = t.sec;
  Serial.println(detik);
  
}
