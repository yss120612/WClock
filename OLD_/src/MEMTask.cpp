#include "MEMTask.h"
//#include "Events.h"

void MEMTask::setup()
{
	Wire.begin();
	read_state();
}

void MEMTask::cleanup()
{
}

void MEMTask::read_state()
{
	read(0, (uint8_t *)&sstate, sizeof(sstate));
		//Serial.printf("Version1=%d Version2=%d\n",VER,sstate.version);

	if (VER!=sstate.version)
	{
		
		sstate.relay1 = 0;
		sstate.relay2 = 0;
		sstate.relay3 = 0;
		sstate.relay4 = 0;
		sstate.br1 = 0;
		sstate.br2 = 0;
		sstate.br3 = 0;
		for (uint8_t i = 0; i < ALARMS_COUNT; i++)
		{
			sstate.alr[i].action = 0;
			sstate.alr[i].period = ONCE_ALARM;
			sstate.alr[i].active = false;
			sstate.alr[i].hour = 0;
			sstate.alr[i].minute = 0;
			sstate.alr[i].wday = 0;
		}
		sstate.version = VER;
		write_state();
	}
	//Serial.printf("version=%d rel1=%d rel2=%d rel3=%d rel4=%d br1=%d br1=%d br1=%d\n",sstate.version,sstate.relay1,sstate.relay2,sstate.relay3,sstate.relay4,sstate.br1,sstate.br2,sstate.br3);
	// for (uint8_t ii = 0; ii < ALARMS_COUNT; ii++)
	// 	{
	// 		Serial.printf("idx=%d active=%d %d:%d Period=%d Wday=%d Action=%d \n",ii,sstate.alr[ii].active, sstate.alr[ii].hour,sstate.alr[ii].minute,sstate.alr[ii].period, sstate.alr[ii].wday,sstate.alr[ii].action);
	// 	}
}

void MEMTask::write_state()
{
	write(0, (uint8_t *)&sstate, sizeof(sstate));
}

void MEMTask::loop()
{


	uint32_t command;

	if (xTaskNotifyWait(0, 0, &command, portMAX_DELAY))
	{
		notify_t nt;
		memcpy(&nt,&command,sizeof(command));
		switch (nt.title)
		{

		case 1:
			//read(addr, &value, sizeof(value));
			//event_t ev;
			//ev.state = MEM_EVENT;
			//ev.button = addr;
			//ev.count = value;
			//xQueueSend(que, &ev, portMAX_DELAY);
			break;
		case 2:
			//write(addr, &value, sizeof(value));
			break;
		case 100:	
		case 101:	
		case 102:	
		case 103:	
		case 104:	
		case 105:	
		case 106:	
		case 107:	
		case 108:	
		case 109:	
		sstate.alr[nt.title-100]=nt.alarm;
		//Serial.printf("alarm to save active=%d %d:%d Period=%d Wday=%d Action=%d \n",nt.alarm.active, nt.alarm.hour,nt.alarm.minute,nt.alarm.period, nt.alarm.wday,nt.alarm.action);
		write_state();
		break;
		case 200:
	   		xMessageBufferSend(alarm_mess, &sstate.alr[0], ALARM_LENGTH, portMAX_DELAY);
			break;
		case 201:
		for (uint8_t i = 0; i < ALARMS_COUNT; i++)
			{
			sstate.alr[i].action = 0;
			sstate.alr[i].period = ONCE_ALARM;
			sstate.alr[i].active = false;
			sstate.alr[i].hour = 0;
			sstate.alr[i].minute = 0;
			sstate.alr[i].wday = 0;
			}
			write_state();
	   		xMessageBufferSend(alarm_mess, &sstate.alr[0], ALARM_LENGTH, portMAX_DELAY);
			break;	
		}
	}
}

void MEMTask::read(uint16_t index, uint8_t *buf, uint16_t len)
{
	index += AT24C32_OFFSET;
	Wire.beginTransmission(AT24C32_ADDRESS);
	Wire.write((index >> 8) & 0x0F);
	Wire.write(index & 0xFF);
	if (Wire.endTransmission() == 0)
	{
		while (len > 0)
		{
			uint8_t l;

			l = EEPROM_WORK_SIZE;
			if (l > len)
				l = len;
			len -= l;
			Wire.requestFrom(AT24C32_ADDRESS, l);
			delay(EEPROM_WRITE_TIMEOUT);
			while (l--)
				*buf++ = Wire.read();
		}
	}
}



void MEMTask::write(uint16_t index, const uint8_t *buf, uint16_t len)
{

	index += AT24C32_OFFSET;
	index &= 0x0FFF;
	while (len > 0)
	{
		uint8_t l;

		l = EEPROM_WORK_SIZE - (index % EEPROM_WORK_SIZE);
		if (l > len)
			l = len;
		len -= l;
		Wire.beginTransmission(AT24C32_ADDRESS);
		Wire.write(index >> 8);
		Wire.write(index & 0xFF);
		while (l--)
		{
			Wire.write(*buf++);
			++index;
		}
		delay(EEPROM_WRITE_TIMEOUT);
		if (Wire.endTransmission() != 0)
			break;
		while (!Wire.requestFrom(AT24C32_ADDRESS, (uint8_t)1))
			; // Polling EEPROM ready (write complete)
	}
}