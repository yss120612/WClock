#ifndef _WEATHERTASK_h
#define _WEATHERTASK_h
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
//#include <Wire.h>
#include <Task.h>
#include "Settings.h"
//#include "Events.h"
#define weather_request  "http://api.openweathermap.org/data/2.5/weather?q=Irkutsk&appid=7d7b4c4884dc5a5c839debe4488d1712&units=metric"
#define forecast_request   "http://api.openweathermap.org/data/2.5/forecast?q=Irkutsk&appid=7d7b4c4884dc5a5c839debe4488d1712&units=metric"
class WeatherTask: public Task{
public:    
WeatherTask(const char *name, uint32_t stack, QueueHandle_t q, EventGroupHandle_t f, MessageBufferHandle_t m):Task(name, stack){que=q;flg=f;_mess=m;};

protected:
void cleanup() override;
void setup() override;
void loop() override;
DynamicJsonDocument requestURL();
QueueHandle_t que;
EventGroupHandle_t flg;
MessageBufferHandle_t _mess;

};


#endif 