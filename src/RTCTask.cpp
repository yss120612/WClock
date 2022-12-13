#include "RTCTask.h"
#include "Events.h"

void RTCTask::setup()
{
rtc= new RTC_DS3231();
rtc->begin();
fast_time_interval=true;
last_sync=0;
}

void RTCTask::cleanup()
{
delete rtc;
}

//установка конкретного будильника
void RTCTask::alarm(alarm_t &a){
  a.active=true;
  //a.need_alarm=false;
  DateTime dt;
  
  switch (a.action)
  {
    case  ONCE_ALARM:
    case  EVERYHOUR_ALARM:
    case  EVERYDAY_ALARM:
      rtc->setAlarm2(dt+TimeSpan(0,a.hour,a.minute,0),DS3231_A2_Hour);  
    case  WDAY_ALARM:
    case  HDAY_ALARM:
    case  WD1_ALARM:
    case  WD2_ALARM:
    case  WD3_ALARM:
    case  WD4_ALARM:
    case  WD5_ALARM:
    case  WD6_ALARM:
    case  WD7_ALARM:
      rtc->setAlarm2(dt+TimeSpan(a.wday,a.hour,a.minute,0),DS3231_A2_Day);
    break;
    
    // a.need_alarm=true;//установить через минуту
    // break;
  }
}

void RTCTask::set_if_need(alarm_t &a){
  // if (!a.active || !a.need_alarm) return;
  // DateTime dt(rtc->now());
  // if (dt.minute()!=a.minute){
  //   //rtc->setAlarm2(dt+TimeSpan((a.wday+1)%7,a.hour,a.minute,0),DS3231_A2_Day);
  //   a.need_alarm=false;
  // }
  // refreshAlarms();
}



void RTCTask::alarmFired(uint8_t an){
rtc->clearAlarm(an);
refreshAlarms();

DateTime dt;
if (an==1){
//Serial.println("Alarm 1 fired");
}else if(an==2){

dt=rtc->getAlarm2();

event_t ev;
ev.state=RTC_EVENT;
if(rtc->getAlarm2Mode()==DS3231_A2_Hour)
{
  ev.button=99;
}
else{
  ev.button=dt.dayOfTheWeek();
}

ev.data=makeAlarm(0,ev.button,dt.hour(),dt.minute());
//(dt.hour()<<24) & 0xFF000000 | (dt.minute() <<16) & 0x00FF0000;
//Serial.println(ev.data);
//alarm(dt.hour(),dt.minute()+3);
xQueueSend(que,&ev,portMAX_DELAY);
}
}

bool RTCTask::setupAlarm(uint8_t idx, uint8_t act, uint8_t h, uint8_t m,  period_t p){

  
  Serial.print(h);
  Serial.print(":");
  Serial.println(m);

if (idx>=ALARMS_COUNT) return false;
alarms[idx].active=true;
alarms[idx].action=act;

alarms[idx].minute=m;
alarms[idx].period=p;
alarms[idx].need_alarm=false;
DateTime dt=rtc->now();
uint16_t amin=h*60+m;
uint16_t nmin=dt.hour()*60+dt.minute();
uint8_t dw=dt.dayOfTheWeek();

if (p>=WD7_ALARM) dw=(uint8_t)WD7_ALARM-5;
else if (p==WDAY_ALARM) {
if (dw>5||dw==0) dw=1; 
if (nmin>=amin) dw=dw<5?dw+1:1;
}else if (p==HDAY_ALARM) {
if (dw<6 && dw!=0) dw=6; 
if (nmin>=amin) dw=dw==6?0:6;
}else if (p==ONCE_ALARM || p==EVERYDAY_ALARM){
  // if (nmin>=amin) dw++;
  // if (dw>6) dw=0;
}else if (p==EVERYHOUR_ALARM){
  while (nmin>=amin && h>0) {
    h++;
    if (h>23) h=0;
    amin+=60;
  }
  
}
alarms[idx].hour=h;
alarms[idx].wday=dw;
return true;
}

uint8_t RTCTask::refreshAlarms(){
uint8_t index=ALARMS_COUNT;

uint16_t amin,nmin,cdiff,min_diff=WEEK+1;//week and one minutes

DateTime d=rtc->now();

for (uint8_t i=0;i<ALARMS_COUNT;i++){
  if (!alarms[i].active || alarms[i].need_alarm) continue;
  amin=alarms[i].hour*60+alarms[i].minute;
  nmin=d.hour()*60+d.minute();
 Serial.print("amin=");
Serial.print(amin);
Serial.print(" nmin=");
Serial.println(nmin);

  switch (alarms[i].period)
  {
  case EVERYDAY_ALARM:
  case EVERYHOUR_ALARM: 
  if (amin<=nmin) amin+=DAY;
    break;
  default:
  amin+=((d.dayOfTheWeek()>alarms[i].wday)?(7-alarms[i].wday):alarms[i].wday)*DAY;
  nmin+=d.dayOfTheWeek()*DAY;
  if (amin<=nmin) amin+=WEEK;
    break;
  }
cdiff=amin-nmin;  
Serial.print("Index=");
Serial.print(i);
Serial.print(" Diff=");
Serial.println(cdiff);
if (cdiff<min_diff){
  min_diff=cdiff;
  index=i;
}
}
return index;
}

void RTCTask::loop()
{
    uint32_t command;
    if (xTaskNotifyWait(0, 0, &command, 1000))
    {
        uint8_t comm, act;
        uint16_t data;
        uint8_t h; 
        uint8_t m;
        period_t d;  
        readPacket(command, &comm, &act, &data);
        switch (comm)
        
        {
        case 11:
          h = (data >>8) & 0x000000FF; 
          m = data & 0x000000FF;
          d = (period_t)(act-1);  
          //if (d>=7) alarm(h,m);
          //else alarm(h,m,d); 
          
          setupAlarm(0,1,h,m,d);
          refreshAlarms();
          break;
        case 10:{

            char buf[100];
            size_t si;
            int res;
            if (fast_time_interval){
              res = snprintf(buf, sizeof(buf), "%s","Time is not*syncronized**");
            }else{
              DateTime dt=rtc->now();
              char p [3];
              strncpy(p,dayofweek+3 * dt.dayOfTheWeek(),3);
              res = snprintf(buf, sizeof(buf), "Time %d:%02d:%02d*Date %d-%02d-%d*%s", dt.hour(),dt.minute(),dt.second(),dt.day(),dt.month(),dt.year(),p);
            }
            si=xMessageBufferSend(mess,buf,res,portMAX_DELAY);
            break;}
        }
    }

if (rtc->alarmFired(1)){
alarmFired(1);
}
if (rtc->alarmFired(2)){
alarmFired(2);
}  
//for (uint8_t i=0;i<ALARMS_COUNT;i++) set_if_need(alarms[i]);


if (xEventGroupWaitBits(flg, FLAG_WIFI, pdFALSE, pdTRUE, portMAX_DELAY) & FLAG_WIFI) {    
unsigned long t= millis();    
if (t < last_sync) last_sync=t;
  if (last_sync==0 || t - last_sync > (fast_time_interval ? SHORT_TIME : LONG_TIME))
  {
    last_sync = t;
    fast_time_interval = !update_time_from_inet();
  }
}

}

bool RTCTask::update_time_from_inet()
{
  WiFiUDP *ntpUDP;
  NTPClient *timeClient;
  ntpUDP = new WiFiUDP();

  timeClient = new NTPClient(*ntpUDP, NTPServer, 3600 * TIME_OFFSET, 60000 * 60 * 24);
  timeClient->begin();
  bool result=timeClient->forceUpdate();
  if (result)
  {
    DateTime d(timeClient->getEpochTime());
    rtc->adjust(d);
    Serial.println("Success update time from inet. Time is :" + rtc->now().timestamp());
  }
    

  timeClient->end();
  delete timeClient;
  delete ntpUDP;
  return result;
}