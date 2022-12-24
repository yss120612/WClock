#ifndef _RTCTASK_h
#define _RTCASK_h
#include <Arduino.h>
#include "Task.h"
//#include "Settings.h"
#include <WiFiUdp.h>
#include <NtpClient.h>
#include "RTClib.h"
#include "Settings.h"


//#define LED_PIN GPIO_NUM_32
//const ledc_channel_t channels[]={LEDC_CHANNEL_0,LEDC_CHANNEL_1,LEDC_CHANNEL_2,LEDC_CHANNEL_3};

struct alarm_t{
    bool active;
    uint8_t action;
    uint8_t hour;
    uint8_t minute;
    uint8_t wday;
    period_t period;
    alarm_t(){
        active=false;
        action=0;
        hour=0;
        minute=0;
        wday=0;
        period=ONCE_ALARM;
    }
   
    static void getNext(alarm_t &at){
        switch (at.period) {
            ONCE_ALARM:
            at.active=false;
            break;
            WDAY_ALARM:
             if (at.wday>=5) at.wday=1;
             else at.wday++;
            break;
            HDAY_ALARM:
             if (at.wday==6) at.wday=0;
             else at.wday=6;
            break;
            EVERYDAY_ALARM:
             if (at.wday==6) at.wday=0;
             else at.wday++;
            break;
            EVERYHOUR_ALARM:
             if (at.hour>=23) at.hour=0;
             else at.hour++;
            break;
            WD1_ALARM:
            WD2_ALARM:
            WD3_ALARM:
            WD4_ALARM:
            WD5_ALARM:
            WD6_ALARM:
            WD7_ALARM:
            break;
        }
    }
};


class RTCTask: public Task{
public:   
    RTCTask(const char *name, uint32_t stack,EventGroupHandle_t f,QueueHandle_t q,MessageBufferHandle_t m):Task(name, stack){que=q;flg=f;mess=m;}
    
   
    //blinkmode_t get_blinkmode(uint8_t idx) {return led[idx]->getMode();}
protected:
    //void setLedMode(uint8_t ledN, blinkmode_t bm);
    //void alarm(uint8_t hh,uint8_t mm);
    //void alarm(uint8_t hh,uint8_t mm,uint8_t dw);
    uint8_t findAndSetNext(DateTime dt, Ds3231Alarm2Mode mode);
    bool setupAlarm(uint8_t idx, uint8_t act, uint8_t h, uint8_t m,  period_t p);
    uint8_t refreshAlarms();
    void alarm(alarm_t &a);
 

    void alarmFired(uint8_t aNo);
    bool update_time_from_inet();
    void cleanup() override;
    void setup() override;
    void loop() override;
    int32_t last_sync;
    boolean fast_time_interval; 
    QueueHandle_t que;
    EventGroupHandle_t flg;
    RTC_DS3231 * rtc;
    const char * NTPServer = "pool.ntp.org";
    MessageBufferHandle_t mess;
    alarm_t alarms[ALARMS_COUNT];
};
#endif 