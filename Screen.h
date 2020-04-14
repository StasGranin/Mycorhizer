//Mycorhizer Screen Utils

#include "Arduino.h"
#include "math.h"
#include "LiquidCrystal_I2C.h"
#include "Config.h"
#include "Device.h"
#include "Button.h"

#ifndef MSU_h
#define MSU_h

#define DEVICE_ICON_STATE_OFF 0
#define DEVICE_ICON_STATE_ON 1
#define DEVICE_ICON_STATE_BLINK 2
#define DEVICE_ICON_STATE_INV 3
#define DEVICE_ICON_STATE_INV_BLINK 4

#define DEFAULT_DEVICE_ICON 0x00,0x04,0x0E,0x04,0x00,0x0E,0x00,0x00


class MycoScreen
{
	public:
		MycoScreen(uint8_t lcdAddr = SCREEN_ADDRESS);
		void init(void);
		void update(float temp, float humidity);
		void on(void);
		void off(void);
		bool isOn(void);
		void attachButton(MycoButton *button);
		void attachDevice(MycoDevice *device, byte iconArray[8]); // Attach device with icon
		void attachDevice(MycoDevice *device); // Attach device without icon. Default icon would be used

	private:
		struct ScreenDevice 
		{
			MycoDevice *device;
			byte deviceIndex;
			byte iconBitmap[8];
			byte iconBitmapInverted[8];
			byte iconX;
			byte iconY;
			byte iconState;
			bool iconIsInverted;
		};
		LiquidCrystal_I2C _lcd; // This should not be hardcoded. A better way would be to provide a pointer to a compatible driver. Somehow...
		MycoButton *_button;
		ScreenDevice _devices[MAX_DEVICES];
		unsigned long _prevRefreshMillis;
		unsigned long _prevBlinkMillis;
		bool _blinkState;
		bool _isOn;
		byte _numOfAttachedDevices;

		//void _drawIcon(DeviceIcon iconBitmap);
		void _attachDevice(MycoDevice *device, byte iconArray[8]);
		void _updateDeviceStates(void);
		void _setIconState(ScreenDevice *device, byte state);
		void _organizeDeviceIcons(void);
		void _renderIcons(void);
};

#endif