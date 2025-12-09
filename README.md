# GPT_basicfunction_R4

## 概要 / Overview
このライブラリはArduino UNO R4のGPTタイマーを使ったプログラミングを簡単にするよう作られています。  
GPTタイマーの基本的な設定と、タイマー割り込みの登録および割り込みサービスからの復帰のための関数を提供します。  
This library is designed to simplify GPT timer programming on Arduino UNO R4.    
It provides basic setup of  timer GPT,  and registration of timer interrupt callback function and return from ISR.  

---

## 特徴 / Features
- チャネルを指定してGPTタイマーの周期を設定  
- GPTタイマー要因の割り込みハンドラを優先度を指定して登録  
- 割り込みコントローラの割り込み要因クリアをサポート  
>注釈:  
>このライブラリは、そのまま何かの機能を実行するものではなく、Arduino環境においてGPTタイマーを使用するための基本的な関数を提供します。  
>このライブラリを特定のアプリケーションに適用するには、レジスタの直接操作および/またはほかのGPTタイマーライブラリがあわせて必要です。  
>強く推奨：GPTタイマーのリソース競合を避けるため、このライブラリを適用する前にモジュールストップコントロールレジスタ(MSTPD)のMSTPD5、MSTPD6と汎用PWM タイマソフトウェアストップレジスタ(GTSTP)を観察し、タイマーGPTxが使用されていないか確認すること。  
- Set timer period to selected channel of timer GPT  
- Registration of timer interrupt handler function with its priority level  
- Support to clear cause of interrupt stored in interrupt controller  
>Note:  
>This library does not brings any function to work immediately but basic function to use timer GPT on Arduino IDE.  
>If apply this library for specific application, it should be necessary to use with directly resistor access and/or other timer GPT library together.  
>Strongly recommended: To aviod timer GPT resource contention, check if timer GPTx is not used by observing MSTPD5 and MSTPD6 of Module Stop Control Register D (MSTPCRD) and General PWM Timer Software Stop Register (GTSTP) before applying this library.

---

## 想定用途 / Expected  application
- 精密な回転数測定  
- 低ジッターの定時間間隔アナログ波形サンプリング  
- PWMパルス毎のブラシレスモーター制御  
- タイムクリティカルなタスクスケジューラー  
- タイマーと割り込み処理の学習を含んだ教育プロジェクト  
>注釈  
>このライブラリは汎用目的のためレジスタの特定の設定を意図的に避けており、ユーザーは幅広いアプリケーションに適応できます。    
- High precision revolution speed measurement  
- Constant intervals analog wave acquisition with reduced jitter  
- Pulse-by-pulse BLDC motor control  
- Time-critical task scheduling  
- Educational projects which includes timer and interrupt process learning  
>NOTE  
>This library is general-purpose and intentionally avoids specific register configurations,  allowing users to adapt it to a wide range of applications.  

---

## 対応ハードウェア / Supported Hardware
- Arduino Uno R4 MINIMA, NANO (both tested), WiFi (not tested, may work)  
- Other board with renesas RA4M1  

---

## 使い方 / Usage

```cpp
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

//// Grobals
GPTFunction  myGpt;
uint8_t   ledState = 0;
uint8_t   flag1sec = 0;
uint32_t  secCount = 0;


//// ISR task every 1sec
void  irq_gptovf_callback() {
//LED blink
  ledState = !ledState;
  digitalWrite(LED_BUILTIN, ledState); 
  flag1sec++;
//Termination of Irq
  R_GPT0->GTST_b.TCFPO = 0;
  myGpt.retCallback();
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
  myGpt.begin(0, 3000000, TIMER_SOURCE_DIV_16);         //Set GPT0 period to 1sec (1*48e-6/16)
  myGpt.setCallback(OVERFLOW, 2, irq_gptovf_callback);  //Set callback function with priorty 2 (higher)
  R_GPT0->GTST_b.TCFPO = 0;                             //Overflow irq status clear
  R_GPT0->GTSTR_b.CSTRT0 = 1;                           //Start GPT0
  interrupts();
}


//// Loop function
void loop() {
  if (flag1sec > 0) {
    Serial.println(secCount++);     //Time print every 1second
    flag1sec = 0;
  }
  delayMicroseconds(10);

}   //End of loop()

```

---

## API

### begin(uint8_t channel, uint32_t period, timer_source_div_t sd)
与えられたタイマ設定値をクラスの変数に格納する  
Store provided timer setting values to class variables  

### setCallback(TimerIntSource_t intsrc, uint8_t priority, void (*callback)())
タイマー割り込みのコールバック関数とその優先レベル、ベクタテーブルを設定する  
Set callback function of timer GPT interrupt together with priority level and vector table  

### clearInterruptFlag(void)
現在の割り込みフラグを参照してクリアする  
Refer current cause of interrupt and clear it  

---

## Revision history

1.0.0 First release  
1.0.1 Name 'TimerIntSource_t' change to lower-case  
1.0.2 Name 'retCallback()' change to 'clearInterruptflag()'  
1.0.3 The timer start status follows as previous one after setCallback() process  

---

## License
Copyright (c) 2025 inteGN - MIT License  

