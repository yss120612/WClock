#ifndef __MEMTASK__
#define __MEMTASK__
#include "Task.h"
#include <Wire.h>
#include "Settings.h"



//const char WIFI_SSID[] = "Lite5G";
//const char WIFI_PSWD[] = "qweasdzxc";





class MEMTask: public Task
{
public:
MEMTask(const char *name, uint32_t stack,QueueHandle_t q,MessageBufferHandle_t a):Task(name, stack){que=q;alarm_mess=a;}

protected:

void cleanup() override;
void setup() override;
void loop() override;
void read(uint16_t index, uint8_t* buf, uint16_t len);
void write(uint16_t index, const uint8_t* buf, uint16_t len);
void sendBuffer();
    template<typename T> T & get(uint16_t index, T& t) {
		read(index, (uint8_t*)&t, sizeof(T));
		return t;
	}
	template<typename T> const T& put(uint16_t index, const T& t) {
		write(index, (const uint8_t*)&t, sizeof(T));
		return t;
	}

QueueHandle_t que;
MessageBufferHandle_t alarm_mess;
//uint8_t _address;

};
#endif