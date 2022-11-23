#include "ENCTask.h"

void ENCTask::setup(){
 gpio_set_direction(ENCBTN, GPIO_MODE_INPUT);
 gpio_set_direction(ENCS1, GPIO_MODE_INPUT);
 gpio_set_direction(ENCS2, GPIO_MODE_INPUT);
 lastbtn=false;
};
void ENCTask::loop(){

    if (xSemaphoreTake(sem,portMAX_DELAY)){
        bool btnNow=gpio_get_level(ENCBTN);
        if (lastbtn!=btnNow)//button semaphore
        {
            lastbtn=btnNow;
        if (btnNow==level){
            last_press=millis();
            if (last_press-last_click >= LONGCLICK) result=51;
        }else{
            uint64_t released=millis();
            if (released-last_press <= BOUNCE) return;
            if (released-last_press < LONGCLICK) {//5 click max
                if (released-last_click <= DOUBLECLICK) result++;
                xQueueSend(que,&result,portMAX_DELAY);
                last_click=released;
            }else{
                result+=40;
                xQueueSend(que,&result,portMAX_DELAY);
            }
        }
    }else{//encoder semaphore
        bool s1=gpio_get_level(ENCS1);
        bool s2=gpio_get_level(ENCS2);
       if (s1){
        if(s2) result=71; else result=72;
        xQueueSend(que,&result,portMAX_DELAY);
       }
       
    }
    }
   
};

void ENCTask::cleanup(){

};

