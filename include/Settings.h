#ifndef __SETTINGS__
#define __SETTINGS__
#include <driver/ledc.h>
#define DEBUGG1
#include <GlobalSettings.h>

#define VERSION 7

const uint8_t band_pins[]={GPIO_NUM_33,GPIO_NUM_32,0,0};




//static const char dayofweek_1[] = "SunMonTueWedThuFriSat";

#define IR_PIN GPIO_NUM_4 //pin for IR receiver
#define IR_DEVICE 162

#define ENC_A GPIO_NUM_17
#define ENC_B GPIO_NUM_18
#define ENC_BTN GPIO_NUM_16

#define  AT24C32_ADDRESS 0x57
#define  AT24C32_OFFSET 0x78

#define  EEPROM_PAGE_SIZE  32
#define  EEPROM_WORK_SIZE  EEPROM_PAGE_SIZE / 2
#define  EEPROM_WRITE_TIMEOUT  10

#define SEALEVELPRESSURE_HPA (1013.25)



// void static readPacket(uint32_t container, uint8_t * btn, uint8_t * value, uint16_t * data){
//     *btn   = (container >> 24) & 0x000000FF;
//     *value = (container >> 16) & 0x000000FF;
//     *data  = container & 0x0000FFFF;
// }
//  static uint32_t makePacket(uint8_t btn, uint8_t value, uint16_t data){
//     uint32_t container;
//     container = (btn << 24) & 0xFF000000 | (value<<16) & 0x00FF0000 | data & 0x0000FFFF;
//     return container;
// }

//const uint8_t band_pins[] = {GPIO_NUM_25, GPIO_NUM_33, GPIO_NUM_32, 0};




#define MAX_CS_PIN GPIO_NUM_5

#define  AT24C32_ADDRESS 0x57
#define  AT24C32_OFFSET 0xF0



struct __attribute__((__packed__)) SystemState_t
{
    uint8_t version : 8;
    alarm_t alr[ALARMS_COUNT];
	// uint16_t ac_duration;
	// float ac_temperature;
	// uint8_t ac_forsajpower;
     uint8_t crc;
	
};
static uint8_t process_notify(SystemState_t * ss, event_t * event, notify_t nt){
	uint8_t i;
	
switch (nt.title)
	{
		case MEM_ASK_00://timers
		case MEM_ASK_01:
		case MEM_ASK_02:
		case MEM_ASK_03:
		case MEM_ASK_04:
		case MEM_ASK_05:
		case MEM_ASK_06:
		case MEM_ASK_07:
		case MEM_ASK_08:
		case MEM_ASK_09:
			event->state=MEM_EVENT;
			event->button=MEM_READ_00+nt.title-MEM_ASK_00;
			event->alarm=ss->alr[nt.title-MEM_ASK_00];
			
		break;
		// case MEM_ASK_10://autoclav duration
		// 	event->state=MEM_EVENT;
		// 	event->button=MEM_READ_10;
		// 	event->data=ss->ac_duration;
		// break;
		// case MEM_ASK_11://autoclav temperature
		// 	event->state=MEM_EVENT;
		// 	event->button=MEM_READ_11;
		// 	event->data=ss->ac_temperature;
			
		// break;
		// case MEM_ASK_12://autoclav temperature
		// 	event->state=MEM_EVENT;
		// 	event->button=MEM_READ_12;
		// 	event->data=ss->ac_forsajpower;
			
		// break;
		// case MEM_ASK_13:
		// 	event->state=MEM_EVENT;
		// 	for (i=0;i<ALARMS_COUNT;i++)
		// 	{
		// 	event->button=MEM_READ_00+i;
		// 	event->alarm=ss->alr[i];
		// 	}
		// break;

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
			ss->alr[nt.title-MEM_SAVE_00]=nt.alarm;
		break;
		// case MEM_SAVE_10:
		// 	ss->ac_duration=nt.packet.value;
		// 	break;
		// case MEM_SAVE_11:
		// 	ss->ac_temperature=nt.packet.value;
		// break;
		// case MEM_SAVE_12:
		// 	ss->ac_forsajpower=nt.packet.value;
		// break;
    }
	if (nt.title<MEM_ASK_00) return 1;//MEM_READ
	if (nt.title<MEM_SAVE_00) return 2;//MEM_ASK
	return 3;
}
const uint16_t SSTATE_LENGTH = sizeof(SystemState_t);


static void reset_default(SystemState_t * ss){
		ss->version=VERSION;
		for (uint8_t i=0;i<ALARMS_COUNT;i++){
			ss->alr[i].action=i;
			ss->alr[i].active=false;
			ss->alr[i].hour=0;
			ss->alr[i].minute=0;
			ss->alr[i].wday=0;
			ss->alr[i].period=ONCE_ALARM;
		}
		// ss->ac_duration=40;
		// ss->ac_temperature=115;
		// ss->ac_forsajpower=50;	
		// ss->crc=crc8((uint8_t*)ss, sizeof(ss));
	}




#define WEATHER_INTERVAL 1000*60*10 //10 minutes
#endif
