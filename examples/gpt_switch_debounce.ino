//************************************************
//  FILE        :gpt_switch_debounce.ino
//  DATE        :2025/11/18
//  DESCRIPTION :switch debounce in 1 msec irq
//  BOARD TYPE  :UNO R4 MINIMA
//  AUTHER      :inteGN
//************************************************

/*
1msec毎の割り込みを使い、D2につながれたスイッチのチャタリングを除去しLEDに出力する
 - ISRが呼ばれる毎にSWbuffer_2を左シフトし最下位ビットにスイッチの最新値を入れる
 - 0または1が何個連続したかをSWwindowと比較し（ここでは12個）、結果をSWdetect_2に入れる
 - loop()内でSWdetect_2をLED_TXに出力する（回路構成上、スイッチがONのときLED_TXが点灯する）
 - setup()内であらかじめSWdetect_2とSWbuffer_2を初期化しておく

In every 1 msec interrupt, read switch and output its value to LED port with debouncing.
 - Put newest switch value into LSB of SWbuffer_2 after its left 1 bit shift at called interrupt service routine.
 - Compare to SWwindow how many 0s or 1s are continued (12 in this example) and store the result in SWdetect_2.
 - Output SWdetect_2 to LED_TX in loop() function (circuitly, LED_TX is light while switch is ON).
 - Initialise SWdetect_2 and SWbuffer_2 before above process are excecuted.
*/

//// Pin connection
//  - ポートD2とGNDの間にスイッチを接続
//  - Switch connect between D2 port and GND
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
uint32_t  irqCount = 0;
uint8_t   flag1sec = 0;
uint32_t  secCount = 0;
//チャタリング除去のexampleで使うグローバル変数   //grovals used for debouncing
uint8_t   SWdetect_2;               //チャタリングが除去された検出値、0=LOW, 1=HIGH   //debouced value
uint16_t  SWbuffer_2;               //スイッチ状態を順に記憶するバッファ              //buffer for debouncing
const uint16_t SWwindow = 0x0FFF;   //連続n個を判断するための判定値、0x0FFF=連続12個  //conparison window


//// ISR task every 1 msec
void  irq_gptovf_callback() {
//D2を入力してn個連続で0か1なら確定する   //determine if 0s or 1s are continued n times.
  SWbuffer_2 = (SWbuffer_2 << 1);
  if (digitalRead(2) != 0) {SWbuffer_2++;}
  if ((SWbuffer_2 & SWwindow) == 0x0000)   {SWdetect_2 = 0;}
  if ((SWbuffer_2 & SWwindow) == SWwindow) {SWdetect_2 = 1;}
//1sec毎にLED＿BUILTINを点滅　　//blink LED_BUILTIN every 1 sec
  if (++irqCount > 999) {
    irqCount = 0;
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState); 
    flag1sec++;
  }
//Termination of Irq
  R_GPT0->GTST_b.TCFPO = 0;
  __NOP2;
  myGpt.clearInterruptFlag();
}


//// Setup function
void setup() {
//Setup serial Interface
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(LED_TX, OUTPUT);
  pinMode(LED_RX, OUTPUT);
  digitalWrite(LED_TX, HIGH);
  digitalWrite(LED_RX, HIGH);
  //D2を入力に設定し、最初に1回読んでスイッチの検出値とバッファを初期化する   //Setup port D2 and initialise SWdetect_2 and SWbuffer_2
  pinMode(2, INPUT_PULLUP);
  if (digitalRead(2) == 0) {
    SWdetect_2 = 0;
    SWbuffer_2 = 0x0000;
  }
  else {
    SWdetect_2 = 1;
    SWbuffer_2 = 0xFFFF;
  }
//Setup GPT0 and callback function
  noInterrupts();
  myGpt.begin(0, 48000, TIMER_SOURCE_DIV_1);            //Set GPT0 period to 1 msec (1e-3*48e6/1)
  myGpt.setCallback(OVERFLOW, 2, irq_gptovf_callback);  //Set callback function with priorty 2 (higher)
  R_GPT0->GTST_b.TCFPO = 0;                             //Overflow irq status clear
  R_GPT0->GTSTR_b.CSTRT0 = 1;                           //Start GPT0
  interrupts();
}


//// Loop function
void loop() {
//チャタリングを除去した結果をLED_TXに出力する    //Output debounced value to LED_TX
  digitalWrite(LED_TX, SWdetect_2);
//Time print every 1second
  if (flag1sec > 0) {
    Serial.println(secCount++);
    flag1sec = 0;
  }
  delayMicroseconds(10);

}   //End of loop()



