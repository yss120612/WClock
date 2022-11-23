#ifndef _ENCTASK_h
#define _ENCTASK_h
#include "Task.h"
#define ENCBTN GPIO_NUM_25
#define ENCS1 GPIO_NUM_27
#define ENCS2 GPIO_NUM_26
#define BOUNCE 20
#define LONGCLICK 1000
#define DOUBLECLICK 700

class ENCTask: public Task{
public:    
ENCTask(const char *name, uint32_t stack, QueueHandle_t q, SemaphoreHandle_t s, bool lvl):Task(name, stack){que=q;sem=s;level=LOW;};

protected:
void cleanup() override;
void setup() override;
void loop() override;
QueueHandle_t que;
SemaphoreHandle_t sem;
uint32_t result;
bool level;
uint64_t last_click;
uint64_t last_press;
bool lastbtn;
int8_t lastState;
//void buttonISR();


};

#endif