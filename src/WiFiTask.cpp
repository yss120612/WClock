#include "WiFiTask.h"
//#include "Events.h"

void WiFiTask::setup(){
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.onEvent(std::bind(&WiFiTask::wifiOnEvent, this, std::placeholders::_1));
  WiFi.disconnect();
  xEventGroupClearBits(flg, FLAG_WIFI);
}


void WiFiTask::cleanup(){
WiFi.disconnect();
}


void WiFiTask::wifiOnEvent(WiFiEvent_t event) {

  uint32_t result;
  switch (event) {
      case WIFI_EVENT_STA_WPS_ER_SUCCESS:
    
    break;
    case WIFI_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi Сonnection Loss!");
        xEventGroupClearBits(flg, FLAG_WIFI);
        //result=113;
        //xQueueSend(que,&result,portMAX_DELAY);
    break;
    case WIFI_EVENT_STA_CONNECTED:
        //Serial.println("Connected to WiFi!!!");
    break;
    default:
    break;
  }
}

void WiFiTask::loop(){
const uint32_t WIFI_CONNECT_WAIT = 5000; // 5sec.    
const uint32_t WIFI_TIMEOUT = 30000; // 30 sec.
event_t result;
result.state=LED_EVENT;

    if (! WiFi.isConnected()) {
      WiFi.begin(WIFI_SSID, WIFI_PSWD);
      Serial.printf("Connecting to SSID \"%s\"...\n",WIFI_SSID);
      result.button=111;
      xQueueSend(que,&result,portMAX_DELAY);

      {// wait connection WIFI_CONNECT_WAIT
        uint32_t start = millis();
        while ((! WiFi.isConnected()) && (millis() - start < WIFI_CONNECT_WAIT)) {
          vTaskDelay(pdMS_TO_TICKS(1000));
        }
      }
      if (WiFi.isConnected()) {
        portENTER_CRITICAL(&_mutex);
      Serial.print("Connected to WiFi with IP ");
      Serial.println(WiFi.localIP());
      portEXIT_CRITICAL(&_mutex);
      xEventGroupSetBits(flg, FLAG_WIFI);
      result.button=112;
      xQueueSend(que,&result,portMAX_DELAY);

        //result.button=112;
        //xQueueSend(que,&result,portMAX_DELAY);
        } else {
        WiFi.disconnect();
        Serial.println("Failed to connect to WiFi!");
        result.button=113;
        xEventGroupClearBits(flg, FLAG_WIFI);
        xQueueSend(que,&result,portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(WIFI_TIMEOUT));
      }
    } else {//all OK
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
  
}

