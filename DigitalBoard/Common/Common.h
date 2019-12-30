#pragma once

// internal
#include <Arduino.h>
#include <EEPROM.h>
#include <Esp.h>

// wifi
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// lcd
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ws2812b
#define FASTLED_INTERNAL
#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"

// general string type
typedef const __FlashStringHelper* FlashString;

#ifdef __INTELLISENSE__
// placement new fix
inline void* operator new(unsigned, void* ptr)
{
	return ptr;
}
#endif

#ifdef VM_DEBUG
#define DEBUG_MESSAGE(message) Serial.println(message)
#else
#define DEBUG_MESSAGE(message) (void) 0
#endif

#ifndef DEF_PLAYER_COUNT
#define DEF_PLAYER_COUNT 4
#endif

#ifndef DEF_INTERFACE_COUNT
#define DEF_INTERFACE_COUNT DEF_PLAYER_COUNT
#endif
