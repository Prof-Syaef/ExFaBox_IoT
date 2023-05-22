#include <EEPROM.h>
#include "sdcard.h"

#define bt1 9
#define bt2 4
#define bt3 2
#define bt4 6

#define relay1 7
#define relay2 8


byte LTemp[8] =
{
  B00100,
  B01010,
  B01010,
  B01110,
  B11111,
  B11111,
  B01110,
  B00000
};

byte LHum[8] =
{
  B00100,
  B01010,
  B01010,
  B10001,
  B10001,
  B10001,
  B01110,
  B00000
};
bool left, back, select, right;
bool key = false;
bool savingState;
bool longPress;
bool buttonTimer;
bool lcdRefresh = true;
int menuState = 0;
int menuOpt1, menuOpt2, menuOpt3;
float tempCal, humCal;
float offsetTemp, offsetHum;
float maxTemp, maxHum;
int lastState = LOW;
unsigned long timeNow, timeLCD = 0, timeRTC;
unsigned long buttonLast;
const int SHORT_PRESS_TIME = 1000;
void timeButton()
{
  if (menuState == 4)
  {
    if (timeNow - buttonLast > 1000 && !left)
    {
      if (suhu + offsetTemp > 0.1)
      {
        offsetTemp -= 0.1;
      }
    }
    else if (timeNow - buttonLast > 1000 && !right)
    {
      if (suhu + offsetTemp < 79.9)
      {
        offsetTemp += 0.1;
      }
    }
  }
  else if (menuState == 5)
  {
    if (timeNow - buttonLast > 1000 && !left)
    {
      if (kelembapan + offsetHum > 0.1)
      {
        offsetHum -= 0.1;
      }
    }
    else if (timeNow - buttonLast > 1000 && !right)
    {
      if (kelembapan + offsetHum < 99.8)
      {
        offsetHum += 0.1;
      }
    }
  }
  else if (menuState == 6)
  {
    if (timeNow - buttonLast > 1000 && !left)
    {
      if (maxTemp > 0.1)
      {
        maxTemp -= 0.1;
      }
    }
    else if (timeNow - buttonLast > 1000 && !right)
    {
      if (maxTemp < 79.9)
      {
        maxTemp += 0.1;
      }
    }
  }
  else if (menuState == 7)
  {
    if (timeNow - buttonLast > 1000 && !left)
    {
      if (maxHum > 0.1)
      {
        maxHum -= 0.1;
      }
    }
    else if (timeNow - buttonLast > 1000 && !right)
    {
      if (maxHum < 99.8)
      {
        maxHum += 0.1;
      }
    }
  }
}

void setup() {
  //data save default
  //  EEPROM.put(0, 0.0);
  //  EEPROM.put(10, 0.0);
  //  EEPROM.put(20, 50.0);
  //  EEPROM.put(30, 99.0);

  pinMode(bt1, INPUT_PULLUP);
  pinMode(bt2, INPUT_PULLUP);
  pinMode(bt3, INPUT_PULLUP);
  pinMode(bt4, INPUT_PULLUP);

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  ExFaBox PMT  ");
  lcd.setCursor(0, 1);
  lcd.print("   Starting    ");
  delay(500);
  Serial.begin(9600);
  Serial.println("Set Serial OK");

  sdcard_begin();
  rtc_begin();
  lcd.setCursor(0, 0);
  lcd.print("Check Sensor    ");
  dht_update();
  if (isnan(suhu) || isnan(kelembapan))
  {
    lcd.setCursor(0, 1);
    lcd.print("Failed          ");
    while (1)
    {
      ;
    }
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("Done            ");
    delay(500);
  }
  EEPROM.get(0, offsetTemp);
  EEPROM.get(10, offsetHum);
  EEPROM.get(20, maxTemp);
  EEPROM.get(30, maxHum);
  delay(1000);
  lcd.clear();
  rtc_update();
  dht_update();
  delay(1000);
  Serial.begin(115200);
}

void loop() {
  int read_data = 40 ; // interval reading 0 - 60 detik
  if (detik == read_data )
  {
    if (savingState)
    {
      //dht_update();
      saving_SD();
      savingState = false;
    }
  }
  else
  {
    savingState = true;
  }
  timeNow = millis();
  if (timeNow - timeRTC >= 1000)
  {
    dht_update();
    rtc_update();
    timeRTC = timeNow;
  }
  tempCal = suhu + offsetTemp;
  humCal = kelembapan + offsetHum;

  left = digitalRead(bt1);
  back = digitalRead(bt2);
  select = digitalRead(bt3);
  right = digitalRead(bt4);

  //Serial.println(" L" + String(left) + " R" + String(right) + " B" + String(back) + " S" + String(select));

  if (tempCal > maxTemp || humCal > maxHum)
  {
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
  }
  else
  {
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
  }

  if (left && back && select && right)
  {
    key = true;
    buttonTimer = true;
    buttonLast = millis();
  }
  switch (menuState)
  {
    case 0:
      if (timeNow - timeLCD < 10000)
      {
        if (lcdRefresh) {
          lcd.clear();
          lcdRefresh = false;
        }
        lcd.setCursor(1, 0);
        lcd.print("Temp : ");
        lcd.print(tempCal);
        lcd.print((char)223);
        lcd.print("C   ");
        lcd.setCursor(1, 1);
        lcd.print("Hum  : ");
        lcd.print(humCal);
        lcd.print(" %   ");
      }
      else if (((timeNow - timeLCD) < 20000) && ((timeNow - timeLCD) >= 10000))
      {
        if (!lcdRefresh) {
          lcd.clear();
          lcdRefresh = true;
        }
        lcd.setCursor(3, 0);
        lcd.print(rtc.getDateStr());

        lcd.setCursor(0, 1);
        lcd.print(rtc.getTimeStr());
       
        lcd.setCursor(10, 1);
        lcd.print(rtc.getTemp());
        
        lcd.setCursor(14, 1);
        lcd.print((char)223);
        lcd.print("C   ");

      }
      else if (timeNow - timeLCD > 20000)
      {
        timeLCD = timeNow;
      }

      if (key)
      {
        if (!select)
        {
          lcd.clear();
          menuState = 1;
          menuOpt1 = 0;
          key = false;
        }
      }
      break;
    case 1:
      if (menuOpt1 == 0)
      {
        lcd.setCursor(0, 0);
        lcd.print(">Callibration");
        lcd.setCursor(0, 1);
        lcd.print("Ideal Value");
      }
      else if (menuOpt1 == 1)
      {
        lcd.setCursor(0, 0);
        lcd.print("Callibration");
        lcd.setCursor(0, 1);
        lcd.print(">Ideal Value");
      }
      else if (menuOpt1 == 2)
      {
        lcd.setCursor(0, 0);
        lcd.print(">Device Info");
      }

      if (key)
      {
        if (!left && menuOpt1 > 0)
        {
          lcd.clear();
          menuOpt1--;
          key = false;
        }
        else if (!right && menuOpt1 < 2)
        {
          lcd.clear();
          menuOpt1++;
          key = false;
        }
        else if (!back)
        {
          lcd.clear();
          menuState = 0;
          key = false;
        }
        else if (!select && menuOpt1 == 0)
        {
          lcd.clear();
          menuState = 2;
          menuOpt2 = 0;
          key = false;
        }
        else if (!select && menuOpt1 == 1)
        {
          lcd.clear();
          menuState = 3;
          menuOpt3 = 0;
          key = false;
        }
        else if (!select && menuOpt1 == 2)
        {
          lcd.clear();
          menuState = 99;
          key = false;
        }
      }
      break;
    case 2:
      if (menuOpt2 == 0)
      {
        lcd.setCursor(0, 0);
        lcd.print(">Temp Correct  ");
        lcd.setCursor(0, 1);
        lcd.print("Hum Correct     ");
      }
      else if (menuOpt2 == 1)
      {
        lcd.setCursor(0, 0);
        lcd.print("Temp Correct  ");
        lcd.setCursor(0, 1);
        lcd.print(">Hum Correct     ");
      }

      if (key)
      {
        if (!left && menuOpt2 > 0)
        {
          lcd.clear();
          menuOpt2--;
          key = false;
        }
        else if (!right && menuOpt2 < 1)
        {
          lcd.clear();
          menuOpt2++;
          key = false;
        }
        else if (!back)
        {
          lcd.clear();
          menuState = 1;
          menuOpt1 = 0;
          key = false;
        }
        else if (!select && menuOpt2 == 0)
        {
          lcd.clear();
          menuState = 4;
          key = false;
        }
        else if (!select && menuOpt2 == 1)
        {
          lcd.clear();
          menuState = 5;
          key = false;
        }
      }
      break;
    case 3:
      if (menuOpt3 == 0)
      {
        lcd.setCursor(0, 0);
        lcd.print(">Max Temperature");
        lcd.setCursor(0, 1);
        lcd.print("Max Humidity");
      }
      else if (menuOpt3 == 1)
      {
        lcd.setCursor(0, 0);
        lcd.print("Max Temperature");
        lcd.setCursor(0, 1);
        lcd.print(">Max Humidity");
      }

      if (key)
      {
        if (!left && menuOpt3 > 0)
        {
          lcd.clear();
          menuOpt3--;
          key = false;
        }
        else if (!right && menuOpt3 < 1)
        {
          lcd.clear();
          menuOpt3++;
          key = false;
        }
        else if (!back)
        {
          lcd.clear();
          menuState = 1;
          menuOpt1 = 0;
          key = false;
        }
        else if (!select && menuOpt3 == 0)
        {
          lcd.clear();
          menuState = 6;
          key = false;
        }
        else if (!select && menuOpt3 == 1)
        {
          lcd.clear();
          menuState = 7;
          key = false;
        }
      }
      break;
    case 4:
      timeButton();
      lcd.setCursor(0, 0);
      lcd.print("Actual : ");
      lcd.print(suhu);
      lcd.print((char)223);
      lcd.print("C   ");
      lcd.setCursor(0, 1);
      if (offsetTemp > 0.0)
      {
        lcd.print("Offset : +");
        lcd.print(offsetTemp);
        lcd.print("   ");
      }
      else
      {
        lcd.print("Offset : ");
        lcd.print(offsetTemp);
        lcd.print("   ");
      }

      if (key)
      {
        if (!left)
        {
          if (suhu + offsetTemp > 0.1)
          {
            offsetTemp -= 0.1;
          }
          key = false;
        }
        else if (!right)
        {
          if (suhu + offsetTemp < 79.9)
          {
            offsetTemp += 0.1;
          }
          key = false;
        }
        else if (!select)
        {
          key = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Save Data...");
          EEPROM.put(0, offsetTemp);
          delay(500);
          lcd.setCursor(0, 1);
          lcd.print("DATA SAVED");
          delay(500);
          lcd.clear();
          menuState = 2;
          menuOpt2 = 0;
        }
        else if (!back)
        {
          key = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          EEPROM.get(0, offsetTemp);
          lcd.print("Cancelled");
          delay(500);
          lcd.clear();
          menuState = 2;
          menuOpt2 = 0;
        }
      }
      break;
    case 5:
      timeButton();
      lcd.setCursor(0, 0);
      lcd.print("Actual : ");
      lcd.print(kelembapan);
      lcd.print(" %   ");
      lcd.setCursor(0, 1);
      if (offsetHum > 0.0)
      {
        lcd.print("Offset : +");
        lcd.print(offsetHum);
        lcd.print("   ");
      }
      else
      {
        lcd.print("Offset : ");
        lcd.print(offsetHum);
        lcd.print("   ");
      }
      if (key)
      {
        if (!left)
        {
          if (kelembapan + offsetHum > 0.1)
          {
            offsetHum -= 0.1;
          }
          key = false;
        }
        else if (!right)
        {
          if (kelembapan + offsetHum < 99.8)
          {
            offsetHum += 0.1;
          }
          key = false;
        }
        else if (!select)
        {
          key = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Save Data...");
          EEPROM.put(10, offsetHum);
          delay(500);
          lcd.setCursor(0, 1);
          lcd.print("DATA SAVED");
          delay(500);
          lcd.clear();
          menuState = 2;
          menuOpt2 = 0;
        }
        else if (!back)
        {
          key = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          EEPROM.get(10, offsetHum);
          lcd.print("Cancelled");
          delay(500);
          lcd.clear();
          menuState = 2;
          menuOpt2 = 0;
        }
      }
      break;
    case 6:
      timeButton();
      lcd.setCursor(0, 0);
      lcd.print("Temp : ");
      lcd.print(tempCal);
      lcd.print((char)223);
      lcd.print("C   ");
      lcd.setCursor(0, 1);
      lcd.print("Max  : ");
      lcd.print(maxTemp);
      lcd.print((char)223);
      lcd.print("C   ");
      if (key)
      {
        if (!left)
        {
          if (maxTemp > 0.1)
          {
            maxTemp -= 0.1;
          }
          key = false;
        }
        else if (!right)
        {
          if (maxTemp < 79.9)
          {
            maxTemp += 0.1;
          }
          key = false;
        }
        else if (!select)
        {
          key = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Save Data...");
          EEPROM.put(20, maxTemp);
          delay(500);
          lcd.setCursor(0, 1);
          lcd.print("DATA SAVED");
          delay(500);
          lcd.clear();
          menuState = 3;
          menuOpt3 = 0;
        }
        else if (!back)
        {
          key = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          EEPROM.get(20, maxTemp);
          lcd.print("Cancelled");
          delay(500);
          lcd.clear();
          menuState = 3;
          menuOpt3 = 0;
        }
      }
      break;
    case 7:
      timeButton();
      lcd.setCursor(0, 0);
      lcd.print("Hum : ");
      lcd.print(humCal);
      lcd.print(" %   ");
      lcd.setCursor(0, 1);
      lcd.print("Max : ");
      lcd.print(maxHum);
      lcd.print(" %   ");
      if (key)
      {
        if (!left)
        {
          if (maxHum > 0.1)
          {
            maxHum -= 0.1;
          }
          key = false;
        }
        else if (!right)
        {
          if (maxHum < 99.8)
          {
            maxHum += 0.1;
          }
          key = false;
        }
        else if (!select)
        {
          key = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Save Data...");
          EEPROM.put(30, maxHum);
          delay(500);
          lcd.setCursor(0, 1);
          lcd.print("DATA SAVED");
          delay(500);
          lcd.clear();
          menuState = 3;
          menuOpt3 = 0;
        }
        else if (!back)
        {
          key = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          EEPROM.get(30, maxHum);
          lcd.print("Cancelled");
          delay(500);
          lcd.clear();
          menuState = 3;
          menuOpt3 = 0;
        }
      }
      break;
    case 99:
      lcd.setCursor(0, 0);
      lcd.print("Firmware V1.0");
      lcd.setCursor(0, 1);
      lcd.print("Hardware V1.0");
      if (key)
      {
        if (!back)
        {
          key = false;
          lcd.clear();
          menuState = 1;
          menuOpt1 = 0;
        }
      }
      break;
  }
}
