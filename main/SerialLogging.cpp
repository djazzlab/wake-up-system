/**
 * SerialLogging.cpp - Library to help writting to the Serial
 * Created by Xavier Beurois, October 16, 2021.
 * Released into the public domain.
 */

#include "Arduino.h"
#include "SerialLogging.h"

SerialLogging::SerialLogging(bool Enabled) {
  _Enabled = Enabled;
}

void SerialLogging::Print(String Message, bool NewLine = true) {
  if (_Enabled) {
    if (NewLine) {
      Serial.println(Message);
    } else {
      Serial.print(Message);
    }
  }
}

void SerialLogging::SetSpeed(int Speed) {
  if (_Enabled) {
    Serial.begin(Speed);
  }
}
