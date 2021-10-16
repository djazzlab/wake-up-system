/**
 * SerialLogging.h - Library to help writting to the Serial
 * Created by Xavier Beurois, October 16, 2021.
 * Released into the public domain.
*/

#ifndef SerialLogging_h
#define SerialLogging_h

#include "Arduino.h"

class SerialLogging
{
  public:
    SerialLogging(bool Enabled);
    void Print(String Message, bool NewLine = true);
    void SetSpeed(int Speed);
  private:
    bool _Enabled;
};

#endif
