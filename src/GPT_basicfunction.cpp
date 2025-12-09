//************************************************
//  FILE        :GPT_basicfunction.cpp
//  DATE        :2025/11/03
//  VERSION     :1.0.3 (2025/12/09)
//  DESCRIPTION :GPT resistor setting
//  BOARD TYPE  :UNO R4 MINIMA
//  AUTHOR      :inteGN
//************************************************


#include "GPT_basicfunction.h"

bool GPTFunction::begin(uint8_t channel, uint32_t period, timer_source_div_t sd) {
  myChannel = channel;
  bool rv = true;
  if (channel < 8) {
    R_MSTP->MSTPCRD_b.MSTPD5 = 0;             //GPT320～GPT321 activation
    R_MSTP->MSTPCRD_b.MSTPD6 = 0;             //GPT162～GPT167 activation
  }
  else {
    rv = false;
  }
  if (rv) {
    rv = fsp_timer.begin(TIMER_MODE_PWM, GPT_TIMER, channel, period, 0xA5A5, sd, nullptr, nullptr);   //(period-1) -> GTPR
    rv = fsp_timer.open();
  }
  return rv;
}

bool GPTFunction::setCallback(timer_int_source_t intsrc, uint8_t priority, void (*callback)()) {
  bool wasStarted = R_GPT0->GTSTR & (1 << myChannel);
  bool rv = true;
  if (myChannel > 7) {
    rv = false;
  }
  else {
    if (intsrc == OVERFLOW) {
      rv = fsp_timer.setup_overflow_irq(priority, callback);
    }
    else if (intsrc == UNDERFLOW) {
      rv = fsp_timer.setup_underflow_irq(priority, callback);
    }
    else if (intsrc == CAPTURE_A || intsrc == COMPARE_A) {
      rv = fsp_timer.setup_capture_a_irq(priority, callback);
    }
    else if (intsrc == CAPTURE_B || intsrc == COMPARE_B) {
      rv = fsp_timer.setup_capture_b_irq(priority, callback);
    }
    else {
      rv = false;
    }
  }
  if ((rv) && (!wasStarted)) {
    fsp_timer.stop();
  }
  return rv;
}

void GPTFunction::clearInterruptFlag(void) {
  R_BSP_IrqStatusClear(R_FSP_CurrentIrqGet ());
}


