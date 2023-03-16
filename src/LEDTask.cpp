#include "LEDTask.h"

void LEDTask::setup()
{

  _timer = xTimerCreate("LedTimer", pdMS_TO_TICKS(50), pdTRUE, static_cast<void *>(this), timerCb);

  ledc_timer_config_t ledc_timer_cfg = {
      .speed_mode = SPEED_MODE,
      {.duty_resolution = LEDC_TIMER_8_BIT},
      .timer_num = TIMER_NUM,
      .freq_hz = 1000};

  if (ledc_timer_config(&ledc_timer_cfg) != ESP_OK)
  {
    xTimerDelete(_timer, 0);
    // Serial.println("ERROR CONFIGURE  LEDC TIMER");
    return;
  }
  else
  {
    // Serial.println("OK CONFIGURE LEDC TIMER");
  }

  for (uint8_t i = 0; i < LEDS_COUNT; i++)
  {
    if (band_pins[i] > 0)
    {

      led[i] = new Led(band_pins[i], _level, channels[i]);
    }
    else
      led[i] = NULL;
  }
}

void LEDTask::save(uint8_t idx)
{
  event_t ev;
  ev.state = MEM_EVENT;
  ev.button = 20 + idx;
  ev.count = led[idx]->getMode();
  ev.data = led[idx]->getBrightness();
  xQueueSend(que, &ev, portMAX_DELAY);
}

void LEDTask::setLedBrightness(uint8_t ledN, uint8_t br, bool sav)
{
  if (ledN >= LEDS_COUNT || !led[ledN])
  {
#ifdef DEBUGG
    Serial.print(ledN);
    Serial.println(" LED IS NULL (in setBrightness)");
#endif
    return;
  }
  uint8_t duty;
  led[ledN]->setBrightness(br);
  if (led[ledN]->getMode() <= BLINK_TOGGLE)
  {
    switch (led[ledN]->getMode())
    {
    case BLINK_OFF:
      duty = 0;
      break;
    case BLINK_ON:
      duty = led[ledN]->getBrightness();
      break;
    case BLINK_TOGGLE:
      duty = ledc_get_duty(SPEED_MODE, led[ledN]->getChannel()) > 0 ? led[ledN]->getBrightness() : 0;
      break;
    }
    reinitDuty(ledN, duty);
  }
  if (sav)
    save(ledN);
}

void LEDTask::setLedMode(uint8_t LedN, blinkmode_t bm, bool sav)
{
  if (LedN < LEDS_COUNT && led[LedN])
    led[LedN]->setMode(bm);
  else
  {
#ifdef DEBUGG
    Serial.print(LedN);
    Serial.println(" LED IS NULL (in setMode)");
#endif
    return;
  };
  xTimerStop(_timer, 0);
  bool need_timer = false;
  uint8_t duty;
  for (int8_t i = 0; i < LEDS_COUNT; i++)
  {
    if (!led[i])
      continue;
    need_timer = need_timer | led[i]->getMode() > BLINK_TOGGLE;
  }

  if (led[LedN]->getMode() <= BLINK_TOGGLE)
  {
    switch (led[LedN]->getMode())
    {
    case BLINK_OFF:
      duty = 0;
      break;
    case BLINK_ON:
      duty = led[LedN]->getBrightness();
      break;
    case BLINK_TOGGLE:
      duty = ledc_get_duty(SPEED_MODE, led[LedN]->getChannel()) > 0 ? 0 : led[LedN]->getBrightness();
      break;
    }
    reinitDuty(LedN, duty);
  }

  if (need_timer)
  {
    xTimerStart(_timer, 0);
  }
  if (sav)
    save(LedN);
}

void LEDTask::loop()
{
  uint32_t command;
  notify_t nt;
  memcpy(&nt, &command, sizeof(command));
  if (xTaskNotifyWait(0, 0, &command, portMAX_DELAY))
  {
    memcpy(&nt, &command, sizeof(command));

    switch (nt.title)
    {
    case 1:
    case 2:
    case 3: // init leds from memory
      setLedMode(nt.title - 1, (blinkmode_t)nt.packet.var, false);
      setLedBrightness(nt.title - 1, nt.packet.value, false);
      break;

    case 11:
    case 12:
    case 13:
      setLedMode(0, BLINK_ON);
      setLedBrightness(0, 64);
      setLedMode(1, BLINK_ON);
      setLedBrightness(1, 128);
      setLedMode(2, BLINK_ON);
      setLedBrightness(2, 255);
      break;
    case 14:
      setLedMode(0, BLINK_SUNSET);
      setLedBrightness(0, 255);
      setLedMode(1, BLINK_FADEINOUT);
      setLedBrightness(1, 255);
      setLedMode(2, BLINK_SUNRAISE);
      setLedBrightness(2, 255);
      break;
    case 15:
      setLedMode(0, BLINK_1HZ);
      setLedBrightness(0, 255);
      setLedMode(1, BLINK_05HZ);
      setLedBrightness(1, 255);
      setLedMode(2, BLINK_4HZ);
      setLedBrightness(2, 255);
      break;
    case 16:
      setLedMode(0, BLINK_ON);
      setLedBrightness(0, 8);
      setLedMode(1, BLINK_ON);
      setLedBrightness(1, 8);
      setLedMode(2, BLINK_ON);
      setLedBrightness(2, 8);
      break;
    case 17:
      setLedMode(0, BLINK_OFF);
      setLedBrightness(0, 255);
      setLedMode(1, BLINK_OFF);
      setLedBrightness(1, 255);
      setLedMode(2, BLINK_OFF);
      setLedBrightness(2, 255);
      break;
    case 21:
    case 22:
    case 23:
      setLedBrightness(nt.title - 21, nt.packet.value, nt.packet.var);
      break;
    case 31:
    case 32:
    case 33:
      setLedMode(nt.title - 31, (blinkmode_t)nt.packet.value, nt.packet.var);
      break;
    case 40:
      setLedMode(0, BLINK_OFF, true);
      vTaskDelay(pdMS_TO_TICKS(300));
      setLedMode(1, BLINK_OFF, true);
      vTaskDelay(pdMS_TO_TICKS(300));
      setLedMode(2, BLINK_OFF, true);
    break;  
    case 112:
      setLedMode(0, BLINK_OFF, true);
      vTaskDelay(pdMS_TO_TICKS(300));
      setLedMode(1, BLINK_OFF, true);
      vTaskDelay(pdMS_TO_TICKS(300));
      setLedMode(2, BLINK_OFF, true);
    break;  
    }

  }
}

void LEDTask::cleanup()
{

  xTimerDelete(_timer, 0);

  for (uint8_t i = 0; i < LEDS_COUNT; i++)
  {
    if (band_pins[i] <= 0)
      break;

    ledc_stop(SPEED_MODE, channels[i], 0);

    gpio_reset_pin((gpio_num_t)band_pins[i]);
    delete (led[i]);
  }
  //}
}

void LEDTask::timerCb(TimerHandle_t tm)
{
  LEDTask *lt = static_cast<LEDTask *>(pvTimerGetTimerID(tm));
  if (lt)
    lt->timerCallback();
}

uint8_t LEDTask::getStepValue(uint8_t idx, uint8_t max)
{

  const uint8_t FADES[FADES_SIZE] = {0, 8, 16, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248, 255};
  if (idx >= sizeof(FADES))
    return 0;
  uint16_t index = idx * max;
  index /= 255;
  return FADES[index];
}

void LEDTask::reinitDuty(uint8_t idx, uint8_t d)
{

  ledc_set_duty(SPEED_MODE, led[idx]->getChannel(), d);
  ledc_update_duty(SPEED_MODE, led[idx]->getChannel());
}

void LEDTask::timerCallback()
{

  // multiplier for timer period
  uint32_t duty;
  bool change_light;
  for (uint8_t i = 0; i < LEDS_COUNT; i++)
  {
    change_light = false;
    if (!led[i])
      continue;
    if (led[i]->getMode() < BLINK_05HZ)
      continue;
    if (led[i]->getMode() <= BLINK_4HZ)
    {

      if (led[i]->getValue() == 0)
      {
        duty = led[i]->getBrightness();
        change_light = true;
      }
      else if (led[i]->getValue() == 2)
      {
        duty = 0;
        change_light = true;
        // Serial.println("BLINK TIMER");
      }
      if (led[i]->incValue() >= (led[i]->getMode() == BLINK_05HZ ? 40 : led[i]->getMode() == BLINK_1HZ ? 20
                                                                    : led[i]->getMode() == BLINK_2HZ   ? 10
                                                                                                       : 5))
      {
        led[i]->setValue(0);
      }

      if (change_light)
      {
        reinitDuty(i, duty);
      }
    }
    else if (led[i]->getMode() < BLINK_SUNRAISE)
    { // BLINK_FADEIN, BLINK_FADEOUT, BLINK_FADEINOUT

      reinitDuty(i, getStepValue(led[i]->absValue(), led[i]->getBrightness()));
      if (led[i]->getMode() == BLINK_FADEIN)
      {
        if (led[i]->incValue() >= FADES_SIZE)
        {
          led[i]->setValue(0);
        }
      }
      else if (led[i]->getMode() == BLINK_FADEOUT)
      {
        if (led[i]->decValue() < 0)
        {
          led[i]->setValue(FADES_SIZE - 1);
        }
      }
      else if (led[i]->getMode() == BLINK_FADEINOUT)
      {
        if (led[i]->incValue() >= FADES_SIZE)
        {
          led[i]->setValue(-FADES_SIZE + 1);
        }
      }
    }
    else
    { // BLINK_SUNRAISE, BLINK_SUNSET

      int8_t bright = led[i]->getValue() / SUM_MULT;
      change_light = led[i]->getValue() % SUM_MULT == 0;

      if (led[i]->getMode() == BLINK_SUNRAISE)
      {
        if (bright > FADES_SIZE - 1)
        {
          setLedMode(i, BLINK_ON);
          //if (i==2){
          //event_t ev;
          //ev.state=LED_EVENT;
          //ev.button=i+20;
          //ev.count=0;
          //ev.data=0;
          //xQueueSend(que,&ev,portMAX_DELAY);
          //}
          return;
        }
      }
      else if (led[i]->getMode() == BLINK_SUNSET)
      {
        if (bright > FADES_SIZE - 1)
        {
          setLedMode(i, BLINK_OFF);
           if (i==2){
           event_t ev;
           ev.state=LED_EVENT;
           ev.button=i+10;
          // ev.count=0;
          // //какие реле выключить
          // //ev.data=1<<24 & 0xFF000000 | 1<<16 & 0x00FF0000 | 0<<8 & 0x0000FF00 | 0 & 0x000000FF;
           xQueueSend(que,&ev,portMAX_DELAY);
           }
          
          return;
        }
      }

      if (change_light)
      {
        int8_t index = led[i]->getMode() == BLINK_SUNRAISE ? bright : FADES_SIZE - 1 - bright;
        reinitDuty(i, getStepValue(index, led[i]->getBrightness()));
      }
      led[i]->incValue();
    }
  }
}
