#include <SPI.h>
#include <SD.h>
#include "rtc.h"
#include "sensor.h"

const int chipSelect = 10;
File myFile;

void sdcard_begin()
{
  Serial.print("Initializing SD card...");
  lcd.setCursor(0, 0);
  lcd.print("Check SD Card   ");
  while (!SD.begin())
  {
    Serial.println("initialization failed!");
    lcd.setCursor(0, 1);
    lcd.print("Failed          ");
  }
  Serial.println("initialization done.");
  lcd.setCursor(0, 1);
  lcd.print("Done            ");
  Serial.flush();
  delay(500);
}

void saving_SD()
{
  Serial.println("saving sd");
  myFile = SD.open("DATA.txt", FILE_WRITE);

  if (myFile)
  {
    Serial.print("Writing to DATA.txt...");
    myFile.print(rtc.getDateStr());
    myFile.print(";");
    myFile.print(rtc.getTimeStr());
    myFile.print(";");
    myFile.print(suhu);
    myFile.print(";");
    myFile.println(kelembapan);
    myFile.close();
    Serial.println("done.");
  }
  else
  {
    Serial.println("error opening DATA.txt");
  }
  
}
