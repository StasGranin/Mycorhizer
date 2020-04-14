// Mycorhizer Button
// Info in Button.h

#include "Button.h"

MycoButton::MycoButton(byte buttonPin)
{
  _pin = buttonPin;
  _isPressed = false;
  _isLongPressed = false;
}

void MycoButton::init(void)
{
  pinMode(_pin, INPUT);
}

byte MycoButton::getState(void)
{
  byte status = BUTTON_NO_PRESS;
  
  if (digitalRead(_pin) == HIGH) // Button is pressed
  {
    if (!_isLongPressed) // Prevent additional actions, no need to report anything until the button is released and pressed again
    {
      if (!_isPressed) // Started pressing
      {
        _isPressed = true;
        _startPressingMillis = millis();
      }
      else // Continuous press 
      {
        if ((unsigned long)(millis() - _startPressingMillis) >= LONG_BUTTON_PRESS_DURATION) // Long press detected
        {
          _isLongPressed = true; // This will also prevent further pressed status reporting 
          status = BUTTON_LONG_PRESSED;
        }
      }
    }
  }
  else
  {
    if (_isPressed && !_isLongPressed) // Button was releaced. No need to report short press if long press was already reported
    {
      status = BUTTON_PRESSED;
    }
    
    _isLongPressed = false;
    _isPressed = false;
  }
  
  return status;
}


