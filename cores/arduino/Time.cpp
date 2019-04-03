
#include "Time.h"
#include "Arduino.h"
#include "wiring_private.h"

static void syncRTC_16(){
  while ((RTC->MODE0.STATUS.reg & RTC_STATUS_SYNCBUSY));
}

Time::Time(unsigned int mode)
{
	timeMode = mode;
}

void Time::begin(voidFuncPtr callback, unsigned int msecond)
{
  /* enable RTC clock */
  PM->APBAMASK.reg |= PM_APBAMASK_RTC;

  /* config GCLK for RTC : enable GCLK for rtc, select GCLK_CLKCTRL_GEN02(32.768KHz)*/
  GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | (RTC_GCLK_ID << GCLK_CLKCTRL_ID_Pos) | 
							(GCLK_CLKCTRL_GEN_GCLK2_Val << GCLK_CLKCTRL_GEN_Pos));
  NVIC->ICER[0] = (uint32_t)(1 << ((uint32_t)RTC_IRQn & 0x0000001f));
  syncRTC_16();

  /* Disbale interrupt */
  RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_MASK;
  /* Clear interrupt flag */
  RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_MASK;
  /* Disable RTC module. */
  RTC->MODE0.CTRL.reg &= ~RTC_MODE0_CTRL_ENABLE;
  syncRTC_16();

  /* Initiate software reset. */
  RTC->MODE0.CTRL.reg |= RTC_MODE0_CTRL_SWRST;

  /* config DIV of RTC and mode of RTC */
  RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_MODE(0) | (RTC_DIV << 8);
  RTC->MODE1.CTRL.reg |= RTC_MODE1_CTRL_MODE(1);

  for (uint8_t i = 0; i < RTC_NUM_OF_COMP16; i++) {
    syncRTC_16();
    RTC->MODE1.COMP[i].reg = 0 & 0xffff;
  }

  RTC->MODE0.READREQ.reg |= RTC_READREQ_RCONT;

  NVIC->ISER[0] = (uint32_t)(1 << ((uint32_t)RTC_IRQn & 0x0000001f));
  syncRTC_16();
  RTC->MODE0.CTRL.reg |= RTC_MODE0_CTRL_ENABLE;
  /* enable RTC IRQ for RTC_MODE0_INTFLAG_OVF */
  RTC->MODE0.INTENSET.reg = RTC_MODE0_INTFLAG_OVF;
  irqFunc = callback;
  syncRTC_16();

  /* Write value to register. */
  RTC->MODE1.PER.reg = (CLK_RTC_CNT / 1000);
  if(timeMode = TIME_16B_COUNT)
    time16B_msecond = msecond;
}
void Time::end(void)
{
  RTC->MODE0.INTENSET.reg &= ~RTC_MODE0_INTFLAG_OVF;
  RTC->MODE0.CTRL.reg &= ~RTC_MODE0_CTRL_ENABLE;
  syncRTC_16();
  timeMode = 0;
  time16B_msecond = 0;
  count = 0;
  irqFunc = NULL;
}

void Time::IrqHandler(void)
{

  uint16_t interrupt_status = RTC->MODE0.INTFLAG.reg;
  interrupt_status &= RTC->MODE0.INTENSET.reg;

  if (interrupt_status & RTC_MODE0_INTFLAG_OVF) 
  {
    if(irqFunc != NULL && count == time16B_msecond)
    {
      count = 0;
      irqFunc();
    }
    count = count + 1;
    /* Clear interrupt flag */
    RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_OVF;
  }
}

