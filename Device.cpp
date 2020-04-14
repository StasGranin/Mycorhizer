// Mycorhizer Device
// Info in Device.h

#include "Device.h"

MycoDevice::MycoDevice(byte deviceId, byte devicePin)// : _manualButton(deviceManualButtonPin)
{
  _outputPin = devicePin;
  _id = deviceId;
  _deviceType = UNKNOWN_DEVICE;
  _deviceState = DEVICE_STATE_OFF;
  _isOn = false;
  _targetValue = NAN;
  _button = NULL;

  _isPaused = false;
  _isManual = false;
}

// Initializes value dependant device. Can be ether raising or lowering
// Run this at setup();
void MycoDevice::init(long targetValue, byte deviceType)
{
  if (!isnan(targetValue) && (deviceType == RAISING_DEVICE || deviceType == LOWERING_DEVICE))
  {
    _targetValue = targetValue;
    _deviceType = deviceType;
    _init();
  }
  else
  {
    // error
  } 
}

// Initialize device with array or timing to on/off consequently
void MycoDevice::init(unsigned long eventTimes[MAX_DEVICE_EVENTS])
{
  _numOfEvents = sizeof(eventTimes);
  
  if (_numOfEvents > 1)
  {
    for (byte i = 0; i < MAX_DEVICE_EVENTS; i++)
    {
      _eventTimes[i] = i < _numOfEvents ? eventTimes[i] : NULL;
    }

    _currentTimingIndex = 1; // Let's start from the OFF time. No sense blasting all devices on powerup
    _eventStartMillis = 0;
    _deviceType = TIMED_DEVICE;
    _init();
  }
  else
  {
    // error
  } 
}

// Initialize aux device with a control function
void MycoDevice::init(handler fn)
{
  _auxHandler = fn;
  _deviceType = AUX_DEVICE;
  _init(); 
}

// Update the device status. Run this at main loop() 
void MycoDevice::update(long value)
{
  _update(value);
}

void MycoDevice::update()
{
  _update(0);
}

void MycoDevice::onChangeState(callback fn)
{
  _onStateChangeCallback = fn;
}

void MycoDevice::attachButton(MycoButton *button)
{
  _button = button;

  _button->init();
}

void MycoDevice::on(void)
{
  digitalWrite(_outputPin, HIGH);
  _isOn = true;
}

void MycoDevice::off(void)
{
  digitalWrite(_outputPin, LOW);
  _isOn = false;
}

bool MycoDevice::isOn(void)
{
  return _isOn;
}

void MycoDevice::pause(void)
{
  off();
  _isPaused = true;
}

void MycoDevice::unpause(void)
{
  _isPaused = false;
}

byte MycoDevice::getDeviceId(void)
{
  return _id;
}

byte MycoDevice::getState(void)
{
  return _deviceState;
}


// Private functions

void MycoDevice::_init(void)
{
  _deviceState = DEVICE_STATE_OFF;
  _lastControledState = DEVICE_STATE_OFF;
  
  pinMode(_outputPin, OUTPUT);
  digitalWrite(_outputPin, LOW);

  off(); // Just to make sure it's off
}

void MycoDevice::_update(long value)
{
  const byte state = _deviceState;
  
  if (_button)
  {
    _handleButton();
  }

  
  switch (_deviceType)
  {
    case RAISING_DEVICE:
    case LOWERING_DEVICE:
      _handleValueDependant(value);
      break;
    
    case TIMED_DEVICE:
      _handleTimeDependant();
      break;

    case AUX_DEVICE:
      _handleAuxDependant();

    default:
      return;
  }

  if (_onStateChangeCallback && state != _deviceState)
  {
    _onStateChangeCallback(_id, _deviceState);
  }

  (_deviceState == DEVICE_STATE_ON || _deviceState == DEVICE_STATE_MANUAL_ON) ? on() : off();
}

void MycoDevice::_handleButton(void)
{
  const byte buttonState = _button->getState();
  
  if (buttonState == BUTTON_PRESSED) // Short press turns manual on
  {
    if (_deviceState == DEVICE_STATE_MANUAL_ON) // If it already is, turn manual state off
    {
      _isManual = false;
      _deviceState = _lastControledState;
    }
    else
    {
      _isManual = true;
      _deviceState = DEVICE_STATE_MANUAL_ON;
    }
  }
  else if (buttonState == BUTTON_LONG_PRESSED) // Long press turns manual off
  {
    if (_deviceState == DEVICE_STATE_MANUAL_OFF) 
    {
      _isManual = false;
      _deviceState = _lastControledState;
    }
    else
    {
      _isManual = true;
      _deviceState = DEVICE_STATE_MANUAL_OFF;
    }
  }
}

void MycoDevice::_handleTimeDependant(void) //TODO: Something here is not working right... (on/off are mixed up) 
{
  const unsigned long currentMillis = millis();
  
  if (_isPaused || _isManual)
  {
    _eventStartMillis += (unsigned long)(currentMillis - _lastUpdateMillis); // Add time while waiting. This will effectively pause the timer
  }
  
  if ((unsigned long)(currentMillis - _eventStartMillis) >= _eventTimes[_currentTimingIndex] * 1000) // Advance to next task
  {
     Serial.println("=== Task ended ===");
     Serial.println("device " + (String)(_id) + ": " + (String)(_eventTimes[_currentTimingIndex] * 1000));
     Serial.println("lasted " + (String)(_eventTimes[_currentTimingIndex]) + " seconds");
    
    // Since tasks are a sequence of ON and OFF, every odd index task in the array is an OFF task
    _deviceState = _lastControledState = (_currentTimingIndex % 2 == 0) ? DEVICE_STATE_OFF : DEVICE_STATE_ON;

    if (++_currentTimingIndex >= _numOfEvents)
    {
      _currentTimingIndex = 0;
    }

    _eventStartMillis = currentMillis;
  } 
  
  _lastUpdateMillis = currentMillis;
}

void MycoDevice::_handleValueDependant(long value)
{
  if (!_isManual && !isnan(value))
  {
    _deviceState = _lastControledState = (RAISING_DEVICE && value < _targetValue || LOWERING_DEVICE && value > _targetValue) ? DEVICE_STATE_ON : DEVICE_STATE_OFF;
  } 
}

void MycoDevice::_handleAuxDependant()
{
  if (!_isManual && _auxHandler)
  {
    _deviceState = _lastControledState = (_auxHandler(_id, _deviceState)) ? DEVICE_STATE_ON : DEVICE_STATE_OFF;
  } 
}

// byte MycoDevice::_getNormalizedHumidity(byte currentHumidity) // No need to be fancy here. whole values are enought
// {
//   int accumulator = 0;
//   byte numberOfValues = 0;
//   byte humidityValue = 0;
  
//   for (int i = HUMIDITY_VALUES_ARRAY_SIZE - 1; i > 0; i--)
//   {
//     humidityValue = _previousHumidities[i-1];
    
//     _previousHumidities[i] = humidityValue;

//     if (!isnan(humidityValue))
//     {
//       accumulator += humidityValue;
//       numberOfValues++;
//     }
//   }

//   accumulator += currentHumidity;
//   _previousHumidities[0] = currentHumidity;

//   return (byte)(accumulator / (numberOfValues + 1));
// }
