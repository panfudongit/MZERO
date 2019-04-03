

#pragma once

#define RTC_DIV 0x0
#define RTC_GEN2_CLOCK 0x8000
#define CLK_RTC_CNT (RTC_GEN2_CLOCK >> RTC_DIV) // 32768KHz / RTC_DIV

#define TIME_8B_COUNT 8
#define TIME_16B_COUNT 16
#define TIME_32B_COUNT 32

typedef void (*voidFuncPtr)(void);

class Time
{
  public:
    Time(unsigned int mode);
    void begin(voidFuncPtr callback, unsigned int msecond);
    void end();
    void IrqHandler();
  private:
    unsigned char timeMode; //8B, 16B, 32B
    unsigned int time16B_msecond;
    int count;
    voidFuncPtr  irqFunc;
};


