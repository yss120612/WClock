#include "IRTask.h"

void IRTask::setup(){
irrecv = new IRrecv(IR_PIN);
irrecv->enableIRIn();
}

void IRTask::loop(){
    if (irrecv->decode(&dres))
{
        
        
        uint32_t command=dres.command;
        if (command){
        lock();
        Serial.print("Command="); 
        Serial.print(dres.command);
        Serial.print(" Address="); 
        Serial.print(dres.address);
        Serial.print(" Type="); 
        Serial.println(dres.decode_type);
        xQueueSend(que,&command,portMAX_DELAY);
        unlock();
        }
        irrecv->resume();
    }
       
    //Serial.println(irrecv!=NULL);
    delay(100);
}

void IRTask::cleanup(){
    irrecv->disableIRIn();
    delete irrecv;
}