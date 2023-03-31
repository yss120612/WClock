#ifndef __LEDCUBETASK
#define __LEDCUBETASK

#include "Task.h"
#include "Settings.h"
#include <pgmspace.h>
#include <SPI.h>


#define MATRIX_COUNT 4
#define HEIGHT 8
#define WIDTH (8 * MATRIX_COUNT)
#define FONT_HEIGHT 8
#define FONT_GAP 1
#define STOPLESS 0x7FFF

enum clockmode_t : uint8_t {NONE_MODE, WATCH_MODE, DAY_MODE};
enum skind_t : uint8_t {SCROLL_UP, SCROLL_DOWN, SCROLL_LEFT, SCROLL_RIGHT};

class LedCubeTask : public Task{
public:
  
  //MAX7219() : _ticker(Ticker()), _scrolling(nullptr) {}
  LedCubeTask(const char *name, uint32_t stack,QueueHandle_t q,MessageBufferHandle_t mess):Task(name,stack) {_scrolling=NULL;que=q;messages=mess;}
  ~LedCubeTask() {
    end();
  }

  void init(uint8_t pin);
  void begin(uint8_t bright = 7);
  void end();
  void clear();
  
  uint8_t getBrightness() const {
    return _bright;
  }
  void setBrightness(uint8_t value);
  void beginUpdate();
  void endUpdate();
  void repaint();
  bool getPixel(uint8_t x, uint8_t y);
  void setPixel(uint8_t x, uint8_t y, bool color);
  void drawPattern(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t pattern);
  void drawPattern(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *pattern);
  
  void printStr(uint8_t x, uint8_t y, const char *str);
  void printClock(uint8_t hour, uint8_t min);
  void scroll(const char *str, uint32_t tempo = 100);
  void scroll2center(const char *str, uint32_t tempo = 100);
  void noScroll();
  void animate(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t frames, const uint8_t *patterns, uint32_t tempo = 100);
  void noAnimate();
  uint8_t charWidth(char c);
  uint16_t strWidth(const char *str);

protected:
struct __attribute__((__packed__)) element_t {
    uint8_t x, y;
    char lett;
  };
  void scrollDigits(char * newtime, uint32_t tempo, skind_t kind);
  void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h,bool what);
  void printChar(uint8_t x, uint8_t y, char c);
  void printChar(element_t el);
  //static const uint8_t SCROLL_ANCHOR = 5;
  void cleanup() override;
  void setup() override;
  void loop() override;
  void beginTransaction();
  void endTransaction();
  void sendCommand(uint8_t cmd, uint8_t value, uint8_t target = 0xFF);
  char charNormalize(char c);
  const uint8_t *charPattern(char c);

  static void onTick(TimerHandle_t tm);
  void onMyTick();
  //static void onClockTick(TimerHandle_t tm);
  //void onMyClockTick();

  struct __attribute__((__packed__)) scrolling_t {
    uint16_t width;
    int16_t pos;
    uint8_t canvas[0];
  };
  struct __attribute__((__packed__)) animating_t {
    uint8_t x, y, w, h;
    uint8_t frames, frame;
    uint8_t patterns[0];
    clockmode_t cmode;
  };

 

  uint8_t _bits[WIDTH];
  
  union {
    scrolling_t *_scrolling;
    animating_t *_animating;
  };
  uint8_t _bright : 4;
  uint8_t _updating : 3;
  bool _anim : 1;
  element_t watch[5];
  static const uint8_t FONT[] PROGMEM;
  static const uint8_t CHAR_WIDTH[] PROGMEM;
  TimerHandle_t _timer;
  //TimerHandle_t _clock;
  uint8_t CS_PIN;
  clockmode_t cmode;
  //SPIClass _SPI;
  QueueHandle_t que;
  MessageBufferHandle_t messages;
  
};
#endif