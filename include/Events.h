#ifndef _EVENTS_h
#define _EVENTS_h
#include <Arduino.h>
#include "Config.h"




struct event_t {
    buttonstate_t state;
    uint16_t button;
    uint8_t count;
    //int8_t type;
    uint32_t data;
  //  volatile long wait_time;
  };

#endif