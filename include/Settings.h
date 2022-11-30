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
static const char dayofweek[] = "SunMonTueWedThuFriSat";

enum flags_t : uint8_t { FLAG_WIFI = 1, FLAG_MQTT = 2 };

#define IR_PIN GPIO_NUM_4 //pin for IR receiver
#define IR_DEVICE 162

#define  AT24C32_ADDRESS 0x57
#define  AT24C32_OFFSET 0x78
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


#endif
