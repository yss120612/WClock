#include <esp_log.h>
#include "Led.h"

//static const char TAG[] = "Blinker";



static int8_t abs8(int8_t value) {
  if (value < 0)
    return -value;
  return value;
}


Led::Led(uint8_t pin, uint8_t level, ledc_channel_t channel){
  _mode = BLINK_OFF;
 // _timer = timer;
  _level=level;
  _channel = channel;
  _value = 0;
  _pin=pin;
  _brightness=255;
  
   //pinMode(pin,OUTPUT);
   gpio_set_direction((gpio_num_t)pin, GPIO_MODE_OUTPUT);
   //gpio_set_level((gpio_num_t)pin, !level);
   ledc_channel_config_t ledc_channel_cfg1;
          ledc_channel_cfg1.gpio_num=_pin;
          ledc_channel_cfg1.speed_mode=SPEED_MODE;
          ledc_channel_cfg1.intr_type=LEDC_INTR_DISABLE;
          ledc_channel_cfg1.channel=_channel;
          ledc_channel_cfg1.timer_sel=TIMER_NUM;
          ledc_channel_cfg1.duty= _level ? 0 : _brightness;
          ledc_channel_cfg1.hpoint=255;
          ledc_channel_cfg1.flags.output_invert = !_level;
    if (ledc_channel_config(&ledc_channel_cfg1)!=ESP_OK){
   //  Serial.println("ERROR CONFIGURE CHANNEL");
    }else{
    // Serial.println("OK CONFIGURE CHANNEL");
    }
    //if (!level) ledc_d
}

Led::~Led() {
}


void Led::setMode(blinkmode_t mode) {
 // if (_mode==mode && mode!=BLINK_TOGGLE) return;

  // if (_mode >= BLINK_FADEIN) {
  //         ledc_stop(SPEED_MODE, _channel, ! _level);
  //         gpio_set_direction((gpio_num_t)_pin, GPIO_MODE_OUTPUT);
  //     } 
  //if (_timer && esp_timer_is_active(_timer)) esp_timer_stop(_timer);
  
  // ledc_stop(SPEED_MODE, _channel, ! _level);

  // uint32_t period;
  // setValue(0);
  // switch (mode)
  //   {
  //   case BLINK_OFF:
  //        _curr_brightness=0; 
  //   case BLINK_ON:
  //        _curr_brightness=_brightness;
  //        ////gpio_set_level((gpio_num_t)_pin, mode == BLINK_ON ? _level : ! _level);
  //       break;
  //   case BLINK_TOGGLE:
  //        _curr_brightness=_curr_brightness<_brightness?_brightness:0;
  //        //gpio_set_level((gpio_num_t)_pin, ! gpio_get_level((gpio_num_t)_pin));
  //      break;
  //   case BLINK_05HZ:
  //   case BLINK_1HZ:
  //   case BLINK_2HZ:
  //   case BLINK_4HZ:
  //       _curr_brightness=0; 
  //         break;
  //   case BLINK_FADEIN:
  //   case BLINK_FADEOUT:
  //   case BLINK_FADEINOUT:
  //   case BLINK_SUNRAISE:
  //   case BLINK_SUNSET:
  //       _curr_brightness=0; 
  //   break;
  //   }
  //       ledc_channel_config_t ledc_channel_cfg1;
  //         ledc_channel_cfg1.gpio_num=_pin;
  //         ledc_channel_cfg1.speed_mode=SPEED_MODE;
  //         ledc_channel_cfg1.intr_type=LEDC_INTR_DISABLE;
  //         ledc_channel_cfg1.channel=_channel;
  //         ledc_channel_cfg1.timer_sel=TIMER_NUM;
  //         ledc_channel_cfg1.duty= _level ? _curr_brightness : 255-_curr_brightness;
        
  //       if (ledc_channel_config(&ledc_channel_cfg1) != ESP_OK) {
  //         return;
  //        }
    

  _mode=mode;
  _value = 0;
}

