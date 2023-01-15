#include "MEMTask.h"
//#include "Events.h"

void MEMTask::setup()
{
	Wire.begin();
	read_state();
	Serial.print("Length=");
    Serial.println(sizeof(sstate));
	Serial.print("version=");
    Serial.println(sstate.version);
}

void MEMTask::cleanup()
{
}

void MEMTask::read_state()
{
	read(0, (uint8_t *)&sstate, sizeof(sstate));
	if (sstate.version != version)
	{
		sstate.version = version;
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
		write_state();
	}
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
		uint8_t comm, value;
		uint16_t addr;
		readPacket(command, &comm, &value, &addr);
		// comm=1 read, com=2 write com=3 read alarms
		// Serial.print(comm == 1 ? "read" : "write");
		// Serial.print(" addr=");
		// Serial.print(addr);
		// Serial.print(" value=");
		// Serial.println(value);
		switch (comm)
		{

		case 1:
			read(addr, &value, sizeof(value));
			event_t ev;
			ev.state = MEM_EVENT;
			ev.button = addr;
			ev.count = value;
			xQueueSend(que, &ev, portMAX_DELAY);
			break;
		case 2:
			write(addr, &value, sizeof(value));
			break;
		case 200:
			//sendBuffer();
			xMessageBufferSend(alarm_mess, &sstate.alr, ALARM_LENGTH, portMAX_DELAY);
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