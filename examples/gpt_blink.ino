//************************************************
//  FILE        :gpt_blink.ino
//  DATE        :2025/11/03
//  DESCRIPTION :GPT periodic irq example
//  BOARD TYPE  :UNO R4 MINIMA
//  AUTHER      :inteGN
//************************************************
/*
Turns an LED on for one second, then off for one second, repeatedly.

This program is driven by periodic timer interrupt.  Arduino UNO R4 has 8
channels of General PWM Timer (GPT).  GPT0 is set as 1 second free-run counter
and interrupt trigger source of interrupt when its count up. Interrupt service
routine (ISR) is called every 1 second and turns LED on/off.
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
uint8_t   ledState = 0;
uint8_t   flag1sec = 0;
uint32_t  secCount = 0;


//// ISR task every 1 sec
void  irq_gptovf_callback() {
//LED blink
  ledState = !ledState;
  digitalWrite(LED_BUILTIN, ledState); 
  flag1sec++;
//Termination of Irq
  R_GPT0->GTST_b.TCFPO = 0;
  __NOP2;
  myGpt.clearInterruptFlag();
}


//// Setup function
void setup() {
//Setup general port
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
//Setup GPT0 and callback function
  noInterrupts();
  myGpt.begin(0, 3000000, TIMER_SOURCE_DIV_16);         //Set GPT0 period to 1 sec (1*48e6/16)
  myGpt.setCallback(OVERFLOW, 2, irq_gptovf_callback);  //Set callback function with priorty 2 (higher)
  R_GPT0->GTST_b.TCFPO = 0;                             //Overflow irq status clear
  R_GPT0->GTSTR_b.CSTRT0 = 1;                           //Start GPT0
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



