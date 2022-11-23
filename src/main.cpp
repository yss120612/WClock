#include <Arduino.h>
#include "IRTask.h"
#include "LEDTask.h"
#include "ENCTask.h"
#include "WiFiTask.h"
#include "HTTPTask.h"
#include "RTCTask.h"
#include "BMP280Task.h"
#include <Wire.h>
#include <SPI.h>



#define LEDR GPIO_NUM_32
#define LEDB GPIO_NUM_33



IRTask * irt;
LEDTask * leds;
ENCTask * enc;
WiFiTask * wifi;
HTTPTask * http;
RTCTask * rtc;
QueueHandle_t queue;
BMP280Task * bmp280;
SemaphoreHandle_t btn_semaphore, enc_semaphore;
EventGroupHandle_t flags;
MessageBufferHandle_t message;



void IRAM_ATTR btnISR(){
  xSemaphoreGiveFromISR(btn_semaphore,NULL);
}

void IRAM_ATTR encISR(){
  xSemaphoreGiveFromISR(btn_semaphore,NULL);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
 unsigned status;
 


//bme.init();
queue= xQueueCreate(16,sizeof(uint32_t));
btn_semaphore=xSemaphoreCreateBinary();
flags=xEventGroupCreate();
message=xMessageBufferCreate(100);


attachInterrupt(digitalPinToInterrupt(ENCBTN),btnISR,CHANGE);
attachInterrupt(digitalPinToInterrupt(ENCS1),encISR,RISING);

irt= new IRTask("IR",2048,queue);  
irt->resume();

leds = new LEDTask("Leds",3072,queue,LOW);
leds->resume();

enc = new ENCTask("Encoder",2048,queue,btn_semaphore,LOW);
enc->resume();

wifi = new WiFiTask("WiFi",8192,queue,flags);
wifi->resume();

http = new HTTPTask("http",8192,queue,flags);
http->resume();

rtc = new RTCTask("rtc",4096,flags,queue,message);
rtc->resume();

bmp280= new BMP280Task("BMP280",2048);  
bmp280->resume();


//leds->setup();
//gpio_set_direction((gpio_num_t)LEDR, GPIO_MODE_OUTPUT);
//gpio_set_direction((gpio_num_t)LEDB, GPIO_MODE_OUTPUT);
//gpio_set_level((gpio_num_t)LEDR, LOW);
//gpio_set_level((gpio_num_t)LEDB, HIGH);
//delay(1000);
//leds->setLedMode(0,BLINK_ON);
//leds->setLedMode(1,BLINK_ON);



//blinker=new Blinker(LEDR,LOW,1000U,LEDC_TIMER_3,LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0);
//blinker->setMode(BLINK_FADEINOUT);
}

//h=25 every hour
//wd=10 everyday 0..6
void setAlarm(uint8_t no, uint8_t m,uint8_t h=25, bool retry=true, uint8_t wd=10){
        
        //uint32_t cmd=no << 24 | h<< 16 | m << 8 | retry<<4 | wd;
        //rtp->notify(cmd); 


}

void loop() {
  uint32_t command;
if (xQueueReceive(queue,&command,portMAX_DELAY))
{
  switch(command){
    case 17:leds->notify(command);break;  
     case 18:leds->notify(command);break;  
     case 19:leds->notify(command);break;  
      case 20:leds->notify(command);break;  
      case 8:leds->notify(command);break;  
      case 23:leds->notify(command);break;  
      case 24:leds->notify(command);break;  
      case 25:leds->notify(command);break; 
      //case 30:ntp->notify(command);break; 


      case 51:Serial.println("1 click");break;  
      case 52:Serial.println("2 click");break;  
      case 53:
      {
        Serial.println("3 click");
        setAlarm(1,50);
        
      };
      break;  
      case 54:Serial.println("4 click");break;  
      case 55:Serial.println("5 click");break;  

      case 71:Serial.println("encoder clock");break;  
      case 72:Serial.println("encoder unclock");break;  


      case 91:Serial.println("longclick after 1 click");break;  
      case 92:Serial.println("longclick after 2 click");break;  
      case 93:Serial.println("longclick after 3 click");break;  
      case 94:Serial.println("longclick after 4 click");break;  
      case 95:Serial.println("longclick after 5 click");break;  

      case 111:
        leds->notify(command);
      break;  
      case 112:
        leds->notify(command);
      break;  
      case 113:
        leds->notify(command);
        break;
      case 700:
      case 701:
      case 702:
      case 703:
      {
        Serial.printf("Alarm !!!! %d alarm\n", command-700);
        leds->notify(33);
      } 
      break; 
      case 800:
      {
        Serial.printf("begin update\n");
        leds->notify(33);
      } 
      break; 

  }

}
// put your main code here, to run repeatedly:
}