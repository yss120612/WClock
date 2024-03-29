#ifndef _HTTPTASK_h
#define _HTTPTASK_h
#include "Task.h"
#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "Settings.h"

class HTTPTask: public Task{
public:
HTTPTask(const char *name, uint32_t stack,QueueHandle_t q,EventGroupHandle_t f):Task(name, stack){que=q;flg=f;}

protected:
void cleanup() override;
void setup() override;
void loop() override;
void handleRoot(AsyncWebServerRequest * request);
void handleReboot(AsyncWebServerRequest * request);
void handleMain(AsyncWebServerRequest * request);
void handleFile(String path,String type, AsyncWebServerRequest *request);
void handleNotFound(AsyncWebServerRequest * request);
void handleA2W(AsyncWebServerRequest * request);
void handleW2A(AsyncWebServerRequest * request);
// void handleBootstrapCss(AsyncWebServerRequest * request);
// void handleRadioCss(AsyncWebServerRequest * request);
// void handleFontAwesomeCss(AsyncWebServerRequest * request);
// void handleFontAwesomeFontsWoff(AsyncWebServerRequest * request);
// void handleJqueryJs(AsyncWebServerRequest * request);
// void handleBootstrapJs(AsyncWebServerRequest * request);
void handleUpd(AsyncWebServerRequest * request);
void handleUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
void handleSpiffs(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
void handleLog(AsyncWebServerRequest * request);
void var(String n, String v);
String getI2Cdevices();
QueueHandle_t que;
EventGroupHandle_t flg;
AsyncWebServer * server;
};
#endif

