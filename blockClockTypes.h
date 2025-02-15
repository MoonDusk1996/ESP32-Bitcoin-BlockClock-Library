#ifndef BLOCKCLOCKTYPES
#define BLOCKCLOCKTYPES

#include "userBoardDefines.h"
#ifdef M5STACK
#include <M5StickCPlus.h>
#endif
#ifdef GENERIC_ESP32
#include <Arduino.h>
#endif

#include <time.h>

#include "blockClockUtils.h"

struct PriceData {
  String price;
  CurrencyState currency;
  float change1h;
  float change24h;
  float change7d;
  float change30d;
  time_t timestamp;
  bool error;
};

struct WiFiData {
  bool connected;
  String SSID;
  int8_t SignalStrength;
};

struct RecommendedFees {
  uint16_t high;
  uint16_t medium;
  uint16_t low;
  uint16_t noPriority;
  uint16_t minimum;
  bool error;
};

#endif
