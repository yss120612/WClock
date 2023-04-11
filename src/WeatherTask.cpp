#include "WeatherTask.h"


void WeatherTask::setup(){

}

void WeatherTask::loop(){
    xEventGroupWaitBits(flg,FLAG_WIFI,false,false,portMAX_DELAY);
    event_t ev;
    DynamicJsonDocument jd=requestURL();
    if (!jd.isNull()){
    int8_t temp=rint((double)jd["main"]["temp"]);
    uint8_t  hum=jd["main"]["humidity"];
    uint16_t  press=jd["main"]["pressure"];
    //Serial.printf("t=%d h=%d p=%d\n",temp,hum,press);
    ev.button=2;
    ev.data=(hum<<24) & 0xFF000000 | (temp << 16) &0x00FF0000 | press & 0xFFFF;
    xMessageBufferSend(_mess, &ev, sizeof(event_t), portMAX_DELAY);
    }
    vTaskDelay(pdMS_TO_TICKS(WEATHER_INTERVAL));

}

void WeatherTask::cleanup(){
    
}

DynamicJsonDocument WeatherTask::requestURL()
 
{
  //Serial.println("Connecting to domain: " + String(host));
   WiFiClient client; 
    HTTPClient http;
    http.begin(client,weather_request);
    int httpCode = http.GET();
    DynamicJsonDocument doc(1024);
  if (httpCode > 0) { 
    //response = http.getString();
    //Serial.println(response);
    deserializeJson(doc, http.getString().c_str());
  }
  
  
  //Serial.println("Connected!");
    http.end();
    client.stop();
  return doc;
}