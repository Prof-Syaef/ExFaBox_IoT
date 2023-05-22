#include <dhtnew.h>
DHTNEW DHTPin(A0);
unsigned long timeScale               = 1000;
unsigned long routineTime             = 1;
unsigned long routineTimepoint        = 0;
float kelembapan, suhu;

void dht_update() {
  if (millis() - routineTimepoint > (routineTime * timeScale)) {
    DHTPin.read();
    //read humidity
    kelembapan = (DHTPin.getHumidity());
    Serial.print("Humidity : "); // Jangan dihapus, untuk cek sensor
    Serial.println(kelembapan); // Jangan dihapus, untuk cek sensor
    //read temperature
    suhu = (DHTPin.getTemperature());
    Serial.print("Temperature : "); // Jangan dihapus, untuk cek sensor
    Serial.println(suhu);  // Jangan dihapus, untuk cek sensor
    routineTimepoint = millis();
  }
}
