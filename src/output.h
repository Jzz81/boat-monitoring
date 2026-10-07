#pragma once

#include <Arduino.h>

void outputInfo(const char* source, const String& message);
void outputWarning(const char* source, const String& message);
void outputError(const char* source, const String& message);
void outputSerial(const String& value);
void outputSerial(const char* value);
void outputSerial(int value);
void outputSerial(unsigned int value);
void outputSerial(long value);
void outputSerial(unsigned long value);
void outputSerial(float value);
void outputSerial(double value);
void outputSerial(bool value);