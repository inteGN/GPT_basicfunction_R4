//************************************************
//  FILE        :GPT_basicfunction.h
//  DATE        :2025/11/03
//  DESCRIPTION :header of GPT resistor setting
//  BOARD TYPE  :UNO R4 MINIMA
//  AUTHOR      :inteGN
//************************************************


#ifndef GPT_BASICFUNCTION_H
#define GPT_BASICFUNCTION_H


#include "Arduino.h"
#include "FspTimer.h"

//Conversion of resistor name to number for directly access of resistor
//レジスタ直接操作で利用できるレジスタ名から番号への読み替え
#define     GTCCRA            0
#define     GTCCRB            1
#define     GTCCRC            2
#define     GTCCRD            4
#define     GTCCRE            3
#define     GTCCRF            5

//Enumeration of cause of interrupt used in this library
//このライブラリで設定できる割り込み要因の列挙
typedef enum {
  OVERFLOW,
  UNDERFLOW,
  CAPTURE_A,
  COMPARE_A,      //Equivalent to CAPTURE_A   //CAPTURE_Aと同等
  CAPTURE_B,
  COMPARE_B       //Equivalent to CAPTURE_B   //CAPTURE_Bと同等
} timer_int_source_t;


class GPTFunction {
private:
  FspTimer fsp_timer;

public:
//Store provided timer setting values to class variables
//与えられたタイマ設定値をクラスの変数に格納する
  bool begin(uint8_t channel, uint32_t period, timer_source_div_t sd);

//Set callback function of timer GPT interrupt together with priority level and vector table
//タイマー割り込みのコールバック関数とその優先レベル、ベクタテーブルを設定する
  bool setCallback(timer_int_source_t intsrc, uint8_t priority, void (*callback)());

//Refer current cause of interrupt and clear it
//現在の割り込みフラグを参照してクリアする
  void clearInterruptFlag(void);
};


#endif


// reference - predefined interrupt priority
// from IRQManager.cpp in Arduino Uno R4 library version 1.5.1
//#define ETHERNET_PRIORITY          12
//#define SDCARD_ACCESS_PRIORITY     10
//#define SDCARD_DMA_REQ_PRIORITY    10
//#define SDCARD_CARD_PRIORITY       12
//#define EXTERNAL_PIN_PRIORITY      12
//#define UART_SCI_PRIORITY          12
//#define USB_PRIORITY               12
//#define AGT_PRIORITY               14
//#define RTC_PRIORITY               12
//#define I2C_MASTER_PRIORITY        12
//#define I2C_SLAVE_PRIORITY         12
//#define SPI_MASTER_PRIORITY        6
//#define DMA_PRIORITY               12
//#define TIMER_PRIORITY             8
//#define ADC_PRIORITY               12
//#define CAN_PRIORITY               12
//#define CANFD_PRIORITY             12
//#define I2S_PRIORITY               12
//#define FIRST_INT_SLOT_FREE         0


