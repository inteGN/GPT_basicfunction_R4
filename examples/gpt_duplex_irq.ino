//************************************************
//  FILE        :gpt_duplex_irq.ino
//  DATE        :2025/11/03
//  DESCRIPTION :GPT irq over irq example
//  BOARD TYPE  :UNO R4 MINIMA
//  AUTHOR      :inteGN
//************************************************
/*
Turns an LED on for one second, then off for one second, repeatedly.

This program is driven by two timer interrupts on one timer count resistor.
GPT7 is set as 50 microsecond free-run counter and calls ISR1 when its count up.
ISR1 internally count from 0 to 19 and trigger ISR2 as compare-match interrupt
when count is reset after reached to 19. Therefore ISR2 is called every 1 millisecond.
Equally, ISR2 internally count from 0 to 999 and turns LED on/off every 1 second
when count is reset after reached to 999.
*/

//// Pin connection
//  - not necessary
//

//// Includes
#include <Arduino.h>
#include <GPT_basicfunction.h>

//// Definitions
#define     __NOP2            asm volatile ( \
                                "nop   \n" \
                                "nop   \n" \
                                )

//// Grobals
GPTFunction  myGpt;
uint8_t   ledState  = 0;
uint8_t   flag1sec  = 0;
uint32_t  irq1Count = 0;
uint32_t  irq2Count = 0;
uint32_t  secCount  = 0;


//// ISR1 task every 50 microsecond
void  irq_gptovf_callback() {
  digitalWrite(1, HIGH);            //Signal out for test
//ISR task to trigger another ISR
  if (++irq1Count > 19) {
    irq1Count = 0;
    R_GPT7->GTCCR[GTCCRA] = 1200;   //Trigger at 25 microsecond
  }
  else {
    R_GPT7->GTCCR[GTCCRA] = 2500;   //No-trigger
  }
//Termination of Irq
  R_GPT7->GTST_b.TCFPO = 0;         //Overflow irq status clear
  __NOP2;
  digitalWrite(1, LOW);             //Signal out for test
  myGpt.clearInterruptFlag();
}


//// ISR2 task every 1 millisec
void  irq_gptcma_callback() {
  digitalWrite(2, HIGH);            //Signal out for test
//LED blink
  if (++irq2Count > 999) {
    irq2Count = 0;
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState); 
    flag1sec++;
  }
//delay 100 microsecond for intentionally duplex irq test
  delayMicroseconds(100);
//Termination of Irq
  R_GPT7->GTST_b.TCFA = 0;          //Compare-match-A irq status clear
  __NOP2;
  digitalWrite(2, LOW);             //Signal out for test
  myGpt.clearInterruptFlag();
}


//// Setup function
void setup() {
//Setup general port
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(1, OUTPUT);
  digitalWrite(1, LOW);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
//Setup GPT7 and callback function
  noInterrupts();
  myGpt.begin(7, 2400, TIMER_SOURCE_DIV_1);               //Set GPT7 period to 50us (50e-6*48e6)
  myGpt.setCallback(OVERFLOW, 2, irq_gptovf_callback);    //Set callback function with priorty 2 (higher)
  myGpt.setCallback(COMPARE_A, 15, irq_gptcma_callback);  //Set callback function with priorty 15 (lower)
  R_GPT7->GTCCR[GTCCRA] = 0xA5A5;                         //Set some value greater than 2400
  R_GPT7->GTBER = 0x00000003;                             //Does not use buffer mode
  R_GPT7->GTST_b.TCFPO = 0;                               //Overflow irq status clear
  R_GPT7->GTST_b.TCFA  = 0;                               //Compare-match-A irq status clear
  R_GPT0->GTSTR_b.CSTRT7 = 1;                             //Start GPT7
  interrupts();
}


//// Loop function
void loop() {
  if (flag1sec > 0) {
    Serial.println(secCount++);     //Time print every 1 second
    flag1sec = 0;
  }
  delayMicroseconds(10);

}   //End of loop()



