#include <Arduino.h>
#include "MEMTask.h"
#include "IRTask.h"
#include "LEDTask.h"
#include "ENCTask.h"
#include "WiFiTask.h"
#include "HTTPTask.h"
#include "RTCTask.h"
#include "BMP280Task.h"
#include <Wire.h>
#include <SPI.h>



MEMTask * mem;
IRTask * irt;
LEDTask * leds;
ENCTask * enc;
WiFiTask * wifi;
HTTPTask * http;
RTCTask * rtc;
QueueHandle_t queue;
BMP280Task * bmp280;

EventGroupHandle_t flags;
MessageBufferHandle_t message;






void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
 unsigned status;
 


//bme.init();
queue= xQueueCreate(16,sizeof(event_t));

flags=xEventGroupCreate();
message=xMessageBufferCreate(100);



mem= new MEMTask("Memory",2048,queue);  
mem->resume();
delay(100);
irt= new IRTask("IR",2048,queue);  
irt->resume();
delay(100);
rtc = new RTCTask("rtc",4096,flags,queue,message);
rtc->resume();
delay(100);
bmp280= new BMP280Task("BMP280",2048);  
bmp280->resume();
delay(100);
leds = new LEDTask("Leds",3072,queue,LOW);
leds->resume();
delay(100);
enc = new ENCTask("Encoder",2048,queue,LOW);
enc->resume();
delay(100);
wifi = new WiFiTask("WiFi",8192,queue,flags);
wifi->resume();
delay(100);
http = new HTTPTask("http",8192,queue,flags);
http->resume();




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

String getI2Cdevices(){
    int error;
	String res="I2C device found at address<ul>";
	
	
	uint8_t count=0;
    for (uint8_t address = 1; address < 127; address++ )  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)    {
		res+="<li>";
		res+=String(address,16);
		res+="</li>";
		count++;
      
    }
    }
    res+="<li><b>Ttal devices ";
	res+=String(count);
	res+="</b></li></ul>";
	return res;
}

void encoder_event(event_t e){
switch(e.data){
  case 1:
    Serial.println("encoder clock");
  break;
  case 2:
    Serial.println("encoder unclock");
  break;
}
}

void btn_event(event_t e){
  uint32_t cmd;
  switch (e.state){
    case BTN_CLICK:
      Serial.print("multiclick:");
      Serial.print(e.count);
      Serial.println(" click");
    if(e.count==5) {
     cmd= makePacket(11,22,55);
     rtc->notify(cmd);
    } 
    if (e.count==3) Serial.println(getI2Cdevices());
    if (e.count==4) {
      cmd= makePacket(67,0,0);
      bmp280->notify(cmd);}
      break;;
    break;
    case BTN_LONGCLICK:
      Serial.print("longclick after");
      Serial.print(e.count);
      Serial.println(" click");
      if (e.count==5) ESP.restart();
    break;
  }
}


void rtc_event(event_t e){
  uint32_t cmd;
  switch (e.button){
    case 99:
      uint8_t h=(e.data>>16) & 0x00FF;
      uint8_t m=e.data & 0x00FF;
      Serial.print("Event Alarm 2 at - ");
      Serial.print(h);
      Serial.print(":");
      Serial.println(m);
      Serial.println(e.data);
    break;
  }
} 


void pult_event(event_t e){
switch(e.button){
  case 17:leds->notify(e.button);break;  
  case 18:leds->notify(e.button);break;  
  case 19:leds->notify(e.button);break;  
  case 20:leds->notify(e.button);break;  
  case 8:leds->notify(e.button);break;  
  case 67:bmp280->notify(e.button);break;  
  case 23:leds->notify(e.button);break;  
  case 24:leds->notify(e.button);break;  
  case 25:leds->notify(e.button);break; 
}
}



void loop() {
  event_t ev;
if (xQueueReceive(queue,&ev,portMAX_DELAY))
{
  switch (ev.state){
    case ENCODER_EVENT:
    encoder_event(ev);
    break;
    case BTN_CLICK:
    case BTN_LONGCLICK:
    btn_event(ev);
    break;
    case PULT_BUTTON:
      pult_event(ev);
    break;
    case RTC_EVENT:
      rtc_event(ev);
    break;
  }
  // switch(ev.button){
  //   case 17:leds->notify(ev.button);break;  
  //    case 18:leds->notify(ev.button);break;  
  //    case 19:leds->notify(ev.button);break;  
  //     case 20:leds->notify(ev.button);break;  
  //     case 8:leds->notify(ev.button);break;  
  //     case 67:bmp280->notify(ev.button);break;  
  //     case 23:leds->notify(ev.button);break;  
  //     case 24:leds->notify(ev.button);break;  
  //     case 25:leds->notify(ev.button);break; 
  //     //case 30:ntp->notify(command);break; 


  //     case 51:Serial.println("1 click");break;  
  //     case 52:Serial.println("2 click");
  //     Serial.println(getI2Cdevices());
  //     break;  
  //     case 53:
  //     {
  //       Serial.println("3 click");
  //       setAlarm(1,50);
  //     };
  //     break;  
  //     case 54:Serial.println("4 click");break;  
  //     case 55:Serial.println("5 click");break;  

  //     case 71:Serial.println("encoder clock");break;  
  //     case 72:Serial.println("encoder unclock");break;  


  //     case 91:Serial.println("longclick after 1 click");break;  
  //     case 92:Serial.println("longclick after 2 click");break;  
  //     case 93:Serial.println("longclick after 3 click");break;  
  //     case 94:Serial.println("longclick after 4 click");break;  
  //     case 95:Serial.println("longclick after 5 click");break;  

  //     case 111:
  //       leds->notify(ev.button);
  //     break;  
  //     case 112:
  //       leds->notify(ev.button);
  //     break;  
  //     case 113:
  //       leds->notify(ev.button);
  //       break;
  //     case 700:
  //     case 701:
  //     case 702:
  //     case 703:
  //     {
  //       Serial.printf("Alarm !!!! %d alarm\n", ev.button-700);
  //       leds->notify(33);
  //     } 
  //     break; 
  //     case 800:
  //     {
  //       Serial.printf("begin update\n");
  //       leds->notify(33);
  //     } 
  //     break; 

  // }

}


// put your main code here, to run repeatedly:
}

