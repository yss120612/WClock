 #include <Arduino.h>
#include <MEMTask.h>
#include <IRTask.h>
#include <LEDTask.h>
#include <ENCTask.h>
#include <WiFiTask.h>
#include "HTTP2Task.h"
#include <RTCTask.h>
#include "BMP280Task.h"
#include "LedCubeTask.h"
#include "WeatherTask.h"
#include <Wire.h>
#include <SPI.h>



MEMTask<SystemState_t> * mem;
IRTask * irt;
LEDTask * leds;
ENCTask * enc;
WiFiTask * wifi;
HTTP2Task * http;
RTCTask * rtc;
QueueHandle_t queue;
BMP280Task * bmp280;
LedCubeTask * tablo; 
WeatherTask * weather;

EventGroupHandle_t flags;
MessageBufferHandle_t display_messages;
MessageBufferHandle_t alarm_messages;
MessageBufferHandle_t web_messages;






void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
 unsigned status;
 


//bme.init();
queue= xQueueCreate(16,sizeof(event_t));

flags=xEventGroupCreate();
display_messages=xMessageBufferCreate(100);
alarm_messages=xMessageBufferCreate(SSTATE_LENGTH+4);//=length label
web_messages=xMessageBufferCreate(100);//=length label



mem=new MEMTask<SystemState_t>("MEM",2048,queue,reset_default,process_notify, alarm_messages,web_messages,VERSION,AT24C32_ADDRESS,AT24C32_OFFSET);
mem->resume();
delay(100);
irt= new IRTask("IR",2048,queue,IR_PIN,IR_DEVICE);  
irt->resume();
delay(100);
rtc = new RTCTask("rtc",4096,flags,queue,display_messages,alarm_messages);
rtc->resume();
rtc->setNeedWatch();
rtc->resetAlarms();
rtc->init();
delay(100);
bmp280= new BMP280Task("BMP280",2048);  
bmp280->resume();
delay(100);
leds = new LEDTask("Leds",3072,queue,band_pins,LOW);
leds->resume();
delay(100);
// enc = new ENCTask("Encoder",2048,queue,ENC_A,ENC_B,ENC_BTN,LOW);
// enc->resume();
delay(100);
wifi = new WiFiTask("WiFi",4096,queue,flags);
wifi->resume();
delay(100);
http = new HTTP2Task("http",4096,queue,flags,web_messages);
http->resume();
delay(100);
weather = new WeatherTask("weather",4096,queue,flags,display_messages);
weather->resume();
delay(100);
tablo = new LedCubeTask("tablo",4096,queue,display_messages);
tablo->resume();




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
// void setAlarm(uint8_t no, uint8_t m,uint8_t h=25, bool retry=true, uint8_t wd=10){
        
//         //uint32_t cmd=no << 24 | h<< 16 | m << 8 | retry<<4 | wd;
//         //rtp->notify(cmd); 


// }

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


void web_event(event_t e){
uint32_t cmd;
notify_t nt;
switch(e.button){
  case 1:
     nt.title=1;
     nt.alarm=e.alarm;
     memcpy(&cmd,&nt,sizeof(cmd));
     Serial.printf("from web %d:%d Period=%d Wday=%d Action=%d \n",nt.alarm.hour,nt.alarm.minute,nt.alarm.period, nt.alarm.wday,nt.alarm.action);
     rtc->notify(cmd);
  break;
  case 2:
     nt.title=11;
     nt.packet.var=0;
     nt.packet.value=0;
     memcpy(&cmd,&nt,sizeof(cmd));
     rtc->notify(cmd);
  break;
  case 3:
     nt.title=12;
     nt.packet.var=0;
     nt.packet.value=0;
     memcpy(&cmd,&nt,sizeof(cmd));
     rtc->notify(cmd);
  break;
  case 4:
     nt.title=13;
     nt.packet.var=0;
     nt.packet.value=0;
     memcpy(&cmd,&nt,sizeof(cmd));
     rtc->notify(cmd);
  break;
}
}


void btn_event(event_t e){
  uint32_t cmd;
  notify_t nt;
  switch (e.state){
    case BTN_CLICK:
      Serial.print("multiclick:");
      Serial.print(e.count);
      Serial.println(" click");
    if(e.count==5) {
      //cmd=makeAlarm(11,3,10,12);
     //cmd= makePacket(11,0,(11U<<24)&0xFF000000 |(01U<<16)&0x00FF0000 | (2U & 0x00000FFFF));
     //rtc->notify(cmd);
    } 
    if (e.count==3) Serial.println(getI2Cdevices());
    if (e.count==4) {
      nt.title=67;
      nt.packet.var=0;
      nt.packet.value=0;
      memcpy(&cmd,&nt,sizeof(nt));
      //cmd= makePacket(67,0,0);
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
switch (e.button){
   case 0:
   case 1:
   case 2:
   case 3:
   case 4:
   case 5:
   case 6:
   case 7:
   case 8:
   case 9:
      Serial.print("Event Alarm 2 action:");
      Serial.print(e.alarm.action);
      Serial.print( ", time ");
      Serial.print(e.alarm.hour);
      Serial.print(":");
      Serial.println(e.alarm.minute);
   break;
    
  }
} 


void led_event(event_t e){
  notify_t nt;
  uint32_t comm;
  switch (e.button){
   case 111:
    nt.title=111;
    nt.packet.var=0;
    nt.packet.value=0;
    memcpy(&comm,&nt,sizeof(nt));
    leds->notify(comm);  
  break; 
  case 112:
    nt.title=112;
    nt.packet.var=0;
    nt.packet.value=0;
    memcpy(&comm,&nt,sizeof(nt));
    leds->notify(comm);  
  break; 
   case 113:
    nt.title=113;
    nt.packet.var=0;
    nt.packet.value=0;
    memcpy(&comm,&nt,sizeof(nt));
    leds->notify(comm);  
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


void mem_event(event_t event){
 notify_t notify;
 switch (event.button)
 {
 case MEM_SAVE_00:  
 case MEM_SAVE_01:
 case MEM_SAVE_02:
 case MEM_SAVE_03:
 case MEM_SAVE_04:
 case MEM_SAVE_05:
 case MEM_SAVE_06:
 case MEM_SAVE_07:
 case MEM_SAVE_08:
 case MEM_SAVE_09:
 notify.title=event.button;
 notify.alarm=event.alarm;
 mem->notify(notify);
 break;  
//  case MEM_SAVE_10:
//    notify.title=event.button;
//    notify.packet.value=event.data;
//    mem->notify(notify);
//    break;
//  case MEM_SAVE_11:
//    notify.title=event.button;
//    notify.packet.value=event.data;
//    mem->notify(notify);
//    break;
//   case MEM_SAVE_12:
//    notify.title=event.button;
//    notify.packet.value=event.data;
//    mem->notify(notify);
//   break; 
 
 case MEM_ASK_00:
 case MEM_ASK_01:
 case MEM_ASK_02:
 case MEM_ASK_03:
 case MEM_ASK_04:
 case MEM_ASK_05:
 case MEM_ASK_06:
 case MEM_ASK_07:
 case MEM_ASK_08:
 case MEM_ASK_09:
 case MEM_ASK_10:
 case MEM_ASK_11:
 case MEM_ASK_12:
   notify.title=event.button;
   mem->notify(notify);
 break;

case MEM_READ_00:
case MEM_READ_01:
case MEM_READ_02:
case MEM_READ_03:
case MEM_READ_04:
case MEM_READ_05:
case MEM_READ_06:
case MEM_READ_07:
case MEM_READ_08:
case MEM_READ_09:
notify.title=ALARMSETFROMMEM;
notify.alarm=event.alarm;
rtc->notify(notify);
break;
case MEM_READ_10:
 //notify.title=event.button;
 //notify.packet.value=event.data;
 //active_process->notify(notify);
 break;
case MEM_READ_11:
case MEM_READ_12:
 //notify.title=event.button;
 //notify.packet.value=event.data;
 //active_process->notify(notify);
break;

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
    case MEM_EVENT:
    mem_event(ev);
    break;
    case WEB_EVENT:
    web_event(ev);
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
    case LED_EVENT:
      led_event(ev);
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

