#include "MEMTask.h"
#include "Events.h"

void MEMTask::setup(){
//_address = AT24C32_ADDRESS;
 Wire.begin();  
}


void MEMTask::cleanup(){

}




void MEMTask::loop(){
	alarm_t bf[ALARMS_COUNT];
	if (xMessageBufferReceive(alarm_mess,bf,ALARM_LENGTH,1000)==ALARM_LENGTH){
      write(ALARMS_OFFSET,(uint8_t*)bf,ALARM_LENGTH);
    }

uint32_t command;
  
  if (xTaskNotifyWait(0, 0, &command, 1000))
  {
    uint8_t comm,value;
    uint16_t addr;
	readPacket(command,&comm,&value,&addr);
	//comm=1 read, com=2 write com=3 read alarms
		   Serial.print(comm==1?"read":"write");
		   Serial.print(" addr=");
		   Serial.print(addr);
		   Serial.print(" value=");
		   Serial.println(value);
    switch (comm)
    {

      case 1:
           	read(addr,&value,sizeof(value)); 
          	event_t ev;
           	ev.state=MEM_EVENT;
           	ev.button=addr;
           	ev.count=value;
        	xQueueSend(que,&ev,portMAX_DELAY);
      break;
      case 2:
           write(addr,&value,sizeof(value)); 
      break;
	  case 3:
           sendBuffer(); 
      break;
    }
  }
  
}



void MEMTask::read(uint16_t index, uint8_t* buf, uint16_t len) {
	index+=AT24C32_OFFSET;
	Wire.beginTransmission(AT24C32_ADDRESS);
	Wire.write((index >> 8) & 0x0F);
	Wire.write(index & 0xFF);
	if (Wire.endTransmission() == 0) {
		while (len > 0) {
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

void MEMTask::sendBuffer(){
alarm_t bf[ALARMS_COUNT];
read(ALARMS_OFFSET,(uint8_t*)bf,ALARM_LENGTH);
xMessageBufferSend( alarm_mess,bf,ALARM_LENGTH,1000);
}



void MEMTask::write(uint16_t index, const uint8_t* buf, uint16_t len) {
	
	index+=AT24C32_OFFSET;
	index &= 0x0FFF;
	while (len > 0) {
		uint8_t l;

		l = EEPROM_WORK_SIZE - (index % EEPROM_WORK_SIZE);
		if (l > len)
			l = len;
		len -= l;
		Wire.beginTransmission(AT24C32_ADDRESS);
		Wire.write(index >> 8);
		Wire.write(index & 0xFF);
		while (l--) {
			Wire.write(*buf++);
			++index;
		}
		delay(EEPROM_WRITE_TIMEOUT);
		if (Wire.endTransmission() != 0)
			break;
		while (!Wire.requestFrom(AT24C32_ADDRESS, (uint8_t)1)); // Polling EEPROM ready (write complete)
	}
}