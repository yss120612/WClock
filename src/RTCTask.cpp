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

//void RTCTask::alarm(uint8_t hh,uint8_t mm){
  // DateTime dt;
  // TimeSpan ts(0,hh,mm,0);
  // dt=dt+ts;
  // Serial.print("Set Alarm 2 to - ");
  // Serial.print(hh);
  // Serial.print(":");
  // Serial.println(mm);
  
  
  // rtc->setAlarm2(dt,DS3231_A2_Hour);
//}

//установка конкретного будильника
void RTCTask::alarm(alarm_t &a){
  a.active=true;
  a.need_alarm=false;
  DateTime dt;
  switch (a.action)
  {
    case NO_ALARM:
    a.active=false;
    break;
    case ONCE_ALARM:
    case EVERYHOUR_ALARM:
      rtc->setAlarm2(dt,DS3231_A2_Hour);  
    case  WDAY_ALARM:
    case  HDAY_ALARM:
    case  EVERYDAY_ALARM:
      rtc->setAlarm2(dt+TimeSpan((a.wday+1)%7,a.hour,a.minute,0),DS3231_A2_Day);
    break;
    case  WD1_ALARM:
    case  WD2_ALARM:
    case  WD3_ALARM:
    case  WD4_ALARM:
    case  WD5_ALARM:
    case  WD6_ALARM:
    case  WD7_ALARM:
    a.need_alarm=true;//установить через минуту
    break;
  }
}

void RTCTask::set_if_need(alarm_t &a){
  if (!a.active || !a.need_alarm) return;
  DateTime dt(rtc->now());
  if (dt.minute()!=a.minute){
    //rtc->setAlarm2(dt+TimeSpan((a.wday+1)%7,a.hour,a.minute,0),DS3231_A2_Day);
    a.need_alarm=false;
  }
  refreshAlarms();
}



//dw 6 суббота. 0 понедельник 1 вторник ...
//void RTCTask::alarm(uint8_t hh,uint8_t mm,uint8_t dw){
//   DateTime dt;
//   TimeSpan ts(dw+1%,hh,mm,0);
//   dt=dt+ts;
//  //DateTime dt(0,dw+2,1,hh,mm,0);
//   Serial.print("Set Alarm 2 to day:");
//   Serial.print(dw);
//   Serial.print(" at time ");

//   Serial.print(hh);
//   Serial.print(":");
//   Serial.println(mm);
//  rtc->setAlarm2(dt,DS3231_A2_Day);
//}


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

bool RTCTask::setupAlarm(uint8_t idx, uint8_t act, uint8_t h, uint8_t m,  period_t p, uint8_t d=0){
if (idx>=ALARMS_COUNT) return false;
alarms[idx].active=true;
alarms[idx].action=act;
alarms[idx].hour=h;
alarms[idx].minute=m;
alarms[idx].period=p;
alarms[idx].wday=d;
alarms[idx].need_alarm=false;
return true;
}

uint8_t RTCTask::refreshAlarms(){
uint8_t index=ALARMS_COUNT;
uint8_t dh,dm,dd;
uint16_t amin,nmin,cdiff,min_diff=7*24*60+1;//week and one minutes
DateTime d=rtc->now();
nmin=d.hour()*60+d.minute();
for (uint8_t i=0;i<ALARMS_COUNT;i++){
  if (!alarms[i].active || alarms[i].need_alarm) continue;
  amin=alarms[i].hour*60+alarms[i].minute;
  switch (alarms[i].period)
  {

  case ONCE_ALARM:
    /* code */
    break;
  case EVERYDAY_ALARM:
  case EVERYHOUR_ALARM: 
  if (amin<=nmin) amin+=24*60;
  cdiff=amin-nmin;
    /* code */
    break;
  default:
    break;
  }
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
        uint8_t d;  
        readPacket(command, &comm, &act, &data);
        switch (comm)
        
        {
        case 11:
          h = (data >>8) & 0x000000FF; 
          m = data & 0x000000FF;
          d = act;  
          if (d>=7) alarm(h,m);
          else alarm(h,m,d); 
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
for (uint8_t i=0;i<ALARMS_COUNT;i++) set_if_need(alarms[i]);


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