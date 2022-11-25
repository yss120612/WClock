#include "BMP280Task.h"

void BMP280Task::setup(){
bmp= new Adafruit_BMP280();
if (bmp){
    bmp->begin(0x76);
}
}


void BMP280Task::cleanup(){
    if (bmp) delete bmp;
}


void BMP280Task::loop(){
    uint32_t command;
if (xTaskNotifyWait(0, 0, &command, portMAX_DELAY))
  {
    switch (command)
    {
    case 67:
     Serial.print(F("Temperature = "));
    Serial.print(bmp->readTemperature());
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(bmp->readPressure()/133.322);
    Serial.println(" мм рт. ст.");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp->readAltitude(1013.25)); /* Adjusted to local forecast! */
    Serial.println(" m");

    Serial.println();

    break;
    }
  }
}