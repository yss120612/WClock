#ifndef __SETTINGS__
#define __SETTINGS__
#include <driver/ledc.h>

#define TIME_OFFSET 8 //смещение временной зоны
#define LONG_TIME  60000*60*24 //one per day
#define SHORT_TIME  60000*5 //one per 5 min
#define LED_TIMER_NUM LEDC_TIMER_3
#define BAND_TIMER_NUM LEDC_TIMER_2

#define ENCBTN GPIO_NUM_25
#define ENCS1 GPIO_NUM_27
#define ENCS2 GPIO_NUM_26
#define BOUNCE 20
#define LONGCLICK 1000
#define DOUBLECLICK 700

const uint8_t pins[]={GPIO_NUM_33,GPIO_NUM_32,0,0};
enum blinkmode_t { BLINK_OFF, BLINK_ON, BLINK_TOGGLE, BLINK_05HZ, BLINK_1HZ, BLINK_2HZ, BLINK_4HZ, BLINK_FADEIN, BLINK_FADEOUT, BLINK_FADEINOUT, BLINK_SUNRAISE,BLINK_SUNSET };
const ledc_channel_t channels[]={LEDC_CHANNEL_0,LEDC_CHANNEL_1,LEDC_CHANNEL_2,LEDC_CHANNEL_3};
enum buttonstate_t : uint8_t { NONE_EVENT, BTN_CLICK, BTN_LONGCLICK, PULT_BUTTON, WEB_EVENT, MEM_EVENT,DISP_EVENT, LED_EVENT,ENCODER_EVENT,RTC_EVENT };
enum period_t : uint8_t {ONCE_ALARM, EVERYHOUR_ALARM, EVERYDAY_ALARM,  WDAY_ALARM, HDAY_ALARM, WD7_ALARM,  WD1_ALARM,WD2_ALARM,WD3_ALARM,WD4_ALARM,WD5_ALARM,WD6_ALARM };
static const char dayofweek[] = "SunMonTueWedThuFriSat";

enum flags_t : uint8_t { FLAG_WIFI = 1, FLAG_MQTT = 2 };

#define IR_PIN GPIO_NUM_4 //pin for IR receiver
#define IR_DEVICE 162

#define  AT24C32_ADDRESS 0x57
#define  AT24C32_OFFSET 0x78
enum memaddr_t : uint16_t {CELL0,CELL1,CELL2,CELL3,CELL4,CELL5,CELL6,CELL7}

#define  EEPROM_PAGE_SIZE  32
#define  EEPROM_WORK_SIZE  EEPROM_PAGE_SIZE / 2
#define  EEPROM_WRITE_TIMEOUT  10

#define SEALEVELPRESSURE_HPA (1013.25)



void static readPacket(uint32_t container, uint8_t * btn, uint8_t * value, uint16_t * data){
    *btn   = (container >> 24) & 0x000000FF;
    *value = (container >> 16) & 0x000000FF;
    *data  = container & 0x0000FFFF;
}
 static uint32_t makePacket(uint8_t btn, uint8_t value, uint16_t data){
    uint32_t container;
    container = (btn << 24) & 0xFF000000 | (value<<16) & 0x00FF0000 | data & 0x0000FFFF;
    return container;
}


#define ALARMS_COUNT 10
const uint16_t WEEK=10080;//minutes in week
const uint16_t DAY=1440;// minutes in day

static uint32_t makeAlarm(uint8_t cmd, uint8_t period, uint8_t hour, uint8_t min){
    uint32_t container;
    container = (cmd << 24) & 0xFF000000 | (period<<16) & 0x00FF0000 | (hour << 8) & 0x0000FF00 | min  & 0x000000FF;
    return container;
}




struct  __attribute__((__packed__)) alarm_t{
    bool active:1;
    uint8_t hour:5;
    uint8_t minute:6;
    uint8_t action:4;
    uint8_t wday:4;
    period_t period:4;
};

const uint16_t ALARM_LENGTH=sizeof(alarm_t)*ALARMS_COUNT;
const uint16_t ALARMS_OFFSET=512;

static void getNext(alarm_t &at){
        switch (at.period) {
            case ONCE_ALARM:
            at.active=false;
            break;
            case WDAY_ALARM:
             if (at.wday>=5) at.wday=1;
             else at.wday++;
            break;
            case HDAY_ALARM:
             if (at.wday==6) at.wday=0;
             else at.wday=6;
           break;
            case EVERYDAY_ALARM:
             if (at.wday==6) at.wday=0;
             else at.wday++;
            break;
            case EVERYHOUR_ALARM:
             if (at.hour>=23) at.hour=0;
             else at.hour++;
            break;
            case WD1_ALARM:
            case WD2_ALARM:
            case WD3_ALARM:
            case WD4_ALARM:
            case WD5_ALARM:
            case WD6_ALARM:
            case WD7_ALARM:
            break;
        }
    }

#endif
