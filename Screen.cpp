// Mycorhizer Screen Handler
// Info in Screen.h

#include "Screen.h"

MycoScreen::MycoScreen(uint8_t lcdAddr = SCREEN_ADDRESS) : _lcd(lcdAddr, SCREEN_WIDTH, SCREEN_HEIGHT)
{
  _isOn = true;
  _button = NULL;
  _prevRefreshMillis = 0;
  _prevBlinkMillis = 0;
  _blinkState = false;
  _numOfAttachedDevices = 0;
}

void MycoScreen::init(void)
{
  _lcd.init();
  _lcd.backlight();
}

void MycoScreen::update(float temp, float humidity)
{
  if (_button && _button->getState() == BUTTON_PRESSED)
  {
    _isOn ? off() : on();
  }
  
  if (_isOn) // Operations for working display only
  {
    if ((unsigned long)(millis() - _prevRefreshMillis) >= SCREEN_REFRESH_INERVAL) 
    {
      if (temp && humidity) // TODO: handle screen clearing
      {
        _lcd.setCursor(0,0);
        _lcd.print("Temp: " + String(temp, 1));
        _lcd.write(0xDF); // degree char
        _lcd.setCursor(0,1);
        _lcd.print("Humd: " + String(humidity, 1) + '%');
      }
      else
      {
        //TODO: display error
      }

      _renderIcons();
      _updateDeviceStates();

      _prevRefreshMillis = millis();
    }
  }
  
  // Flip the blink state
  if ((unsigned long)(millis() - _prevBlinkMillis) >= GLOBAL_BLINK_INTERVAL) 
  {
    _blinkState = !_blinkState;
    _prevBlinkMillis = millis();
  }
}

void MycoScreen::attachButton(MycoButton *button)
{
  _button = button;

  _button->init();
}

void MycoScreen::attachDevice(MycoDevice *device, byte iconBitmap[8]) // Accepts a device object and its iconBitmap pictogram and adds it to devices array
{
  _attachDevice(device, iconBitmap);
}

void MycoScreen::attachDevice(MycoDevice *device) // Accepts a device object and its iconBitmap pictogram and adds it to devices array
{
  byte iconBitmap[8] = {DEFAULT_DEVICE_ICON};
  
  _attachDevice(device, iconBitmap);
}

void MycoScreen::on(void)
{
  _isOn = true;
  _lcd.backlight();  
  _lcd.display();  
}

void MycoScreen::off(void)
{
  _isOn = false;
  _lcd.noBacklight();  
  _lcd.noDisplay();  
}

bool MycoScreen::isOn(void)
{
  return _isOn;
}

void MycoScreen::_attachDevice(MycoDevice *device, byte iconBitmap[8])
{
  ScreenDevice *newDevice = &_devices[_numOfAttachedDevices]; // Take the poiner of the next available ScreenDevice struct in the devices array 
  
  newDevice->device = device;
  newDevice->deviceIndex = _numOfAttachedDevices;
  newDevice->iconX = 0;
  newDevice->iconY = 0;
  newDevice->iconState = DEVICE_ICON_STATE_OFF;
  newDevice->iconIsInverted = false;

  for (byte i=0; i<7; i++)
  {
    newDevice->iconBitmap[i] = iconBitmap[i];
    newDevice->iconBitmapInverted[i] = ~iconBitmap[i]; // Yep, that's all it takes
  }

  newDevice->iconBitmap[7] = 0x00; // Last line of the iconBitmap is always empty. For visual appearance
  newDevice->iconBitmapInverted[7] = 0x00;

  _lcd.createChar(newDevice->deviceIndex, newDevice->iconBitmap); // Let's create it, just to be on the safe side

  _numOfAttachedDevices++;

  _organizeDeviceIcons();
}

void MycoScreen::_organizeDeviceIcons(void) // TODO: There's a bug in here somewhere
{
  const byte rows = _numOfAttachedDevices < SCREEN_HEIGHT ? _numOfAttachedDevices : SCREEN_HEIGHT;
  const byte columns = (byte)(_numOfAttachedDevices / rows) + (_numOfAttachedDevices % rows > 0 ? 1 : 0); // Yes, because ceil() doesn't work for some reason. 8bit bullshit?
  byte index = 0;
  
  for (byte y = 0; y < rows; y++)
  {
    for (byte x = 0; x < columns; x++)
    {
      if ((y == rows - 1) && (x < _numOfAttachedDevices % columns)) continue; // Probbably there's a crazier way to do this without the if()  

      _devices[index].iconX = SCREEN_WIDTH - (columns - x);
      _devices[index].iconY = y;

      index++;
    }
  }
}


void MycoScreen::_updateDeviceStates(void)
{
  for (byte i=0; i<_numOfAttachedDevices; i++)
  {
    const byte deviceId = _devices[i].device->getDeviceId();
    byte newIconState;
    
    switch(_devices[i].device->getState()) // This should probably be configured from the outside
    {
      case DEVICE_STATE_ON:
        newIconState = DEVICE_ICON_STATE_ON;
        break;

      case DEVICE_STATE_MANUAL_OFF:
        newIconState = DEVICE_ICON_STATE_INV_BLINK;
        break;

      case DEVICE_STATE_MANUAL_ON:
        newIconState = DEVICE_ICON_STATE_INV;
        break;
      
      case DEVICE_STATE_OFF:
      default:
        newIconState = DEVICE_ICON_STATE_OFF;
        
    }

    _setIconState(&_devices[i], newIconState);
  }
}

void MycoScreen::_setIconState(ScreenDevice *device, byte newIconState)
{
  if ((device->iconState != newIconState) && (newIconState != DEVICE_ICON_STATE_OFF)) // State was changed. No need to create new iconBitmap if new state is off
  {
    if ((newIconState == DEVICE_ICON_STATE_ON || newIconState == DEVICE_ICON_STATE_BLINK) && device->iconIsInverted)
    {
      _lcd.createChar(device->deviceIndex, device->iconBitmap);
      device->iconIsInverted = false;
    }
    else if ((newIconState == DEVICE_ICON_STATE_INV || newIconState == DEVICE_ICON_STATE_INV_BLINK)  && !device->iconIsInverted)
    {
      _lcd.createChar(device->deviceIndex, device->iconBitmapInverted);
      device->iconIsInverted = true;
    }
  }

  device->iconState = newIconState;
}

void MycoScreen::_renderIcons(void)
{
  for (byte i=0; i<_numOfAttachedDevices; i++)
  {
    ScreenDevice *device = &_devices[i];

    _lcd.setCursor(device->iconX, device->iconY);

    if (device->iconState == DEVICE_ICON_STATE_OFF || ((device->iconState == DEVICE_ICON_STATE_BLINK || device->iconState == DEVICE_ICON_STATE_INV_BLINK) && !_blinkState))
    {
      _lcd.print(" ");
    }
    else
    {
      _lcd.write(device->deviceIndex);
    }
  }
}


